#pragma once
#include <format>
#include <sstream>
#include <vector>

namespace Utils
{
class String
{
    static void concat(std::stringstream &ss) {}

    template <typename T, typename... Args>
    static void concat(std::stringstream &ss, T &&value, Args &&...args)
    {
        ss << std::forward<T>(value);
        concat(ss, std::forward<Args>(args)...);
    }

  public:
    template <typename... Args> static std::string concat(Args &&...args)
    {
        std::stringstream ss;
        concat(ss, std::forward<Args>(args)...);
        return ss.str();
    }

    template <typename... Args> static std::string format(std::string_view fmt, Args &&...args)
    {
        return std::vformat(fmt, std::make_format_args(args...));
    }

    static std::string strip(const std::string &input, const std::string &what);

    static bool isInt(const std::string &s);
    static std::vector<std::string> split(const std::string &str, const std::string &delimiter);
    static std::string normalize_spaces(const std::string &input);
    static std::string pad(uint32_t n, std::string by);
    static std::string colorWrap(const std::string &color, const std::string &text);
};

} // namespace Utils
