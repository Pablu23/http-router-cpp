#include "path.hpp"
#include <optional>

Path::Path(std::string path) {
  int pos = path.find("?");
  m_base = path.substr(0, pos);
  path.erase(0, pos + 1);
  m_query = path;
}

std::optional<std::string> Path::Get(std::string name) {
  if (m_variables.contains(name))
    return m_variables.at(name);
  return std::nullopt;
}

std::string Path::Base() { return m_base; }
std::string Path::Query() { return m_query; }

void Path::Match(std::string pattern) {
  int pos = 0;
  std::string path = m_base;
  while (pos != -1) {
    pos = pattern.find('/');
    std::string p = pattern.substr(0, pos);

    int uPos = path.find('/');
    std::string u = path.substr(0, uPos);

    if (p.starts_with('{')) {
      std::string name = p.substr(1, p.size() - 2);
      m_variables.insert_or_assign(name, u);
    }
    pattern.erase(0, pos + 1);
    path.erase(0, uPos + 1);
  }
}
