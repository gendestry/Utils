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
bool Logger::s_enableTime = false;

Logger::Logger(std::string scope) : m_scope(std::move(scope))
{
    if (scope.size() > s_scopeSize)
    {
        s_scopeSize = scope.size();
    }
    toggleScope();
}

Logger::Logger(Logger &other, std::string scope)
{
    m_scope = other.m_scope + scope;
    if (scope.size() > s_scopeSize)
    {
        s_scopeSize = scope.size();
    }
    toggleScope();
}

void Logger::setLevel(Level level) { s_level = level; }

void Logger::setLoggerLevel(Level level) { m_level = level; }

void Logger::incPadOffset() { m_padOffset++; }

void Logger::decPadOffset() { m_padOffset--; }

void Logger::toggleScope()
{
    if (m_scopeCache.empty())
    {
        m_scopeCache = scopeStr();
    }
    else
    {
        m_scopeCache.clear();
    }
}

void Logger::printTime(bool should) { s_enableTime = should; }

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
std::string Logger::timeStr() const
{
    std::string time = s_enableTime ? Utils::String::format("[{}] ", Time().toString()) : "";
    return time;
}
std::string Logger::prependInfoStr(const std::string &level) const
{
    std::string s = level;

    if (!level.empty())
    {
        // std::string t = Utils::String::padUntilLen(level, 5, " ");
        s = std::format("[{}] ", level);
        // s = std::format("[{}] ", Utils::String::padUntilLen(level, 3, " "));
    }

    return String::format("{}{}{}{}", timeStr(), s, scopeStr(), paddingStr());
}

} // namespace Utils