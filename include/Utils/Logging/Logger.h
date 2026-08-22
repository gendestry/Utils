//
// Created by bobi on 16. 03. 26.
//

#pragma once
#include "Utils/Colors/Theme.h"
#include <cstdint>
#include <format>
#include <print>
#include <string>

namespace Utils
{
class Logger
{
  public:
    enum Level : uint8_t
    {
        NOTSET = 0U,
        DEBUGGING = 1U,
        INFO = 2U,
        ERROR = 3U,
    };

  private:
    static uint32_t s_scopeSize;
    static Level s_level;
    static bool s_enableTime;

    Level m_level = NOTSET;
    std::string m_scope;
    mutable std::string m_scopeCache;
    uint32_t m_padOffset = 0;

    inline std::string paddingStr() const;
    inline std::string scopeStr() const;
    inline std::string timeStr() const;
    std::string prependInfoStr() const;

  public:
    explicit Logger(std::string scope);
    explicit Logger(Logger &other, std::string scope);

    static void setLevel(Level level);
    void setLoggerLevel(Level level);

    void incPadOffset();
    void decPadOffset();

    void toggleScope();
    static void printTime(bool should);

    void print(const std::string &text) const;
    void println(const std::string &text) const;

    template <typename... Args> void print(const std::string &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::print("{}{}", prependInfoStr(), msg);
    }

    template <typename... Args> void println(const std::string &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::println("{}{}", prependInfoStr(), msg);
    }

    template <typename... Args>
    void printColor(const std::string &color, const std::string &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::print("{}{}{}{}", prependInfoStr(), color, msg, Font::colorReset);
    }

    template <typename... Args>
    void printlnColor(const std::string &color, const std::string &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::println("{}{}{}{}", prependInfoStr(), color, msg, Font::colorReset);
    }

    template <typename... Args> void debug(const std::string &format, Args &&...args) const
    {
        const Level level = m_level == NOTSET ? s_level : m_level;
        if (level <= Level::DEBUGGING)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            std::println("{}", Font::format(Theme::dim(Font::IT("{}{}")), prependInfoStr(), msg));
        }
    }

    template <typename... Args> void warn(const std::string &format, Args &&...args) const
    {
        const Level level = m_level == NOTSET ? s_level : m_level;
        if (level <= Level::INFO)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            std::println("{}", Font::format(Theme::warn("{}{}"), prependInfoStr(), msg));
        }
    }

    template <typename... Args> void error(const std::string &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::println("{}", Font::format(Theme::err("{}{}"), prependInfoStr(), msg));
    }
};
} // namespace Utils
