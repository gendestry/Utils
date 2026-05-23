#include "Utils/Regex/Engine/AST/AstNodeCapture.h"

#include "Utils/Colors/Font.h"
#include "Utils/Text/LineCounter.h"
#include "Utils/Text/Stream.h"

using namespace Utils::Regex::Engine;

unsigned int AstNodeCapture::_match(std::string text, unsigned int st, bool ignoreAllMathced)
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
            // std::cout << "SUB_Matched: '" << text.substr(start, start - start) << "'" << std::endl;
        }
        else
        {
            // std::cout << "SUB_Not matched" << std::endl;
            allMatched = false;
            break;
        }
        start = start;
    }

    if (allMatched)
        return start;

    return st;
}

// Match AstNodeCapture::match(std::string text, unsigned int start, bool ignoreAllMathced) {
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
//     }
//
//
//     return {false, s};
// }

MatchInfo AstNodeCapture::_match_info(std::string text, unsigned int st, bool ignoreAllMathced)
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
        matchInfo.match += minfo->match;
        // matchInfo.groups.push_back(minfo.value());
    }

    if (!allMatched) {
        matchInfo.match = "";
        matchInfo.start = st;
    }

    return matchInfo;
}
//
// std::optional<MatchInfo> AstNodeCapture::match_info(std::string text, unsigned int start, bool ignoreAllMathced)
// {
//     if (start >= text.size())
//         return {};
//
//     unsigned int s = start;
//     MatchInfo mi = _match_info(text, start, ignoreAllMathced);
//     unsigned int m = mi.start;
//
//     MatchInfo tm = mi;
//     mi.groups.push_back(tm);
//
//     switch (m_OpType)
//     {
//     case NONE:
//             if (mi.start == start) {
//                 return {};
//             }
//             return mi;
//         // return {m != s, m};
//     case PLUS:
//         if (m != s)
//         {
//             s = m;
//             tm = _match_info(text, s, ignoreAllMathced);
//             m = tm.start;
//             mi.match += tm.match;
//             mi.groups.push_back(tm);
//
//             while (m != s)
//             {
//                 s = m;
//                 tm = _match_info(text, s, ignoreAllMathced);
//                 m = tm.start;
//                 mi.match += tm.match;
//                 mi.groups.push_back(tm);
//             }
//
//             mi.start = m;
//             return mi;
//         }
//
//         return {};
//     case ASTERIX:
//         while (m != s)
//         {
//             s = m;
//             tm = _match_info(text, s, ignoreAllMathced);
//             m = tm.start;
//             mi.match += tm.match;
//             mi.groups.push_back(tm);
//         }
//
//         mi.start = m;
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
//                 m = tm.start;
//                 mi.match += tm.match;
//                 mi.groups.push_back(tm);
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
//             m = tm.start;
//             mi.match += tm.match;
//             mi.groups.push_back(tm);
//             i++;
//         }
//
//         if (i <= r2)
//         {
//             mi.start = m;
//             return mi;
//         }
//
//         return {};
//     }
//
//     return {};
// }

std::string AstNodeCapture::toString()
{
    std::string str = "AstCaptureNode[";
    for (auto &op : m_Ops)
    {
        str += op->toString();
    }

    str += "]";
    return str + toOpString();
}

std::string AstNodeCapture::toPrettyString()
{
    Utils::Text::Stream s;
    s << Font::colorYellow << "{" << Font::colorReset;
    for (auto op : m_Ops)
    {
        s << op->toPrettyString();
    }

    // str = str.substr(0, str.size() - 3);
    s << Font::colorYellow << "}"<< Font::colorReset << toOpString();
    return s.end();
}