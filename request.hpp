#ifndef REQUEST_HEADER_H
#define REQUEST_HEADER_H

#include "path.hpp"
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace http {
class Request {
private:
  std::map<std::string, std::string> m_headers;
  std::string m_method;
  std::string m_path_raw;
  std::vector<std::byte> m_payload;
  std::string m_protocol;

private:
  bool protocol(std::stringstream *ss, int *proc_part, char c);

public:
  Path path;
  explicit Request(std::vector<std::byte> buf);
  void print();
  bool has_data();
  std::string method();
  std::vector<std::byte> data();
};
} // namespace http

#endif // !REQUEST_HEADER_H
