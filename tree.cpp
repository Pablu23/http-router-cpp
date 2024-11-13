#include "tree.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>

using namespace http;

std::vector<std::string> split(std::string s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    if (token != "") {
      res.push_back(token);
    }
  }

  token = s.substr(pos_start);
  if (token != "") {
    res.push_back(token);
  }
  return res;
}

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

void create_dummy_nodes(std::shared_ptr<Node> **root,
                        std::vector<std::string> rest) {
  auto curr = *root;
  for (auto next : rest) {
    auto dummy = std::make_shared<Node>(Node{next});
    (*curr)->m_next.insert_or_assign(next, dummy);
    curr = &dummy;
  }
  root = &curr;
}

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

void Tree::DebugPrint() { printNode(m_root, 0, 10); }
