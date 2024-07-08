#ifndef PATH_H
#define PATH_H

#include <map>
#include <optional>
#include <string>

class Path {
private:
  // std::string m_path;
  std::string m_base;
  std::string m_query;
  std::map<std::string, std::string> m_variables;

public:
  Path(std::string path);
  std::optional<std::string> Get(std::string name);
  std::string Query();
  std::string Base();
  void Match(std::string pattern);
};

#endif // !PATH_H
