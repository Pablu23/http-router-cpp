#include "tree.hpp"
#include "util.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>

using namespace http;

Node::Node(std::string sub) {
  m_subPath = sub;
  m_isDummy = true;
  m_isValue = false;
  m_function = nullptr;
}

Node::Node(std::string sub, bool isValue,
           std::function<void(Request, Response *)> func) {
  m_subPath = sub;
  m_isValue = isValue;
  m_function = func;
  m_isDummy = false;
}

Tree::Tree(std::string method) { m_method = method; }

void addNode(std::shared_ptr<Node> const &parent, std::string path,
             std::vector<std::string> rest,
             std::function<void(Request, Response *)> func) {
  std::shared_ptr<Node> curr = parent->m_next[path];
  if (rest.size() == 0) {
    if (curr) {
      curr->m_isDummy = false;
      curr->m_function = func;
    } else {
      std::shared_ptr<Node> leaf =
          std::make_shared<Node>(Node{path, false, func});
      parent->m_next.insert_or_assign(path, leaf);
    }
    return;
  }

  if (curr) {
    auto newPath = rest.front();
    // Ineffiecient, use deque
    rest.erase(rest.begin());
    addNode(curr, newPath, rest, func);
  } else {
    auto newPath = rest.front();
    rest.erase(rest.begin());
    std::shared_ptr<Node> leaf = std::make_shared<Node>(Node{path});
    parent->m_next.insert_or_assign(path, leaf);
    addNode(leaf, newPath, rest, func);
  }
}

void Tree::AddPath(std::string path,
                   std::function<void(Request, Response *)> func) {
  auto subPaths = split(path, "/");

  if (subPaths.size() == 0 && m_root == nullptr) {
    m_root = std::make_shared<Node>(Node{"", false, func});
    return;
  } else if (subPaths.size() == 0) {
    m_root->m_function = func;
    return;
  } else if (m_root == nullptr) {
    m_root = std::make_shared<Node>(Node{""});
  }

  auto newPath = subPaths.front();
  subPaths.erase(subPaths.begin());
  addNode(m_root, newPath, subPaths, func);
}

void printNode(std::shared_ptr<Node> node, size_t depth, size_t max_depth) {
  if (depth >= max_depth) {
    return;
  }

  std::cout << std::string(depth, ' ') << "sub: \"" << node->m_subPath
            << "\" IsDummy: " << node->m_isDummy << std::endl;
  for (auto &next : node->m_next) {
    printNode(next.second, depth + 1, max_depth);
  }
}

std::optional<std::function<void(Request, Response *)>>
traverse(std::shared_ptr<Node> const &parent, std::string path,
         std::vector<std::string> rest) {

  std::shared_ptr<Node> curr = parent->m_next[path];
  if (rest.size() == 0) {
    if (curr != nullptr && !curr->m_isDummy)
      return curr->m_function;
    else
      return std::nullopt;
  }

  if (curr) {
    auto newPath = rest.front();
    // Ineffiecient, use deque
    rest.erase(rest.begin());
    return traverse(curr, newPath, rest);
  }

  return std::nullopt;
}

std::optional<std::function<void(Request, Response *)>>
Tree::Get(std::string path) {
  auto subs = split(path, "/");
  if (subs.size() == 0) {
    if (!m_root->m_isDummy)
      return m_root->m_function;
    else
      return std::nullopt;
  }

  auto newPath = subs.front();
  subs.erase(subs.begin());
  return traverse(m_root, newPath, subs);
}

void Tree::DebugPrint() { printNode(m_root, 0, 10); }
