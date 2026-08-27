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

// Match AstNodeEnclosure::match(std::string text, unsigned int start, bool ignoreAllMathced)
// {
//     unsigned int s = start;
//     unsigned int m = _match(text, start, ignoreAllMathced);
//
//     switch (m_OpType) {
//         case NONE:
//             return {m != s, m};
//         case PLUS:
//             if (m != s)
//             {
//                 s = m;
//                 m = _match(text, s, ignoreAllMathced);
//
//                 while (m != s)
//                 {
//                     s = m;
//                     m = _match(text, s, ignoreAllMathced);
//                 }
//
//                 return {true, m};
//             }
//
//             return {false, s};
//         case ASTERIX:
//             while (m != s)
//             {
//                 s = m;
//                 m = _match(text, s, ignoreAllMathced);
//             }
//
//             return {true, m};
//         case QUESTION_MARK:
//             return {true, m};
//         case RANGE:
//             auto r1 = m_Range.start;
//             auto r2 = m_Range.end;
//             int i = 0;
//             // min ammount
//             for (; i < r1; i++) {
//                 if (m != s) {
//                     s = m;
//                     m = _match(text, s, ignoreAllMathced);
//                 }
//                 else {
//                     return {false, s};
//                 }
//             }
//
//             while (m != s)
//             {
//                 s = m;
//                 m = _match(text, s, ignoreAllMathced);
//                 i++;
//             }
//
//             if (i <= r2)
//             {
//                 return {true, m};
//             }
//
//             return {false, s};
//         }
//
//
//     return {false, s};
// }

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
//
// std::optional<MatchInfo> AstNodeEnclosure::match_info(std::string text, unsigned int start, bool ignoreAllMathced)
// {
//     if (start >= text.size())
//         return {};
//
//     MatchInfo mi = _match_info(text, start, ignoreAllMathced);
//     unsigned int s = start;
//     unsigned int m = mi.current;
//
//     MatchInfo tm = mi;
//
//
//     switch (m_OpType)
//     {
//     case NONE:
//             if (mi.current == start) {
//                 return {};
//             }
//             return mi;
//         // return {m != s, m};
//     case PLUS:
//         if (m != s)
//         {
//             s = m;
//             tm = _match_info(text, s, ignoreAllMathced);
//             m = tm.current;
//             mi.match += tm.match;
//
//             while (m != s)
//             {
//                 s = m;
//                 tm = _match_info(text, s, ignoreAllMathced);
//                 m = tm.current;
//                 mi.match += tm.match;
//             }
//
//             mi.current = m;
//             return mi;
//         }
//
//         return {};
//     case ASTERIX:
//         while (m != s)
//         {
//             s = m;
//             tm = _match_info(text, s, ignoreAllMathced);
//             m = tm.current;
//             mi.match += tm.match;
//         }
//
//         mi.current = m;
//         return mi;
//     case QUESTION_MARK:
//         return mi;
//     case RANGE:
//         auto r1 = m_Range.start;
//         auto r2 = m_Range.end;
//         int i = 0;
//         // min ammount
//         for (; i < r1; i++) {
//             if (m != s) {
//                 s = m;
//                 tm = _match_info(text, s, ignoreAllMathced);
//                 m = tm.current;
//                 mi.match += tm.match;
//             }
//             else {
//                 return {};
//             }
//         }
//
//         while (m != s)
//         {
//             s = m;
//             tm = _match_info(text, s, ignoreAllMathced);
//             m = tm.current;
//             mi.match += tm.match;
//             i++;
//         }
//
//         if (i <= r2)
//         {
//             mi.current = m;
//             return mi;
//         }
//
//         return {};
//     }
//
//     return {};
// }

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