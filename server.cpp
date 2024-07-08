#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class Request {
private:
  std::map<std::string, std::string> m_headers;
  std::string m_method;
  std::string m_path;
  std::vector<std::byte> m_payload;
  std::string m_protocol;

private:
  bool protocol(std::stringstream *ss, int *procPart, char c) {
    if (c == ' ' || c == '\n') {
      switch (*procPart) {
      case 0:
        m_method = ss->str();
        break;
      case 1:
        m_path = ss->str();
        break;
      case 2:
        m_protocol = ss->str();
        break;
      }
      (*procPart)++;
      ss->str("");
      if (c == '\n')
        return false;
    } else {
      ss->put(c);
    }
    return true;
  }

public:
  Request(std::vector<std::byte> buf) {
    std::string name;
    std::stringstream ss;
    bool header = true;
    int procPart = 0;
    bool data = false;
    bool skipNext = false;
    bool p = true;
    for (auto &byte : buf) {
      if (data) {
        m_payload.push_back(byte);
        continue;
      }

      char c = static_cast<char>(byte);
      if (c == '\r')
        continue;

      if (p) {
        p = protocol(&ss, &procPart, c);
        continue;
      }

      if (skipNext && c == ' ') {
        skipNext = false;
        continue;
      }
      skipNext = false;

      if (c == ':' && header) {
        skipNext = true;
        header = false;
        name = ss.str();
        ss.str("");
      } else if (c == '\n' && !name.empty()) {
        m_headers.insert_or_assign(name, ss.str());
        ss.str("");
        name.clear();
        header = true;
      } else if (c == '\n' && name.empty()) {
        data = true;
      } else {
        ss << c;
      }
    }
  }

  void Print() {
    std::cout << "Protocol: " << m_protocol << "\n"
              << "Req: " << m_method << " " << m_path << std::endl;
    for (const auto &[key, value] : m_headers) {
      std::cout << "[" << key << "]: [" << value << "]\n";
    }

    if (HasData()) {
      std::cout << "Payload: " << std::endl;
      for (auto &byte : m_payload) {
        std::cout << static_cast<char>(byte);
      }
      std::cout << std::endl;
    }
  }
  bool HasData() { return m_payload.size() != 0; }
  std::vector<std::byte> Data() { return m_payload; }
};

class Response {
private:
  std::map<std::string, std::string> m_headers;
  std::vector<std::byte> m_payload;

public:
  Response(std::vector<std::byte> data) {
    m_headers.insert(std::pair<std::string, std::string>(
        "content-length", std::to_string(data.size())));
    m_payload = data;
  }

  Response(std::string data) {
    m_headers.insert(std::pair<std::string, std::string>(
        "content-length", std::to_string(std::strlen(data.data()))));

    m_payload = std::vector<std::byte>(data.size() + 1);
    std::transform(data.begin(), data.end(), m_payload.begin(),
                   [](char c) { return std::byte(c); });
  }

  void SetContentType(const std::string type) {
    m_headers.insert_or_assign("content-type", type);
  }

  void Send(int clientSocket) {
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\n";
    for (const auto &[key, value] : m_headers) {
      ss << key << ": " << value << "\n";
    }
    if (m_payload.size() >= 0) {
      ss << "\n";
      for (auto &byte : m_payload) {
        std::cout << int(byte) << ' ';
        char c = static_cast<char>(byte);
        ss << c;
      }
      std::cout << std::endl;
    }
    std::string p = ss.str();
    send(clientSocket, p.data(), p.size(), 0);
  }

  void Print() {
    for (const auto &[key, value] : m_headers) {
      std::cout << "[" << key << "]: [" << value << "]\n";
    }

    if (m_payload.size() != 0) {
      std::cout << "Payload: " << std::endl;
      for (auto &byte : m_payload) {
        std::cout << static_cast<char>(byte);
      }
      std::cout << std::endl;
    }
  }
};

int main() {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(8080);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  int err = bind(serverSocket, (struct sockaddr *)&serverAddress,
                 sizeof(serverAddress));
  if (err != 0) {
    std::cout << "error binding " << strerror(errno) << std::endl;
    exit(1);
  }

  err = listen(serverSocket, 5);
  if (err != 0) {
    std::cout << "error listening " << strerror(errno) << std::endl;
    exit(1);
  }

  std::vector<std::byte> buffer(1024);
  while (true) {
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    int read = recv(clientSocket, buffer.data(), buffer.size(), 0);
    buffer.resize(read);
    Request req(buffer);
    // req.Print();
    Response res = Response("Hello world!");
    if (req.HasData()) {
      auto data = req.Data();
      res = Response(data);
    }

    res.SetContentType("text/plain");
    // std::cout << "RESPONSE: " << std::endl;
    // res.Print();
    res.Send(clientSocket);
    close(clientSocket);
  }

  if (shutdown(serverSocket, SHUT_RDWR) < 0) {
    std::cout << "error shutdown " << strerror(errno) << std::endl;
    exit(1);
  }

  if (close(serverSocket) < 0) {
    std::cout << "error closing socket " << strerror(errno) << std::endl;
    exit(1);
  }

  return 0;
}
