#include "request.hpp"
#include <iostream>

using namespace http;

bool Request::protocol(std::stringstream *ss, int *procPart, char c) {
  if (c == ' ' || c == '\n') {
    switch (*procPart) {
    case 0:
      m_method = ss->str();
      break;
    case 1:
      m_path_raw = ss->str();
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

// This is shit, meaning the path constructor
Request::Request(std::vector<std::byte> buf) : path("") {
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
  path = Path{m_path_raw};
}

void Request::print() {
  std::cout << "Protocol: " << m_protocol << "\n"
            << "Req: " << m_method << " " << m_path_raw << std::endl;
  for (const auto &[key, value] : m_headers) {
    std::cout << "[" << key << "]: [" << value << "]\n";
  }

  if (has_data()) {
    std::cout << "Payload: " << std::endl;
    for (auto &byte : m_payload) {
      std::cout << static_cast<char>(byte);
    }
    std::cout << std::endl;
  }
}

std::string Request::method() { return m_method; }
bool Request::has_data() { return m_payload.size() != 0; }
std::vector<std::byte> Request::data() { return m_payload; }
