#include "Utils/Regex/Engine/AST/AstNodeEnclosure.h"

#include "Utils/Colors/Font.h"

using namespace Utils::Regex::Engine;

unsigned int AstNodeEnclosure::_match(std::string text, unsigned int st, bool ignoreAllMathced)
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

MatchInfo AstNodeEnclosure::_match_info(std::string text, unsigned int st, bool ignoreAllMathced)
{
    MatchInfo matchInfo;
    matchInfo.start = st;

    unsigned int start = st;
    bool allMatched = true;
    for (unsigned int i = 0; i < m_Ops.size(); i++) {
        auto &op = m_Ops[i];
        // std::cout << "Matching: " << op->toPrettyString() << std::endl;
        auto minfo = op->match_info(text, start);
        if (!minfo.has_value())
        {
            // std::cout << "SUB_Not matched" << std::endl;
            allMatched = false;
            break;
        }
        // std::cout << "SUB_Matched: '" << text.substr(start, current - start) << "'" << std::endl;

        start = minfo->start;
        matchInfo.start = start;
        matchInfo.match += minfo->match;

        collectGroups(matchInfo, *op, minfo.value());
    }

    if (!allMatched) {
        matchInfo.match = "";
        matchInfo.start = st;
        matchInfo.groups.clear();
    }

    return matchInfo;
}

std::vector<MatchInfo> AstNodeEnclosure::match_info_candidates(std::string text, unsigned int start, bool ignoreAllMatched)
{
    return applyRepetition(start, [&](unsigned int pos)
    {
        return matchSequenceCandidates(m_Ops, text, pos, ignoreAllMatched);
    });
}

// An enclosure is only there to group and to carry an operator, so it is never a capture
// itself: collectGroups() lifts whatever its children captured into the parent instead.
bool AstNodeEnclosure::shouldCapture() const { return false; }


std::string AstNodeEnclosure::toString()
{
    std::string str = "AstEnclosureNode[";
    for (auto &op : m_Ops)
    {
        str += op->toString() + " ";
    }

    str += "]";
    return str + toOpString();
}

std::string AstNodeEnclosure::toPrettyString()
{
    std::string str = Font::colorMagenta + "(";
    for (auto &op : m_Ops)
    {
        str += op->toPrettyString();
        str += " ";
    }

    str += Font::colorMagenta + ")";
    return str + Font::colorReset + toOpString();
}