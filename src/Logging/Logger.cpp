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
bool Logger::s_enableLocation = true;

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
void Logger::printLocation(bool should) { s_enableLocation = should; }

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
std::string Logger::locationStr(const std::source_location &loc) const
{
    if (!s_enableLocation)
        return "";

    // file_name() is the compiler's full path; keep only the basename.
    std::string_view file = loc.file_name();
    if (const auto pos = file.find_last_of("/\\"); pos != std::string_view::npos)
    {
        file.remove_prefix(pos + 1);
    }

    return String::format("[{}] ", Font::format(c_meta(std::format("{}:{}", file, loc.line()))));
}

std::string Logger::prependInfoStr(const std::string &level, const std::source_location *loc) const
{
    std::string s = level;
    if (!level.empty())
    {
        s = std::format("[{}] ", level);
    }

    const std::string location = loc != nullptr ? locationStr(*loc) : "";

    return String::format("{}{}{}{}{}", timeStr(), s, location, scopeStr(), paddingStr());
}

} // namespace Utils