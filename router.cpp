#include "router.hpp"
#include "http.hpp"
#include "util.hpp"
#include <csignal>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

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

    shutdown(client, SHUT_WR);
    while (recv(client, buffer.data(), buffer.size(), 0) > 0) {
      std::cout << buffer.data() << std::endl;
    }
    close(client);
  }
}

void Router::Handle(std::string pathPattern,
                    std::function<void(Request, Response *)> func) {
  auto route = split(pathPattern, " ");
  // TODO: UNSAFE CHECK BOUNDS
  auto tree = m_routes[route[0]];
  if (!tree) {
    tree = std::make_shared<Tree>(Tree(route[0]));
    m_routes.insert_or_assign(route[0], tree);
  }
  tree->AddPath(route[1], func);
}

Response Router::Route(Request req) {
  auto tree = m_routes[req.Method()];
  auto route = tree->Get(req.path.Base());

  if (!route.has_value()) {
    return Response(statuscode::NOT_FOUND);
  }

  Response res(statuscode::OK);
  route.value()(req, &res);
  return res;
}
