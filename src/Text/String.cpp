//
// Created by bobi on 16. 03. 26.
//

#include "Utils/Text/String.h"
#include <sstream>
#include <charconv>

bool Utils::String::isInt(const std::string& s) {
    int value;

    auto [ptr, ec] =
        std::from_chars(s.data(), s.data() + s.size(), value);

    return ec == std::errc() &&
           ptr == s.data() + s.size();
}

std::vector<std::string> Utils::String::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }

    result.push_back(str.substr(start));
    return result;
}

std::string Utils::String::pad(uint32_t n, std::string by)
{
    std::stringstream s;
    for(uint32_t i = 0; i < n; i++)
    {
        s << by;
    }

    return s.str();
}