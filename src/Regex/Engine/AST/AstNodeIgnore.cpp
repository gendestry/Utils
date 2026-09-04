#include "Utils/Regex/Engine/AST/AstNodeIgnore.h"

#include "Utils/Colors/Font.h"
#include "Utils/Text/LineCounter.h"
#include "Utils/Text/Stream.h"

using namespace Utils::Regex::Engine;

unsigned int AstNodeIgnore::_match(std::string text, unsigned int st, bool ignoreAllMathced)
{
    unsigned int start = st;
    bool allMatched = true;
    for (unsigned int i = 0; i < m_Ops.size(); i++)
    {
        auto &op = m_Ops[i];
        // std::cout << "Matching: " << op->toPrettyString() << std::endl;
        auto [matched, current] = op->match(text, start);
        if (matched)
        {
            // std::cout << "SUB_Matched: '" << text.substr(start, current - start) << "'" << std::endl;
        }
        else
        {
            // std::cout << "SUB_Not matched" << std::endl;
            allMatched = false;
            break;
        }
        start = current;
    }

    if (allMatched)
        return start;

    return st;
}

MatchInfo AstNodeIgnore::_match_info(std::string text, unsigned int st, bool ignoreAllMathced)
{
    MatchInfo matchInfo;
    matchInfo.start = st;

    unsigned int start = st;
    bool allMatched = true;
    for (unsigned int i = 0; i < m_Ops.size(); i++)
    {
        auto &op = m_Ops[i];
        // std::cout << "Matching: " << op->toPrettyString() << std::endl;
        auto minfo = op->match_info(text, start);
        if (!minfo.has_value())
        {
            // std::cout << "SUB_Not matched" << std::endl;
            allMatched = false;
            break;
        }
        // std::cout << "SUB_Matched: '" << text.substr(start, start - start) << "'" << std::endl;

        start = minfo->start;
        matchInfo.start = start;
        // matchInfo.match += minfo->match;
        // matchInfo.groups.push_back(minfo.value());
    }

    if (!allMatched) {
        matchInfo.match = "";
        matchInfo.start = st;
    }

    return matchInfo;
}

std::vector<MatchInfo> AstNodeIgnore::match_info_candidates(std::string text, unsigned int start, bool ignoreAllMatched)
{
    return applyRepetition(start, [&](unsigned int pos)
    {
        return matchSequenceCandidates(m_Ops, text, pos, ignoreAllMatched);
    });
}

std::string AstNodeIgnore::toString()
{
    std::string str = "AstIgnoreNode[";
    for (auto &op : m_Ops)
    {
        str += op->toString();
    }

    str += "]";
    return str + toOpString();
}

std::string AstNodeIgnore::toPrettyString()
{
    Utils::Text::Stream s;
    s << Font::colorYellow << "!{" << Font::colorReset;
    for (auto op : m_Ops)
    {
        s << op->toPrettyString();
    }

    // str = str.substr(0, str.size() - 3);
    s << Font::colorYellow << "}"<< Font::colorReset << toOpString();
    return s.end();
}