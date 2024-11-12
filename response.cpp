#include "response.hpp"
#include "http.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>

using namespace http;

Response::Response(statuscode::statusCode statusCode) {
  m_statusCode = statusCode;
  SetHeader("Connection", "close");
}

void Response::SetHeader(const std::string name, const std::string value) {
  m_headers.insert_or_assign(name, value);
}

void Response::SetPayload(const std::vector<std::byte> data) {
  m_headers.insert(std::pair<std::string, std::string>(
      "Content-Length", std::to_string(data.size())));
  m_payload = data;
}

void Response::SetStatusCode(statuscode::statusCode statuscode) {
  m_statusCode = statuscode;
}

void Response::SetPayload(const std::string data) {
  m_headers.insert(std::pair<std::string, std::string>(
      "Content-Length", std::to_string(std::strlen(data.data()))));

  m_payload = std::vector<std::byte>(data.size() + 1);
  std::transform(data.begin(), data.end(), m_payload.begin(),
                 [](char c) { return std::byte(c); });
}

void Response::SetContentType(const std::string type) {
  m_headers.insert_or_assign("Content-Type", type);
}

void Response::Send(int clientSocket) {
  std::stringstream ss;
  ss << "HTTP/1.1 " << m_statusCode << " "
     << statuscode::StatusCodeString(m_statusCode) << "\n";
  for (const auto &[key, value] : m_headers) {
    ss << key << ": " << value << "\n";
  }
  if (m_payload.size() > 0) {
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
