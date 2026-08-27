#include "Utils/Regex/Matcher.h"
#include "Utils/Colors/Font.h"
#include <iostream>

using namespace Utils;
using namespace Utils::Regex::Engine;

namespace Utils::Regex
{
namespace
{
// AstNodeOps::match_info() hands MatchInfo::start back as the cursor *after* the match,
// which is what the matching loops need but not what a caller wants to read. Rewrite the
// whole tree into real (start, len) spans and shift it into the original text's coordinates.
void normalizeSpans(MatchInfo &info, unsigned int offset)
{
    info.len = static_cast<unsigned int>(info.match.size());
    info.start = offset + (info.start >= info.len ? info.start - info.len : 0);
    for (auto &group : info.groups)
    {
        normalizeSpans(group, offset);
    }
}

// Move an already normalized tree by `offset` characters.
void shiftSpans(MatchInfo &info, unsigned int offset)
{
    info.start += offset;
    for (auto &group : info.groups)
    {
        shiftSpans(group, offset);
    }
}
} // namespace

Matcher::Matcher(const std::string &pattern) : m_Pattern(pattern)
{
    m_Tokenizer = std::make_unique<Engine::Tokenizer>(pattern);
    m_Tokenizer->tokenize();

    m_Syntax = std::make_unique<Engine::Syntax>(m_Tokenizer->get_tokens());
    m_Valid = m_Syntax->parse();

    if (!m_Valid)
    {
        throw std::runtime_error("Error: invalid regex syntax");
    }
}

Matcher::Matcher(const Matcher &other)
    : m_Pattern(other.m_Pattern), m_Valid(other.m_Valid),
      m_Tokenizer(std::make_unique<Engine::Tokenizer>(*other.m_Tokenizer)),
      m_Syntax(std::make_unique<Engine::Syntax>(*other.m_Syntax))
{
}

Matcher::Matcher(Matcher &&other) noexcept
    : m_Pattern(std::move(other.m_Pattern)), m_Tokenizer(std::move(other.m_Tokenizer)),
      m_Syntax(std::move(other.m_Syntax)), m_Valid(other.m_Valid)
{
}

// operators
Matcher &Matcher::operator=(const Matcher &other)
{
    if (this != &other)
    {
        m_Pattern = other.m_Pattern;
        m_Valid = other.m_Valid;
        m_Tokenizer = std::make_unique<Engine::Tokenizer>(*other.m_Tokenizer);
        m_Syntax = std::make_unique<Engine::Syntax>(*other.m_Syntax);
    }

    return *this;
}

Matcher &Matcher::operator=(Matcher &&other) noexcept
{
    if (this != &other)
    {
        m_Pattern = std::move(other.m_Pattern);
        m_Tokenizer = std::move(other.m_Tokenizer);
        m_Syntax = std::move(other.m_Syntax);
        m_Valid = other.m_Valid;
    }

    return *this;
}

const std::string &Matcher::getPattern() const { return m_Pattern; }

bool Matcher::match(const std::string &text) const
{
    if (!m_Valid)
        return false;

    std::string match;
    Engine::Pattern &pattern = m_Syntax->getPattern();
    unsigned int start = 0;

    for (auto &i : pattern)
    {
        PRINT(std::cout << "\n   Matching: " << i->toPrettyString() << " => ";)

        auto [matched, current] = i->match(text, start);
        if (matched)
        {
            const std::string matchedText = text.substr(start, current - start);
            PRINT(std::cout << "Matched: '" << matchedText << "' ";)
            match += matchedText;
        }
        else
        {
            PRINT(std::cout << "Not matched" << std::endl;)
            return false;
        }
        start = current;
    }

    return match.size() == text.size();
}

std::optional<Engine::MatchInfo> Matcher::matchInfo(const std::string &text) const
{
    if (!m_Valid)
        return {};

    MatchInfo ret;
    ret.start = 0;

    std::string match;
    Engine::Pattern &pattern = m_Syntax->getPattern();
    unsigned int start = 0;

    for (auto &i : pattern)
    {
        PRINT(std::cout << "\n   Matching: " << i->toPrettyString() << " => ";)

        auto [matched, current] = i->match(text, start);
        if (matched)
        {
            const std::string matchedText = text.substr(start, current - start);
            PRINT(std::cout << "Matched: '" << matchedText << "' ";)
            match += matchedText;
        }
        else
        {
            PRINT(std::cout << "Not matched" << std::endl;)
            return {};
        }
        start = current;
    }

    if (match.size() == 0)
    {
        return {};
    }

    ret.match = match;
    return ret;
}

std::optional<MatchInfo> Matcher::matchGroups(const std::string &text) const
{
    if (!m_Valid)
        return {};

    MatchInfo ret;

    Engine::Pattern &patterns = m_Syntax->getPattern();
    unsigned int start = 0;

    std::string ctext = std::string(text);
    unsigned int subs = 0;
    Engine::Pos i = 0;
    for (; i < patterns.size(); i++)
    {
        auto &pattern = patterns[i];
        PRINT(std::cout << "\n   Matching: " << pattern->toPrettyString() << " => ";)

        auto [matched, current] = pattern->match(ctext, start, patterns.size() > 1);
        if (matched)
        {
            std::string matchedText = ctext.substr(start, current - start);
            PRINT(std::cout << "Matched: '" << matchedText << "' ";)
            if (matchedText.empty())
            {
                continue;
            }

            if (pattern->shouldIgnore())
            {
                // ret.groups[0].push_back(matchedText);
            }
            else if (pattern->shouldCapture())
            {
                ret.groups.push_back(MatchInfo(start, matchedText));
                ret.match += matchedText;
            }
            else
            {
                ret.match += matchedText;
            }
            ret.fullmatch += matchedText;
        }
        else
        {
            PRINT(std::cout << "Not matched" << std::endl;)
            if (ctext.size() == 1)
            {
                break;
            }
            ctext = ctext.substr(1);
            subs++;
            i--;
        }
        start = current;
    }

    if (i < patterns.size())
    {
        return {};
    }
    // m_MaxMatch = start;
    if (ret.fullmatch.size() != text.size())
        return std::nullopt;

    ret.start = subs;
    return ret;
}

std::optional<MatchInfo> Matcher::matchGroupsInfo(const std::string &text) const
{
    if (!m_Valid)
        return {};

    MatchInfo ret;
    // int group = 0;

    Engine::Pattern &patterns = m_Syntax->getPattern();
    unsigned int start = 0;
    auto current = start;
    std::vector<std::vector<MatchInfo>> groups;

    std::string ctext = std::string(text);
    unsigned int subs = 0;
    Engine::Pos i = 0;
    for (; i < patterns.size(); i++)
    {
        auto &pattern = patterns[i];
        PRINT(std::cout << "\n   Matching: " << pattern->toPrettyString() << " => ";)

        auto match = pattern->match_info(ctext, start, patterns.size() > 1);
        bool matched = match.has_value();

        if (matched)
        {
            current = match->start;
            std::string matchedText = match->match;
            PRINT(std::cout << "Matched: '" << matchedText << "' ";)

            // if (!match->groups.empty()) {
            //     groups.push_back(match->groups);
            //     for (auto& group : match->groups) {
            //         std::cout << group.match << std::endl;
            //     }
            // }

            if (matchedText.empty())
            {
                continue;
            }

            if (!pattern->shouldIgnore())
            {
                MatchInfo info = match.value();
                normalizeSpans(info, subs);
                Engine::AstNodeOps::collectGroups(ret, *pattern, info);
                ret.match += matchedText;
            }
            ret.fullmatch += matchedText;
        }
        else
        {
            PRINT(std::cout << "Not matched" << std::endl;)
            if (ctext.size() == 1)
            {
                break;
            }
            ctext = ctext.substr(1);
            subs++;
            i--;
        }
        start = current;
    }

    if (i < patterns.size())
    {
        return {};
    }
    // m_MaxMatch = start;
    if (ret.match.size() != text.size())
        return std::nullopt;

    ret.start = subs;
    ret.len = static_cast<unsigned int>(ret.match.size());
    return ret;
}

std::optional<MatchInfo> Matcher::findGroupsInfo(const std::string &text) const
{
    if (!m_Valid)
        return {};

    Engine::Pattern &patterns = m_Syntax->getPattern();

    // Try to anchor the whole pattern at every position, first match wins.
    for (unsigned int offset = 0; offset < text.size(); offset++)
    {
        const std::string ctext = text.substr(offset);
        MatchInfo ret;
        unsigned int start = 0;
        bool matchedAll = true;

        for (auto &pattern : patterns)
        {
            PRINT(std::cout << "\n   Matching: " << pattern->toPrettyString() << " => ";)

            auto match = pattern->match_info(ctext, start, patterns.size() > 1);
            if (!match.has_value())
            {
                PRINT(std::cout << "Not matched" << std::endl;)
                matchedAll = false;
                break;
            }

            PRINT(std::cout << "Matched: '" << match->match << "' ";)

            if (!match->match.empty() && !pattern->shouldIgnore())
            {
                MatchInfo info = match.value();
                normalizeSpans(info, offset);
                Engine::AstNodeOps::collectGroups(ret, *pattern, info);
                ret.match += match->match;
            }
            start = match->start;
        }

        if (!matchedAll || ret.match.empty())
            continue;

        ret.start = offset;
        ret.len = start;                        // characters consumed, ignored parts included
        ret.fullmatch = ctext.substr(0, start); // ...and their text
        return ret;
    }

    return {};
}

std::optional<std::list<MatchInfo>> Matcher::findAllGroupsInfo(const std::string &text) const
{
    if (!m_Valid)
        return {};

    std::list<MatchInfo> matches;

    unsigned int acc = 0;
    while (acc < text.size())
    {
        auto match = findGroupsInfo(text.substr(acc));
        if (!match.has_value())
            break;

        shiftSpans(match.value(), acc);
        // A zero-width match would spin here forever, so always step at least one char.
        acc = match->start + std::max(1u, match->len);
        matches.push_back(match.value());
    }

    if (matches.empty())
        return {};

    return matches;
}

std::optional<std::string> Matcher::find(const std::string &text) const
{
    if (auto info = findInfo(text); info.has_value())
    {
        return info.value().match;
    }

    return {};
}

std::optional<MatchInfo> Matcher::findInfo(const std::string &text) const
{
    if (!m_Valid)
        return {};

    std::string match;

    Engine::Pattern &patterns = m_Syntax->getPattern();
    unsigned int start = 0;

    std::string ctext = std::string(text);
    unsigned int subs = 0;
    Engine::Pos i = 0;
    for (; i < patterns.size(); i++)
    {
        auto &pattern = patterns[i];
        PRINT(std::cout << "\n   Matching: " << pattern->toPrettyString() << " => ";)

        auto [matched, current] = pattern->match(ctext, start);
        if (matched)
        {
            std::string matchedText = ctext.substr(start, current - start);
            PRINT(std::cout << "Matched: '" << matchedText << "' ";)
            match += matchedText;
        }
        else
        {
            PRINT(std::cout << "Not matched" << std::endl;)
            if (ctext.size() == 1)
            {
                break;
            }
            ctext = ctext.substr(1);
            subs++;
            i--;
        }
        start = current;
    }

    if (i < patterns.size())
    {
        return {};
    }
    // m_MaxMatch = start;
    return MatchInfo{subs, match};
}

std::optional<std::list<std::string>> Matcher::findAll(const std::string &text)
{
    auto info = findAllInfo(text);

    if (info.has_value())
    {
        std::list<std::string> matches;
        for (const auto &v : info.value())
        {
            matches.push_back(v.match);
        }
        return matches;
    }

    return {};
}

std::optional<std::list<MatchInfo>> Matcher::findAllInfo(const std::string &text)
{
    if (!m_Valid)
        return {};

    lastMaxLength = 0;

    std::list<MatchInfo> matches;
    auto ctext = std::string(text);

    unsigned int acc = 0;
    while (true)
    {
        auto match = findInfo(ctext);
        if (match.has_value())
        {
            auto value = match.value();
            if (lastMaxLength < value.match.size())
            {
                lastMaxLength = value.match.size();
            }
            acc += value.start;
            value.start = acc;
            matches.push_back(value);
            acc += value.match.size();

            ctext = text.substr(acc);
            if (ctext.empty())
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (!matches.empty())
    {
        return matches;
    }

    return {};
}

void Matcher::printTokens() const { m_Tokenizer->print_tokens(); }

void Matcher::printAst() const
{
    if (m_Valid)
        m_Syntax->printAst();
}

void Matcher::prettyPrint() const
{
    if (m_Valid)
    {
        for (auto &p : m_Syntax->getPattern())
        {
            std::cout << p->toPrettyString();
        }
        std::cout << std::endl;
    }
}
}; // namespace Utils::Regex