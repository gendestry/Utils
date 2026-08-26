#include "Utils/Logging/Format.h"

namespace Utils::Logging
{

// Deliberately does not use Utils::Regex: Matcher owns a Logger, and Logger owns a
// Formatter, so reaching for the regex engine here closes a construction cycle.
void Formatter::init()
{
    m_format.clear();
    m_flags.clear();
    m_literals.clear();

    std::size_t pos = 0;
    for (std::size_t i = 0; i + 1 < m_pattern.size();)
    {
        if (m_pattern[i] != '%')
        {
            ++i;
            continue;
        }

        m_literals.push_back(m_pattern.substr(pos, i - pos)); // text before the flag
        m_format += m_literals.back();
        m_format += "{}";
        m_flags.push_back(m_pattern.substr(i, 2)); // "%H"
        i += 2;
        pos = i;
    }

    m_literals.push_back(m_pattern.substr(pos)); // trailing text
    m_format += m_literals.back();
}

} // namespace Utils::Logging
