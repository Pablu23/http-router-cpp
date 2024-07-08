#include "response.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <vector>

Response::Response(http::statusCode statusCode) { m_statusCode = statusCode; }

void Response::SetPayload(std::vector<std::byte> data) {
  m_headers.insert(std::pair<std::string, std::string>(
      "content-length", std::to_string(data.size())));
  m_payload = data;
}

void Response::SetPayload(std::string data) {
  m_headers.insert(std::pair<std::string, std::string>(
      "content-length", std::to_string(std::strlen(data.data()))));

  m_payload = std::vector<std::byte>(data.size() + 1);
  std::transform(data.begin(), data.end(), m_payload.begin(),
                 [](char c) { return std::byte(c); });
}

void Response::SetContentType(const std::string type) {
  m_headers.insert_or_assign("content-type", type);
}

void Response::Send(int clientSocket) {
  std::stringstream ss;
  ss << "HTTP/1.1 " << m_statusCode << " "
     << http::StatusCodeString(m_statusCode) << "\n";
  for (const auto &[key, value] : m_headers) {
    ss << key << ": " << value << "\n";
  }
  if (m_payload.size() >= 0) {
    ss << "\n";
    for (auto &byte : m_payload) {
      char c = static_cast<char>(byte);
      ss << c;
    }
  }
  std::string p = ss.str();
  send(clientSocket, p.data(), p.size(), 0);
}

void Response::Print() {
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
