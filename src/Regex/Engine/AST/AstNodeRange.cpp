#include "Utils/Regex/Engine/AST/AstNodeRange.h"

#include "Utils/Colors/Font.h"
#include "Utils/Text/LineCounter.h"
#include "Utils/Text/Stream.h"

using namespace Utils::Regex::Engine;

unsigned int AstNodeRange::_match(std::string text, unsigned int start, bool ignoreAllMathced)
{
if (start >= text.size())
    return start;

if (m_Start <= text[start] && text[start] <= m_End)
    return start + 1;

return start;
}

// Match AstNodeRange::match(std::string text, unsigned int start, bool ignoreAllMathced)
// {
// unsigned int s = start;
// unsigned int m = _match(text, start, ignoreAllMathced);
//
// switch (m_OpType)
// {
//     case NONE:
//         return {m != s, m};
//     case PLUS:
//         if (m != s)
//         {
//             s = m;
//             m = _match(text, s, ignoreAllMathced);
//
//             while (m != s)
//             {
//                 s = m;
//                 m = _match(text, s, ignoreAllMathced);
//             }
//
//             return {true, m};
//         }
//
//         return {false, s};
//     case ASTERIX:
//         while (m != s)
//         {
//             s = m;
//             m = _match(text, s, ignoreAllMathced);
//         }
//
//         return {true, m};
//     case QUESTION_MARK:
//         return {true, m};
//     case RANGE:
//         auto r1 = m_Range.start;
//         auto r2 = m_Range.end;
//         int i = 0;
//         // min ammount
//         for (; i < r1; i++) {
//             if (m != s) {
//                 s = m;
//                 m = _match(text, s, ignoreAllMathced);
//             }
//             else {
//                 return {false, s};
//             }
//         }
//
//         while (m != s)
//         {
//             s = m;
//             m = _match(text, s, ignoreAllMathced);
//             i++;
//         }
//
//         if (i <= r2)
//         {
//             return {true, m};
//         }
//
//         return {false, s};
// }
//
// return {false, s};
// }
//
MatchInfo AstNodeRange::_match_info(std::string text, unsigned int start, bool ignoreAllMathced)
{
    MatchInfo matchInfo;
    matchInfo.start = start;

    if (m_Start <= text[start] && text[start] <= m_End) {
        matchInfo.start += 1;
        matchInfo.match = text[start];
    }

    return matchInfo;
}
//
// std::optional<MatchInfo> AstNodeRange::match_info(std::string text, unsigned int start, bool ignoreAllMathced)
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

std::string AstNodeRange::toString()
{
    Utils::Text::Stream s;
    s << "RangeNode[" << m_Start << "-" << m_End << "]" << toOpString();
    return s.end();
    // return "RangeNode[" + (m_Start) + "-" + (m_End) + "]" + toOpString();
}

std::string AstNodeRange::toPrettyString()
{
    std::string ret = "[";
    ret += m_Start;
    ret += "-";
    ret += m_End;
    ret += "]";
    return Font::colorGreen + ret + Font::colorReset + toOpString();
}