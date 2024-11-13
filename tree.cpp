#include "tree.hpp"
#include "util.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>

using namespace http;

Node::Node(std::string sub) {
  m_sub_path = sub;
  m_is_dummy = true;
  m_is_value = false;
  m_function = nullptr;
}

Node::Node(std::string sub, bool isValue,
           std::function<void(Request, Response *)> func) {
  m_sub_path = sub;
  m_is_value = isValue;
  m_function = func;
  m_is_dummy = false;
}

Tree::Tree(std::string method) { m_method = method; }

void add_node(std::shared_ptr<Node> const &parent, std::string path,
             std::vector<std::string> rest,
             std::function<void(Request, Response *)> func) {
  std::shared_ptr<Node> curr = parent->m_next[path];
  if (rest.size() == 0) {
    if (curr) {
      curr->m_is_dummy = false;
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
    add_node(curr, newPath, rest, func);
  } else {
    auto newPath = rest.front();
    rest.erase(rest.begin());
    std::shared_ptr<Node> leaf = std::make_shared<Node>(Node{path});
    parent->m_next.insert_or_assign(path, leaf);
    add_node(leaf, newPath, rest, func);
  }
}

void Tree::add_path(std::string path,
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
  add_node(m_root, newPath, subPaths, func);
}

void print_node(std::shared_ptr<Node> node, size_t depth, size_t max_depth) {
  if (depth >= max_depth) {
    return;
  }

  std::cout << std::string(depth, ' ') << "sub: \"" << node->m_sub_path
            << "\" IsDummy: " << node->m_is_dummy << std::endl;
  for (auto &next : node->m_next) {
    print_node(next.second, depth + 1, max_depth);
  }
}

std::optional<std::function<void(Request, Response *)>>
traverse(std::shared_ptr<Node> const &parent, std::string path,
         std::vector<std::string> rest) {

  std::shared_ptr<Node> curr = parent->m_next[path];
  if (rest.size() == 0) {
    if (curr != nullptr && !curr->m_is_dummy)
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
Tree::get(std::string path) {
  auto subs = split(path, "/");
  if (subs.size() == 0) {
    if (!m_root->m_is_dummy)
      return m_root->m_function;
    else
      return std::nullopt;
  }

  auto newPath = subs.front();
  subs.erase(subs.begin());
  return traverse(m_root, newPath, subs);
}

void Tree::debug_Print() { print_node(m_root, 0, 10); }
