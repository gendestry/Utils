#pragma once
#include "Utils/Time/Time.h"
#include <format>
#include <iterator>

#include <string>
#include <string_view>
#include <vector>

namespace Utils::Logging
{

struct Record
{
    std::string level;
    std::string scope;
    std::string location;
    std::string msg;
};

struct  Formatter
{
    std::string m_pattern;                    // owns the text the views below point int
    std::string m_format;                    // owns the text the views below point int
    std::vector<std::string> m_flags;                      // flag letters, in order of appearance
    std::vector<std::string> m_literals;                   // text between the flags (m_flags.size() + 1 entries)

    void init(); // defined in src/Logging/Format.cpp to keep Regex out of this header

    static void appendFlag(std::string& out, char flag, const Record& rec, const Time::Clock& t)
    {
        auto put = [&out](auto&& value, std::string_view spec = "{}")
        { std::vformat_to(std::back_inserter(out), spec, std::make_format_args(value)); };

        switch (flag)
        {
        case 'v': put(rec.msg);                      break;
        case 'n': put(rec.scope);                    break;
        case 'l': put(rec.level);                    break;
        case '@': put(rec.location);                 break;
        case 'T': put(t.toString());                 break;
        case 'H': put(int(t.hours),        "{:02}"); break;
        case 'M': put(int(t.minutes),      "{:02}"); break;
        case 'S': put(int(t.seconds),      "{:02}"); break;
        case 'e': put(int(t.milliseconds), "{:03}"); break;
        case '%': out += '%';                        break;
        default:  out += '%'; out += flag;           break;
        }
    }

public:
    Formatter()
    {
        init();
    };
    explicit Formatter(std::string pattern) : m_pattern(std::move(pattern))
    {
        init();
    }

    void setPattern(std::string pattern)
    {
        m_pattern = std::move(pattern);
        init();
    }


    [[nodiscard]] std::string render(const Record& rec) const
    {
        const Time::Clock t;
        std::string out;
        out.reserve(m_pattern.size() + 64);

        for (std::size_t i = 0; i < m_flags.size(); ++i)
        {
            out += m_literals[i];                       // text before this flag
            appendFlag(out, m_flags[i][1], rec, t);     // m_flags[i] is "%H", so [1] is the letter
        }
        out += m_literals.back();                       // trailing text
        return out;
    }
};

/*
 * ┌─────────────┬─────────────────────────────────────────────────┐
│    Flag     │                     Meaning                     │
├─────────────┼─────────────────────────────────────────────────┤
│ %v          │ the message text                                │
├─────────────┼─────────────────────────────────────────────────┤
│ %n          │ logger name                                     │
├─────────────┼─────────────────────────────────────────────────┤
│ %l / %L     │ level word (info) / single char (I)             │
├─────────────┼─────────────────────────────────────────────────┤
│ %T          │ HH:MM:SS (also %X)                              │
├─────────────┼─────────────────────────────────────────────────┤
│ %H %M %S    │ hours / minutes / seconds separately            │
├─────────────┼─────────────────────────────────────────────────┤
│ %e %f %F    │ milli- / micro- / nanosecond part               │
├─────────────┼─────────────────────────────────────────────────┤
│ %Y %m %d    │ year, month, day                                │
├─────────────┼─────────────────────────────────────────────────┤
│ %D          │ MM/DD/YY                                        │
├─────────────┼─────────────────────────────────────────────────┤
│ %z          │ UTC offset, +02:00                              │
├─────────────┼─────────────────────────────────────────────────┤
│ %s %g %# %! │ source basename / full path / line / function   │
├─────────────┼─────────────────────────────────────────────────┤
│ %@          │ basename:line in one flag                       │
├─────────────┼─────────────────────────────────────────────────┤
│ %t %P       │ thread id / process id                          │
├─────────────┼─────────────────────────────────────────────────┤
│ %o %i %u    │ elapsed ms / µs / ns since the previous message │
├─────────────┼─────────────────────────────────────────────────┤
│ %^ %$       │ start / end of the colored range                │
├─────────────┼─────────────────────────────────────────────────┤
│ %%          │ a literal %
 */
} // namespace Utils::Logging