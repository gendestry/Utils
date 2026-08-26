//
// Created by bobi on 16. 03. 26.
//

#pragma once
#include "Location.h"
#include "Utils/Colors/Theme.h"
#include <cstdint>
#include <format>
#include <print>
#include <source_location>
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
        WARN = 3U,
        ERROR = 4U,
    };

  private:
    inline static const auto c_debug = Font::themed({92, 99, 112});     // grey
    inline static const auto c_info = Font::themed({97, 175, 239});     // blue
    inline static const auto c_warn = Font::themed({229, 192, 123});    // amber
    inline static const auto c_error = Font::themed({224, 108, 117});   // red
    inline static const auto c_success = Font::themed({152, 195, 121}); // green
    inline static const auto c_scope = Font::themed({198, 120, 221});   // violet
    inline static const auto c_meta = Font::themed({92, 99, 112});      // grey: time, call site

    static uint32_t s_scopeSize;
    static Level s_level;
    static bool s_enableTime;
    static bool s_enableLocation;

    Level m_level = NOTSET;
    std::string m_scope;
    mutable std::string m_scopeCache;
    uint32_t m_padOffset = 0;

    inline std::string paddingStr() const;
    inline std::string scopeStr() const;
    inline std::string timeStr() const;
    std::string locationStr(const std::source_location &loc) const;
    std::string
    prependInfoStr(const std::string &level, const std::source_location *loc = nullptr) const;

    // void log(Tag tag, const std::source_location &loc, const std::string &msg) const;

    template <typename... Args>
    void
    log(Level msgLevel, const std::string &label, const Logging::FormatWithLocation &format,
        Args &&...args) const
    {
        const Level level = m_level == NOTSET ? s_level : m_level;
        if (level <= msgLevel)
        {
            auto msg = std::vformat(format.fmt(), std::make_format_args(args...));
            std::println("{} {}", prependInfoStr(label, &format.location), msg);
        }
    }

  public:
    explicit Logger(std::string scope);
    explicit Logger(Logger &other, std::string scope);

    static void setLevel(Level level);
    void setLoggerLevel(Level level);

    void incPadOffset();
    void decPadOffset();

    void toggleScope();
    static void printTime(bool should);
    static void printLocation(bool should);

    void print(const std::string &text) const;
    void println(const std::string &text) const;

    template <typename... Args>
    void print(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::print("{}{}", prependInfoStr("", &format.location), msg);
    }

    template <typename... Args>
    void println(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::println("{}{}", prependInfoStr("", &format.location), msg);
    }

    template <typename... Args>
    void printColor(
        const std::string &color, const Logging::FormatWithLocation &format, Args &&...args
    ) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::print("{}{}{}{}", prependInfoStr("", &format.location), color, msg, Font::colorReset);
    }

    template <typename... Args>
    void printlnColor(
        const std::string &color, const Logging::FormatWithLocation &format, Args &&...args
    ) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        std::println(
            "{}{}{}{}", prependInfoStr("", &format.location), color, msg, Font::colorReset
        );
    }

    template <typename... Args>
    void debug(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        log(Level::DEBUGGING, Font::format(c_debug("DEBUG")), format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        log(Level::INFO, Font::format(c_info("INFO")), format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        log(Level::WARN, Font::format(c_warn("WARN")), format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void success(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        log(Level::ERROR, Font::format(c_success("SUCC")), format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        log(Level::ERROR, Font::format(c_error("ERROR")), format, std::forward<Args>(args)...);
    }
};
} // namespace Utils
