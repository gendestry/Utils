#pragma once
#include <concepts>
#include <cstdint>
#include <source_location>
#include <string_view>

namespace Utils::Logging
{
struct FormatWithLocation
{
    std::string_view format;
    std::source_location location;

    template <typename T>
        requires std::convertible_to<const T &, std::string_view>
    constexpr FormatWithLocation(
        const T &fmt,
        const std::source_location &loc = std::source_location::current()
    )
        : format(fmt), location(loc)
    {
    }

    [[nodiscard]] constexpr std::string_view fmt() const noexcept { return format; }

    [[nodiscard]] constexpr const char *fileName() const noexcept { return location.file_name(); }

    [[nodiscard]] constexpr const char *functionName() const noexcept
    {
        return location.function_name();
    }

    [[nodiscard]] constexpr std::uint_least32_t line() const noexcept { return location.line(); }

    [[nodiscard]] constexpr std::uint_least32_t column() const noexcept
    {
        return location.column();
    }
};
} // namespace Utils::Logging

/*
#include <iostream>
#include <source_location>
#include <string_view>

void log(const std::string_view message,
         const std::source_location location =
               std::source_location::current())
{
    std::clog << "file: "
              << location.file_name() << '('
              << location.line() << ':'
              << location.column() << ") `"
              << location.function_name() << "`: "
              << message << '\n';
}

template<typename T>
void fun(T x)
{
    log(x); // line 20
}

int main(int, char*[])
{
    log("Hello world!"); // line 25
    fun("Hello C++20!");
}*/