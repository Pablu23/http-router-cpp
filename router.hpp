#ifndef ROUTER_H
#define ROUTER_H

#include "request.hpp"
#include "response.hpp"
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <string>

class Router {
private:
  std::map<std::string, std::function<Response(Request, Response)>> m_routes;
  int m_socket;
  sockaddr_in m_address;
  Response Route(Request req);

private:
  std::mutex m_mutex;
  std::condition_variable m_cond;
  std::vector<std::thread> m_threads;
  std::queue<int> m_clients;
  bool m_shouldTerminate = false;
  void StartThreadLoop();
  void ThreadLoop();
  void QueueClient(int client);
  void StopThreadLoop();

public:
  Router(int port);
  void Handle(std::string pathPattern,
              std::function<Response(Request, Response)> func);
  int Start();
  int Stop();
};

#endif // !ROUTER_H
