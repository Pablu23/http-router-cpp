#ifndef ROUTER_H
#define ROUTER_H

#include "request.hpp"
#include "response.hpp"
#include "tree.hpp"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <string>

namespace http {
class Router {
private:
  std::map<std::string, std::shared_ptr<Tree>> m_routes;
  int m_socket;
  sockaddr_in m_address;
  Response Route(Request req);

private:
  std::mutex m_mutex;
  std::condition_variable m_cond;
  std::vector<std::thread> m_threads;
  std::queue<int> m_clients;
  bool m_should_terminate = false;
  void start_thread_loop();
  void thread_loop();
  void queue_client(int client);
  void stop_thread_loop();

public:
  Router(int port);
  void handle(std::string path_pattern,
              std::function<void(Request, Response *)> func);
  int start();
  int stop();
};
} // namespace http

#endif // !ROUTER_H
