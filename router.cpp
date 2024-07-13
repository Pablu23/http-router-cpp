#include "router.hpp"
#include "http.hpp"
#include <csignal>
#include <cstdint>
#include <mutex>
#include <strings.h>

using namespace http;

Router::Router(int port) {
  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  m_address.sin_family = AF_INET;
  m_address.sin_port = htons(port);
  m_address.sin_addr.s_addr = INADDR_ANY;
}

int Router::Start() {
  int err = bind(m_socket, (struct sockaddr *)&m_address, sizeof(m_address));
  if (err != 0)
    return err;

  StartThreadLoop();

  err = listen(m_socket, 5);
  if (err != 0)
    return err;

  while (true) {
    int client = accept(m_socket, nullptr, nullptr);
    QueueClient(client);
  }

  StopThreadLoop();
}

void Router::QueueClient(int fd) {
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_clients.push(fd);
  }
  m_cond.notify_one();
}

void Router::StartThreadLoop() {
  const uint32_t numThreads = std::thread::hardware_concurrency();
  for (uint32_t i = 0; i < numThreads; ++i) {
    m_threads.emplace_back(std::thread(&Router::ThreadLoop, this));
  }
}

void Router::StopThreadLoop() {
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_shouldTerminate = true;
  }
  m_cond.notify_all();
  for (std::thread &active_thread : m_threads) {
    active_thread.join();
  }
  m_threads.clear();
}

void Router::ThreadLoop() {
  std::vector<std::byte> buffer(1024);
  while (true) {
    int client;
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cond.wait(lock,
                  [this] { return !m_clients.empty() || m_shouldTerminate; });
      if (m_shouldTerminate)
        return;
      client = m_clients.front();
      m_clients.pop();
    }

    recv(client, buffer.data(), buffer.size(), 0);
    Request req(buffer);
    Response res = Route(req);
    res.Send(client);
    close(client);
  }
}

void Router::Handle(std::string pathPattern,
                    std::function<Response(Request, Response)> func) {
  m_routes.insert_or_assign(pathPattern, func);
}

// This should be better
// Probably dont use map but a tree for it, then traverse tree for routing
Response Router::Route(Request req) {
  for (const auto &[key, value] : m_routes) {
    std::string pattern = key;

    int mPos = pattern.find(' ');
    std::string method = pattern.substr(0, mPos);

    if (mPos != -1 && method != req.Method()) {
      continue;
    }

    pattern.erase(0, mPos + 1);
    std::string patternCopy = pattern;
    std::string path = req.path.Base();
    bool found = false;
    int pos = 0;
    while (pos != -1) {
      found = true;
      pos = pattern.find('/');
      std::string p = pattern.substr(0, pos);

      int uPos = path.find('/');
      std::string u = path.substr(0, uPos);

      if (!p.starts_with('{') && strcasecmp(p.data(), u.data()) != 0) {
        found = false;
        break;
      }

      pattern.erase(0, pos + 1);
      path.erase(0, uPos + 1);
    }
    if (found) {
      Response res(statuscode::OK);
      req.path.Match(patternCopy);
      return value(req, res);
    }
  }

  return Response(statuscode::NOT_FOUND);
}
