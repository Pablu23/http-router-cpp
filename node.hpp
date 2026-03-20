#ifndef NODE_H
#define NODE_H

#include "request.hpp"
#include "response.hpp"
#include <functional>
#include <string>
namespace http {

class Node {
public:
  bool m_is_value;
  bool m_is_dummy;
  std::string m_sub_path;
  std::map<std::string, Node *> m_next;
  std::function<void(Request, Response *)> m_function;

public:
  Node(std::string sub, bool isValue,
       std::function<void(Request, Response *)> func) {
    m_sub_path = sub;
    m_is_value = isValue;
    m_function = func;
    m_is_dummy = false;
  };

  Node(std::string sub) {
    m_sub_path = sub;
    m_is_dummy = true;
    m_is_value = false;
    m_function = nullptr;
  };
  ~Node() {
    for (auto n : m_next) {
      delete n.second;
    }
  };
};

} // namespace http

#endif
