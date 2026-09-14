//
// Created by bobi on 16. 03. 26.
//

#include "Utils/Text/String.h"
#include "Utils/Colors/Font.h"
#include <algorithm>
#include <charconv>
#include <sstream>

std::string Utils::String::strip(const std::string &input, const std::string &what)
{
    std::string result = input;
    std::erase_if(result, [&](char c) { return what.find(c) != std::string::npos; });
    return result;
}

bool Utils::String::isInt(const std::string &s)
{
    int value;

    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

    return ec == std::errc() && ptr == s.data() + s.size();
}

// std::vector<std::string> Utils::String::split(const std::string& str, const std::string&
// delimiter) {
//     std::vector<std::string> result;
//     size_t start = 0;
//     size_t end;
//
//     while ((end = str.find(delimiter, start)) != std::string::npos) {
//         result.push_back(str.substr(start, end - start));
//         start = end + delimiter.length();
//     }
//
//     result.push_back(str.substr(start));
//     return result;
// }

std::vector<std::string>
Utils::String::split(const std::string &input, const std::string &delimiters)
{
    std::vector<std::string> result;
    std::string current;
    bool in_quotes = false;

    auto is_delim = [&](char c) { return delimiters.find(c) != std::string::npos; };

    for (char c : input)
    {
        if (c == '"')
        {
            in_quotes = !in_quotes;
            continue; // strip quotes
        }

        if (is_delim(c) && !in_quotes)
        {
            if (!current.empty())
            {
                result.push_back(current);
                current.clear();
            }
        }
        else
        {
            current += c;
        }
    }

    if (!current.empty())
    {
        result.push_back(current);
    }

    return result;
}

std::string Utils::String::normalize_spaces(const std::string &input)
{
    std::string result;
    result.reserve(input.size());

    bool in_quotes = false;
    bool prev_was_space = false;

    for (size_t i = 0; i < input.size(); ++i)
    {
        char c = input[i];

        if (c == '"')
        {
            in_quotes = !in_quotes;
            result += c;
            prev_was_space = false;
            continue;
        }

        if (in_quotes)
        {
            // inside quotes: copy everything exactly
            result += c;
            continue;
        }

        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            // outside quotes: collapse whitespace
            if (!prev_was_space)
            {
                result += ' ';
                prev_was_space = true;
            }
        }
        else
        {
            result += c;
            prev_was_space = false;
        }
    }

    // optional: trim trailing space
    if (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }

    return result;
}

std::string Utils::String::pad(uint32_t n, std::string by)
{
    std::stringstream s;
    for (uint32_t i = 0; i < n; i++)
    {
        s << by;
    }

    return s.str();
}

std::string Utils::String::padUntilLen(const std::string &str, uint32_t n, std::string by)
{
    std::stringstream s;
    s << str;
    for (uint32_t i = 0; i < n - str.length(); i++)
    {
        s << by;
    }

    return s.str();
}

std::string Utils::String::colorWrap(const std::string &color, const std::string &text)
{
    return format("{}{}{}", color, text, Utils::Font::colorReset);
}