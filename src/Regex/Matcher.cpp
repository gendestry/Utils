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

// Backtracking driver, defined below; declared here so the anchored entry points can use it.
std::optional<MatchInfo> tryMatchFrom(Pattern &patterns, unsigned int i, const std::string &text,
                                      unsigned int pos, MatchInfo acc, bool requireEnd = false);

namespace
{
// Anchored match: the pattern must consume the whole text.
std::optional<MatchInfo> matchWhole(Pattern &patterns, const std::string &text)
{
    MatchInfo seed;
    seed.start = 0;

    auto result = tryMatchFrom(patterns, 0, text, 0, seed, true);
    if (!result)
        return {};

    normalizeSpans(*result, 0);
    result->start = 0;
    result->len = static_cast<unsigned int>(result->match.size());
    return result;
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

    return matchWhole(m_Syntax->getPattern(), text).has_value();
}

std::optional<Engine::MatchInfo> Matcher::matchInfo(const std::string &text) const
{
    if (!m_Valid)
        return {};

    auto result = matchWhole(m_Syntax->getPattern(), text);
    if (!result || result->match.empty())
        return {};

    result->groups.clear();
    return result;
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

    // Anchored counterpart of findGroupsInfo: same backtracking machinery, but the
    // pattern has to cover the whole text.
    auto result = matchWhole(m_Syntax->getPattern(), text);
    if (!result || result->match.empty())
        return {};

    result->fullmatch = result->match;
    return result;
}

std::optional<MatchInfo> tryMatchFrom(Pattern &patterns, unsigned int i, const std::string &text, unsigned int pos, MatchInfo acc, bool requireEnd)
{
    if (i == patterns.size())
    {
        // Anchored callers need a candidate that consumes everything; keep
        // backtracking instead of accepting one that stops short.
        if (requireEnd && pos != text.size())
            return {};

        acc.start = pos;
        return acc;
    }

    auto &pattern = patterns[i];
    for (auto &candidate : pattern->match_info_candidates(text, pos))
    {
        MatchInfo next = acc;

        if (!candidate.match.empty() && !pattern->shouldIgnore())
        {
            next.match += candidate.match;
            Engine::AstNodeOps::collectGroups(next, *pattern, candidate);
        }

        if (auto result = tryMatchFrom(patterns, i + 1, text, candidate.start, next, requireEnd))
            return result;   // this candidate led to a full match — done
        // else: fall through and try this pattern's next (shorter) candidate
    }

    return std::nullopt;   // no candidate for patterns[i] leads to a full match
}

std::optional<MatchInfo> Matcher::findGroupsInfo(const std::string &text) const
{
    if (!m_Valid)
        return {};

    Pattern &patterns = m_Syntax->getPattern();

    for (unsigned int offset = 0; offset < text.size(); offset++)
    {
        const std::string ctext = text.substr(offset);

        MatchInfo seed;
        seed.start = 0;

        auto result = tryMatchFrom(patterns, 0, ctext, 0, seed);
        if (!result || result->match.empty())
            continue;

        normalizeSpans(*result, offset);
        result->start = offset;
        result->len = result->match.size();       // note: see caveat below
        return result;
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

    // Same scan as findGroupsInfo - the two differ only in whether groups are kept.
    auto result = findGroupsInfo(text);
    if (!result)
        return {};

    result->groups.clear();
    return result;
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
            // A zero-width match would spin here forever, so always step at least one char.
            acc += std::max<std::size_t>(1u, value.match.size());

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