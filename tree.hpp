#ifndef TREE_H
#define TREE_H

#include "node.hpp"
#include "request.hpp"
#include "response.hpp"
#include <cstddef>
#include <functional>
#include <string>
namespace http {

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
