#ifndef TREE_H
#define TREE_H

#include "request.hpp"
#include "response.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
namespace http {

class Node {
public:
  bool m_isValue;
  bool m_isDummy;
  std::string m_subPath;
  std::map<std::string, std::shared_ptr<Node>> m_next;
  std::function<void(Request, Response *)> m_function;

public:
  Node(std::string subPath, bool isValue,
       std::function<void(Request, Response *)>);
  Node(std::string subPath);
};

class Tree {
private:
  std::shared_ptr<Node> m_root;
  std::string m_method;
  size_t m_depth;

public:
  Tree(std::string method);
  void AddPath(std::string, std::function<void(Request, Response *)>);
  std::optional<std::function<void(Request, Response *)>> Get(std::string);
  void DebugPrint();
};
} // namespace http

#endif
