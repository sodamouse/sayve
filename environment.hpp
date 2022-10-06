#pragma once

#include <string>
#include <unordered_map>
#include <utility>

namespace soda {

std::unordered_map<int, std::string> map_args(int argc, char* argv[]);

}
