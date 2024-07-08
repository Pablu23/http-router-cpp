#include "router.hpp"
#include <algorithm> // std::equal
#include <cctype>    // std::tolower
#include <csignal>
#include <netinet/in.h>
#include <string_view> // std::string_view
#include <strings.h>
#include <sys/socket.h>
#include <vector>

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

  err = listen(m_socket, 5);
  if (err != 0)
    return err;

  std::vector<std::byte> buffer(1024);
  while (true) {
    int client = accept(m_socket, nullptr, nullptr);
    int read = recv(client, buffer.data(), buffer.size(), 0);
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
      Response res(http::OK);
      req.path.Match(patternCopy);
      return value(req, res);
    }
  }

  return Response(http::NOT_FOUND);
}
