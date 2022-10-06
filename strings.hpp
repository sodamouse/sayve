#pragma once

#include <string>
#include <vector>

namespace soda {

std::string quotify(const std::string& str);
std::string read_all(const std::string& filePath);
std::vector<std::string> split_string(const std::string& str, char delim);

} // namespace soda
