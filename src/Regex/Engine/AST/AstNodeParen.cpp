#include "Utils/Regex/Engine/AST/AstNodeParen.h"

#include "Utils/Colors/Font.h"

using namespace Utils::Regex::Engine;

unsigned int AstNodeParen::_match(std::string text, unsigned int st, bool ignoreAllMathced)
{
    unsigned int start = st;
    for (auto &subPattern : m_Ops)
    {
        bool allMatched = true;
        for (unsigned int i = 0; i < subPattern.size(); i++)
        {
            auto &op = subPattern[i];
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
    }

    return st;
}

MatchInfo AstNodeParen::_match_info(std::string text, unsigned int st, bool ignoreAllMathced)
{
    MatchInfo matchInfo;
    matchInfo.start = st;

    for (auto &subPattern : m_Ops)
    {
        // Each alternative starts over: a half-matched one must not leave its text or
        // its captures behind for the next.
        matchInfo = MatchInfo{};
        matchInfo.start = st;
        unsigned int start = st;

        bool allMatched = true;
        for (unsigned int i = 0; i < subPattern.size(); i++)
        {
            auto &op = subPattern[i];
            // std::cout << "Matching: " << op->toPrettyString() << std::endl;
            // auto [matched, current] = op->match(text, start);
            auto minfo = op->match_info(text, start);

            if (minfo.has_value())
            {
                // std::cout << "SUB_Matched: '" << text.substr(start, current - start) << "'" << std::endl;
            }
            else
            {
                // std::cout << "SUB_Not matched" << std::endl;
                allMatched = false;
                break;
            }
            start = minfo->start;
            matchInfo.start = start;
            matchInfo.match += minfo->match;
            collectGroups(matchInfo, *op, minfo.value());
        }

        if (allMatched)
            return matchInfo;
    }

    MatchInfo info;
    info.start = st;
    return info;
}

std::vector<MatchInfo> AstNodeParen::match_info_candidates(std::string text, unsigned int start, bool ignoreAllMatched)
{
    return applyRepetition(start, [&](unsigned int pos)
    {
        std::vector<MatchInfo> candidates;
        for (auto &branch : m_Ops)
        {
            auto branchCandidates = matchSequenceCandidates(branch, text, pos, ignoreAllMatched);
            candidates.insert(candidates.end(), branchCandidates.begin(), branchCandidates.end());
        }
        return candidates;
    });
}

std::string AstNodeParen::toString()
{
    std::string str = "AstOrNode[";
    for (auto &op : m_Ops)
    {
        for (auto &o : op)
        {
            str += o->toString();
        }
        str += " | ";
    }

    str = str.substr(0, str.size() - 3);
    str += "]";
    return str + toOpString();
}

std::string AstNodeParen::toPrettyString()
{
    std::string str = Font::colorMagenta + "(";

    for (int i = 0; i < m_Ops.size(); i++)
    {
        auto &op = m_Ops[i];
        for (auto &o : op)
        {
            str += o->toPrettyString();
        }
        if (i != m_Ops.size() - 1)
            str += " | ";
    }

    str += Font::colorMagenta + ")";
    return str + Font::colorReset + toOpString();
}