#ifndef TREE_H
#define TREE_H

#include "request.hpp"
#include "response.hpp"
#include <cstddef>
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
  Node(std::string subPath, bool isValue,
       std::function<void(Request, Response *)>);
  Node(std::string subPath);
  ~Node();
};

class Tree {
private:
  Node *m_root;
  std::string m_method;
  size_t m_depth;

public:
  Tree(std::string method);
  ~Tree();
  void add_path(std::string, std::function<void(Request, Response *)>);
  std::optional<std::function<void(Request, Response *)>> get(std::string);
  void debug_Print();
};
} // namespace http

#endif
