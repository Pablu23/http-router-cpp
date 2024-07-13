#ifndef RESPONSE_H
#define RESPONSE_H

#include "http.hpp"
#include <map>
#include <string>
#include <vector>

namespace http {
class Response {
private:
  std::map<std::string, std::string> m_headers;
  std::vector<std::byte> m_payload;
  statuscode::statusCode m_statusCode;

public:
  Response(statuscode::statusCode statusCode);
  // Response(std::vector<std::byte> data);
  // Response(std::string data);
  void SetPayload(std::vector<std::byte> data);
  void SetPayload(std::string data);
  void SetContentType(const std::string type);
  void SetStatusCode(const statuscode::statusCode statuscode);
  void Send(int clientSocket);
  void Print();
};
} // namespace http

#endif // !RESPONSE_H
