#include "Utils/Logging/Format.h"
#include "Utils/Regex/Matcher.h"

namespace Utils::Logging
{

// Utils::Regex is safe to use here: Matcher deliberately does not log, so there is no
// Formatter -> Matcher -> Logger -> Formatter construction cycle.
void Formatter::init()
{
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
        return;
    }

    std::size_t pos = 0;
    for (const auto &match : *matches)
    {
        m_literals.push_back(m_pattern.substr(pos, match.start - pos)); // text before the flag

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
    }

    std::string Formatter::render(const Record& rec) const
    {
        const Time::Clock t;
        std::string out;
        out.reserve(m_pattern.size() + 64);

        for (std::size_t i = 0; i < m_flags.size(); ++i)
        {
            out += m_literals[i]; // text before this flag

            const Flag& flag = m_flags[i];
            const std::size_t start = out.size();
            appendFlag(out, flag.op, rec, t);

            if (flag.padding == 0 && flag.maxWidth == 0)
                continue;

            // The field is written; adjust it in place. Too long and too short are
            // exclusive, so cut first and only then consider padding.
            std::size_t shown = visibleWidth(std::string_view(out).substr(start));

            if (flag.maxWidth != 0 && shown > flag.maxWidth)
            {
                const std::string_view field = std::string_view(out).substr(start);
                const std::size_t cut = start + byteOffsetOfColumn(field, flag.maxWidth);
                const bool colored = field.substr(0, cut - start).find('\x1b') != std::string_view::npos;

                out.resize(cut);
                if (colored)
                    out += "\x1b[0m"; // the cut may have taken the color's own reset with it

                shown = flag.maxWidth;
            }

            if (shown >= flag.padding)
                continue;

            const std::size_t fill = flag.padding - shown;
            if (flag.lalign)
                out.append(fill, ' ');
            else
                out.insert(start, fill, ' ');
        }
        out += m_literals.back();                       // trailing text
        return out;
    }
} // namespace Utils::Logging
