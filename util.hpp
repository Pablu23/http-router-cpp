#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

inline std::vector<std::string> split(std::string s, std::string delimiter) {
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

#endif
