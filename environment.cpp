#include "environment.hpp"

namespace soda {

std::unordered_map<int, std::string> map_args(int argc, char* argv[])
{
    std::unordered_map<int, std::string> result;
    for (int i = 1; i < argc; ++i)
    {
        result.insert(std::make_pair(i, argv[i]));
    }

    return result;
}

}
