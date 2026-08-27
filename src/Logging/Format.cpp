#include "Utils/Logging/Format.h"
#include "Utils/Regex/Matcher.h"

namespace Utils::Logging
{

// Utils::Regex is safe to use here: Matcher deliberately does not log, so there is no
// Formatter -> Matcher -> Logger -> Formatter construction cycle.
void Formatter::init()
{
    m_format.clear();
    m_flags.clear();
    m_literals.clear();

    // '%', an optional width like "3" or "-3", then the flag character. The flag set is
    // spelled out rather than \T because \T only covers letters, and the table in
    // Format.h has %@ %# %$ and friends too.
    static const Utils::Regex::Matcher flagRegex(
        R"('%'{'-'?\d+}?{'.'\d+}?{(\c|\C|'@'|'#'|'!'|'^'|'$'|'%')})");

    auto matches = flagRegex.findAllGroupsInfo(m_pattern);
    if (!matches)
    {
        m_literals.push_back(m_pattern); // no flags at all, the pattern is one literal
        m_format = m_pattern;
        return;
    }

    std::size_t pos = 0;
    for (const auto &match : *matches)
    {
        m_literals.push_back(m_pattern.substr(pos, match.start - pos)); // text before the flag
        m_format += m_literals.back();
        m_format += "{}";

        // The flag character is always the last group. Whatever comes before it is a width
        // spec, told apart by its text rather than its index: an optional group that did
        // not participate leaves no entry at all, so "%.8l" and "%8l" both arrive as one.
        Flag flag{};
        flag.op = match.groups.back().match.front();

        for (std::size_t i = 0; i + 1 < match.groups.size(); i++)
        {
            const std::string &spec = match.groups[i].match; // "3", "-3" or ".8"

            uint16_t value = 0;
            for (char c : spec)
            {
                if (c >= '0' && c <= '9')
                    value = static_cast<uint16_t>(value * 10 + (c - '0'));
            }

            if (spec.front() == '.')
            {
                flag.maxWidth = value;
            }
            else
            {
                flag.padding = value;
                flag.lalign = spec.front() != '-';
            }
        }

        m_flags.push_back(flag);
        pos = match.start + match.len;
    }

    m_literals.push_back(m_pattern.substr(pos)); // trailing text
    m_format += m_literals.back();
}

} // namespace Utils::Logging
