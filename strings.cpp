#include "strings.hpp"

#include <fstream>
#include <sstream>

namespace soda {

std::string quotify(const std::string& str)
{
    std::string result = "\"";
    result += str;
    result += "\"";

    return result;
}

std::string read_all(const std::string& filePath)
{
    std::fstream file(filePath, std::ios::in);
    std::stringstream stream;
    stream << file.rdbuf();

    return stream.str();
}

std::vector<std::string> split_string(const std::string& str, char delim)
{
    int p = 0;
    int c = 0;

    std::vector<std::string> result;

    c = str.find(delim);
    while (c != std::string::npos)
    {
        c = str.find(delim, p);
        auto sub = str.substr(p, c - p);
        result.push_back(sub);

        p = c + 1;
    }

    return result;
}

} // namespace soda
