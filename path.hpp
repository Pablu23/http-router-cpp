#ifndef PATH_H
#define PATH_H

#include <map>
#include <optional>
#include <string>

namespace http {
class Path {
private:
  // std::string m_path;
  std::string m_base;
  std::string m_query;
  std::map<std::string, std::string> m_variables;

public:
  Path(std::string path);
  std::optional<std::string> get(std::string name);
  std::string query();
  std::string base();
  void match(std::string pattern);
};
} // namespace http

#endif // !PATH_H
