#ifndef RESPONSE_H

#include <map>
#include <string>
#include <vector>
class Response {
private:
  std::map<std::string, std::string> m_headers;
  std::vector<std::byte> m_payload;

public:
  Response(std::vector<std::byte> data);
  Response(std::string data);
  void SetContentType(const std::string type);
  void Send(int clientSocket);
  void Print();
};

#endif // !RESPONSE_H
