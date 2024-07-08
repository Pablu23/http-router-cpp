#ifndef ROUTER_H
#define ROUTER_H

#include "request.hpp"
#include "response.hpp"
#include <functional>
#include <map>
#include <netinet/in.h>
#include <string>

class Router {
private:
  std::map<std::string, std::function<Response(Request, Response)>> m_routes;
  int m_socket;
  sockaddr_in m_address;
  Response Route(Request req);

public:
  Router(int port);
  void Handle(std::string pathPattern,
              std::function<Response(Request, Response)> func);
  int Start();
  int Stop();
};

#endif // !ROUTER_H
