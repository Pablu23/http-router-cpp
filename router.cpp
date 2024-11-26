#include "router.hpp"
#include "http.hpp"
#include "util.hpp"
#include <csignal>
#include <cstdint>
#include <iostream>
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
  m_running = false;
}

Router::~Router() {
  for (auto t : m_routes) {
    delete t.second;
  }
}

int Router::start() {
  m_running = true;
  int err = bind(m_socket, (struct sockaddr *)&m_address, sizeof(m_address));
  if (err != 0)
    return err;

  start_thread_loop();

  err = listen(m_socket, 5);
  if (err != 0)
    return err;

  while (m_running.load()) {
    int client = accept(m_socket, nullptr, nullptr);
    queue_client(client);
  }

  stop_thread_loop();
  return 0;
}

void Router::stop() { m_running.store(false); }

void Router::queue_client(int fd) {
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_clients.push(fd);
  }
  m_cond.notify_one();
}

void Router::start_thread_loop() {
  const uint32_t numThreads = std::thread::hardware_concurrency();
  for (uint32_t i = 0; i < numThreads; ++i) {
    m_threads.emplace_back(std::thread(&Router::thread_loop, this));
  }
}

void Router::stop_thread_loop() {
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_should_terminate = true;
  }
  m_cond.notify_all();
  for (std::thread &active_thread : m_threads) {
    active_thread.join();
  }
  m_threads.clear();
}

void Router::thread_loop() {
  std::vector<std::byte> buffer(1024);
  while (true) {
    int client;
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cond.wait(lock,
                  [this] { return !m_clients.empty() || m_should_terminate; });
      if (m_should_terminate)
        return;
      client = m_clients.front();
      m_clients.pop();
    }

    recv(client, buffer.data(), buffer.size(), 0);
    Request req(buffer);
    Response res = Route(req);
    auto payload = res.compile();
    send(client, payload.data(), payload.size(), 0);

    shutdown(client, SHUT_WR);
    while (recv(client, buffer.data(), buffer.size(), 0) > 0) {
      std::cout << buffer.data() << std::endl;
    }
    close(client);
  }
}

void Router::handle(std::string pathPattern,
                    std::function<void(Request, Response *)> func) {
  auto route = split(pathPattern, " ");
  // TODO: UNSAFE CHECK BOUNDS
  auto tree = m_routes[route[0]];
  if (!tree) {
    tree = new Tree(route[0]);
    m_routes.insert_or_assign(route[0], tree);
  }
  tree->add_path(route[1], func);
}

Response Router::Route(Request req) {
  auto tree = m_routes[req.method()];
  auto route = tree->get(req.path.base());

  if (!route.has_value()) {
    return Response(StatusCode::NOT_FOUND);
  }

  Response res(StatusCode::OK);
  route.value()(req, &res);
  return res;
}
