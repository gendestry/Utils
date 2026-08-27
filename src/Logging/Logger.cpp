//
// Created by bobi on 16. 03. 26.
//

#include "Utils/Logging/Logger.h"
#include "Utils/Text/String.h"
#include "Utils/Time/Time.h"

namespace Utils
{
uint32_t Logger::s_scopeSize = 0;
Logger::Level Logger::s_level = Level::ERROR;
Logging::Formatter Logger::s_formatter{"%H:%M:%S %7l [%@] [%n] %v"};

Logger::Logger(std::string scope) : m_scope(std::move(scope))
{
    if (scope.size() > s_scopeSize)
    {
        s_scopeSize = scope.size();
    }
}

Logger::Logger(Logger &other, std::string scope)
{
    m_scope = other.m_scope + scope;
    if (scope.size() > s_scopeSize)
    {
        s_scopeSize = scope.size();
    }
}

void Logger::setFormat(const std::string &pattern) { s_formatter.setPattern(pattern); }

void Logger::setLoggerFormat(const std::string &pattern) { m_formatter.emplace(pattern); }

void Logger::setLevel(Level level) { s_level = level; }

void Logger::setLoggerLevel(Level level) { m_level = level; }

void Logger::incPadOffset() { m_padOffset++; }

void Logger::decPadOffset() { m_padOffset--; }

void Logger::print(const std::string &text) const { std::print("{}{}", prependInfoStr(""), text); }

void Logger::println(const std::string &text) const
{
    std::println("{}{}", prependInfoStr(""), text);
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