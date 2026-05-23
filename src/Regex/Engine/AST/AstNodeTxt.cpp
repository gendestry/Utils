#include "Utils/Regex/Engine/AST/AstNodeTxt.h"

#include "Utils/Colors/Font.h"

using namespace Utils::Regex::Engine;

unsigned int AstNodeTxt::_match(std::string text, unsigned int start, bool ignoreAllMathced)
{
    if (start >= text.size() || start + txt.size() > text.size())
        return start;

    if (text.substr(start, txt.size()) == txt)
        return start + txt.size();

    return start;
}

// Match AstNodeTxt::match(std::string text, unsigned int start, bool ignoreAllMathced)
// {
//     if (start >= text.size() || start + txt.size() > text.size()) {
//         if (isOtional()) {
//             return {true, start};
//         }
//         return {false, start};
//     }
//
//     unsigned int s = start;
//     unsigned int m = _match(text, start, ignoreAllMathced);
//
//     switch (m_OpType)
//     {
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
//
MatchInfo AstNodeTxt::_match_info(std::string text, unsigned int start, bool ignoreAllMathced)
{
    MatchInfo matchInfo;
    matchInfo.start = start;

    if (text.substr(start, txt.size()) == txt) {
        matchInfo.start = start + txt.size();
        matchInfo.match = txt;
    }

    return matchInfo;
}

// std::optional<MatchInfo> AstNodeTxt::match_info(std::string text, unsigned int start, bool ignoreAllMathced)
// {
//     if (start >= text.size() || start + txt.size() > text.size()) {
//         if (isOtional()) {
//             return MatchInfo("", start);
//         }
//         return {};
//     }
//
//     MatchInfo mi = _match_info(text, start, ignoreAllMathced);
//     unsigned int s = start;
//     unsigned int m = mi.current;
//
//     MatchInfo tm = mi;
//
//     switch (m_OpType)
//     {
//         case NONE:
//             return mi;
//         case PLUS:
//             if (m != s)
//             {
//                 // s = m;
//                 // m = _match(text, s, ignoreAllMathced);
//                 s = m;
//                 tm = _match_info(text, s, ignoreAllMathced);
//                 m = tm.current;
//                 mi.match += tm.match;
//
//                 while (m != s)
//                 {
//                     s = m;
//                     tm = _match_info(text, s, ignoreAllMathced);
//                     m = tm.current;
//                     mi.match += tm.match;
//                 }
//
//                 mi.current = m;
//                 return mi;
//             }
//
//             return {};
//         case ASTERIX:
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
//         case QUESTION_MARK:
//             return mi;
//         case RANGE:
//             auto r1 = m_Range.start;
//             auto r2 = m_Range.end;
//             int i = 0;
//             // min ammount
//             for (; i < r1; i++) {
//                 if (m != s) {
//                     s = m;
//                     tm = _match_info(text, s, ignoreAllMathced);
//                     m = tm.current;
//                     mi.match += tm.match;
//                 }
//                 else {
//                     return {};
//                 }
//             }
//
//             while (m != s)
//             {
//                 s = m;
//                 tm = _match_info(text, s, ignoreAllMathced);
//                 m = tm.current;
//                 mi.match += tm.match;
//                 i++;
//             }
//
//             if (i <= r2)
//             {
//                 mi.current = m;
//                 return mi;
//             }
//
//             return {};
//     }
//
//
//     return {};
//     if (start >= text.size())
//         return {};
//
//
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

std::string AstNodeTxt::toString()
{
    return "TxtNode['" + txt + "']" + toOpString();
}

std::string AstNodeTxt::toPrettyString()
{
    return Font::colorBlue + "'" + txt + "'" + Font::colorReset + toOpString();
}