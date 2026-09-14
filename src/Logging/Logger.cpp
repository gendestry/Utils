//
// Created by bobi on 16. 03. 26.
//

#include "Utils/Logging/Logger.h"
#include "Utils/Text/String.h"
#include "Utils/Time/Time.h"

namespace Utils
{
std::atomic<uint32_t> Logger::s_scopeSize = 0;
std::atomic<Logger::Level> Logger::s_level = Level::ERROR;
std::atomic<std::shared_ptr<const Logging::Formatter>> Logger::s_formatter{
    std::make_shared<const Logging::Formatter>("%H:%M:%S %7l [%@] [%n] %v")
};
std::mutex Logger::s_writeMutex;

// Widening a maximum is a read-modify-write, so plain assignment would let two threads
// racing to grow it settle on the smaller of the two.
static void growTo(std::atomic<uint32_t> &value, std::size_t candidate)
{
    uint32_t seen = value.load(std::memory_order_relaxed);
    while (candidate > seen &&
           !value.compare_exchange_weak(
               seen, static_cast<uint32_t>(candidate), std::memory_order_relaxed
           ))
    {
    }
}

Logger::Logger(std::string scope) : m_scope(std::move(scope))
{
    growTo(s_scopeSize, m_scope.size());
}

Logger::Logger(Logger &other, std::string scope)
{
    m_scope = other.m_scope + scope;
    growTo(s_scopeSize, m_scope.size());
}

// A new Formatter every time rather than setPattern() on the live one: readers hold the
// old object by shared_ptr and finish rendering from it undisturbed.
void Logger::setFormat(const std::string &pattern)
{
    s_formatter.store(std::make_shared<const Logging::Formatter>(pattern), std::memory_order_release);
}

void Logger::setLoggerFormat(const std::string &pattern)
{
    m_formatter = std::make_shared<const Logging::Formatter>(pattern);
}

void Logger::setLevel(Level level) { s_level.store(level, std::memory_order_relaxed); }

void Logger::setLoggerLevel(Level level) { m_level = level; }

void Logger::incPadOffset() { m_padOffset++; }

void Logger::decPadOffset() { m_padOffset--; }

void Logger::print(const std::string &text) const
{
    writeText(std::format("{}{}", prependInfoStr(""), text));
}

void Logger::println(const std::string &text) const
{
    writeLine(std::format("{}{}", prependInfoStr(""), text));
}

std::string Logger::paddingStr() const { return String::pad(m_padOffset, "  "); }
std::string Logger::scopeStr() const
{
    return String::format("[{}]", Font::format(Theme::lime(m_scope)));
};
std::string Logger::locationStr(const std::source_location &loc) const
{
    // file_name() is the compiler's full path; keep only the basename.
    std::string_view file = loc.file_name();
    if (const auto pos = file.find_last_of("/\\"); pos != std::string_view::npos)
    {
        file.remove_prefix(pos + 1);
    }

    // bare value: the pattern (%@) supplies the surrounding brackets
    return Font::format(c_meta(std::format("{}:{}", file, loc.line())));
}

std::string Logger::prependInfoStr(const std::string &level, const std::source_location *loc) const
{
    std::string s = level;
    if (!level.empty())
    {
        s = std::format("[{}] ", level);
    }

    const std::string location = loc != nullptr ? String::format("[{}] ", locationStr(*loc)) : "";

    return String::format("{}{}{}{}", s, location, scopeStr(), paddingStr());
}

} // namespace Utils