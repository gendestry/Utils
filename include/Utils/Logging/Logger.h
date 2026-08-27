//
// Created by bobi on 16. 03. 26.
//

#pragma once
#include "Location.h"
#include "Format.h"
#include "Utils/Colors/Theme.h"

#include <atomic>
#include <cstdint>
#include <format>
#include <memory>
#include <mutex>
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

    static std::atomic<uint32_t> s_scopeSize;
    static std::atomic<Level> s_level;

    // Never mutated in place: setFormat() swaps in a whole new Formatter, so a thread
    // already inside render() keeps the old one alive through its refcount instead of
    // watching its vectors get cleared underneath it.
    static std::atomic<std::shared_ptr<const Logging::Formatter>> s_formatter;

    // std::print formats and writes in several steps, so two threads sharing stdout can
    // interleave halves of a line. Held for the write only, never while formatting.
    static std::mutex s_writeMutex;

    static void writeText(std::string_view text)
    {
        const std::lock_guard lock(s_writeMutex);
        std::print("{}", text);
    }

    static void writeLine(std::string_view text)
    {
        const std::lock_guard lock(s_writeMutex);
        std::println("{}", text);
    }

    Level m_level = NOTSET;
    std::string m_scope;
    uint32_t m_padOffset = 0;

    std::string paddingStr() const;
    std::string scopeStr() const;
    std::string locationStr(const std::source_location &loc) const;
    std::string prependInfoStr(const std::string &level, const std::source_location *loc = nullptr) const;


    // Non-null only when this logger overrides the shared pattern; see s_formatter. Like
    // m_level and m_padOffset this is per-logger configuration, so a Logger belongs to one
    // thread: share work between threads through the static pattern and level instead.
    std::shared_ptr<const Logging::Formatter> m_formatter;

    std::shared_ptr<const Logging::Formatter> formatter() const
    {
        return m_formatter ? m_formatter : s_formatter.load(std::memory_order_acquire);
    }

    template <typename... Args>
    void log(Level msgLevel, const std::string &label,
             const Logging::FormatWithLocation &format, Args &&...args) const
    {
        const Level level = m_level == NOTSET ? s_level.load(std::memory_order_relaxed) : m_level;
        if (level > msgLevel)
            return;

        const Logging::Record rec{
            .level    = label,                                       // already colored by the caller
            .scope    = Font::format(Theme::lime(m_scope)),
            .location = locationStr(format.location),
            .msg      = std::vformat(format.fmt(), std::make_format_args(args...)),
        };

        writeLine(std::format("{}{}", paddingStr(), formatter()->render(rec)));
    }

  public:
    explicit Logger(std::string scope);
    explicit Logger(Logger &other, std::string scope);

    static void setFormat(const std::string &pattern);
    void setLoggerFormat(const std::string &pattern);

    static void setLevel(Level level);
    void setLoggerLevel(Level level);

    void incPadOffset();
    void decPadOffset();

    void print(const std::string &text) const;
    void println(const std::string &text) const;

    template <typename... Args>
    void print(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        writeText(std::format("{}{}", prependInfoStr("", &format.location), msg));
    }

    template <typename... Args>
    void println(const Logging::FormatWithLocation &format, Args &&...args) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        writeLine(std::format("{}{}", prependInfoStr("", &format.location), msg));
    }

    template <typename... Args>
    void printColor(
        const std::string &color, const Logging::FormatWithLocation &format, Args &&...args
    ) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        writeText(std::format(
            "{}{}{}{}", prependInfoStr("", &format.location), color, msg, Font::colorReset
        ));
    }

    template <typename... Args>
    void printlnColor(
        const std::string &color, const Logging::FormatWithLocation &format, Args &&...args
    ) const
    {
        auto msg = std::vformat(format, std::make_format_args(args...));
        writeLine(std::format(
            "{}{}{}{}", prependInfoStr("", &format.location), color, msg, Font::colorReset
        ));
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
