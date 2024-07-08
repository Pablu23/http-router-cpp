#ifndef REQUEST_HEADER_H
#define REQUEST_HEADER_H
#include <map>
#include <sstream>
#include <string>
#include <vector>

class Request {
private:
  std::map<std::string, std::string> m_headers;
  std::string m_method;
  std::string m_path;
  std::vector<std::byte> m_payload;
  std::string m_protocol;

private:
  bool protocol(std::stringstream *ss, int *procPart, char c);

public:
  Request(std::vector<std::byte> buf);
  void Print();
  bool HasData();
  std::vector<std::byte> Data();
};
#endif // !REQUEST_HEADER_H
