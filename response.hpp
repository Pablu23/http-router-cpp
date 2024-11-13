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
  StatusCode::statusCode m_statusCode;

public:
  Response(StatusCode::statusCode status_code);
  void set_header(const std::string name, const std::string value);
  void set_payload(const std::vector<std::byte> data);
  void set_payload(const std::string data);
  void set_content_type(const std::string type);
  void set_status_code(const StatusCode::statusCode status_code);
  void send(int client_socket);
  void print();
};
} // namespace http

#endif // !RESPONSE_H
