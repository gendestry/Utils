#include "Utils/Regex/Engine/AST/AstNodeEscape.h"

#include "Utils/Colors/Font.h"

using namespace Utils::Regex::Engine;

unsigned int AstNodeEscape::_match(std::string text, unsigned int start, bool ignoreAllMathced)
{
    if (start >= text.size())
        return start;

    auto checkSmallChar = [](char c) -> bool
    { return c >= 'a' && c <= 'z'; };

    auto checkBigChar = [](char c) -> bool
    { return c >= 'A' && c <= 'Z'; };

    auto checkDigit = [](char c) -> bool
    { return c >= '0' && c <= '9'; };

    switch (m_EscapeType)
    {
    case CHAR:
        if (checkSmallChar(text[start]))
        {
            return start + 1;
        }
        break;
    case BIG_CHAR:
        if (checkBigChar(text[start]))
        {
            return start + 1;
        }
        break;
    case ANY_CHAR:
        if (checkSmallChar(text[start]) || checkBigChar(text[start]))
        {
            return start + 1;
        }
        break;
    case DIGIT:
        if (checkDigit(text[start]))
        {
            return start + 1;
        }
        break;
    case NEWLINE:
        if (text[start] == '\n')
        {
            return start + 1;
        }
        break;
    }

    return start;
}

// Match AstNodeEscape::match(std::string text, unsigned int start, bool ignoreAllMathced)
// {
//     if (start >= text.size())
//         return {false, start};
//
//     unsigned int s = start;
//     unsigned int m = _match(text, start, ignoreAllMathced);
//
//     switch (m_OpType)
//     {
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
//     }
//
//     return {false, s};
// }

MatchInfo AstNodeEscape::_match_info(std::string text, unsigned int start, bool ignoreAllMathced)
{
    MatchInfo matchInfo;
    matchInfo.start = start;

    if (start >= text.size())
        return matchInfo;

    auto checkSmallChar = [](char c) -> bool
    { return c >= 'a' && c <= 'z'; };

    auto checkBigChar = [](char c) -> bool
    { return c >= 'A' && c <= 'Z'; };

    auto checkDigit = [](char c) -> bool
    { return c >= '0' && c <= '9'; };

    switch (m_EscapeType)
    {
    case CHAR:
        if (checkSmallChar(text[start]))
        {
            matchInfo.match = text[start];
            matchInfo.start = start + 1;
        }
        break;

    case BIG_CHAR:
        if (checkBigChar(text[start]))
        {
            matchInfo.match = text[start];
            matchInfo.start = start + 1;
        }
        break;
    case ANY_CHAR:
        if (checkSmallChar(text[start]) || checkBigChar(text[start]))
        {
            matchInfo.match = text[start];
            matchInfo.start = start + 1;
        }
        break;
    case DIGIT:
        if (checkDigit(text[start]))
        {
            matchInfo.match = text[start];
            matchInfo.start = start + 1;
        }
        break;
    case NEWLINE:
        if (text[start] == '\n')
        {
            matchInfo.match = text[start];
            matchInfo.start = start + 1;
        }
        break;
    }

    return matchInfo;
}
//
// std::optional<MatchInfo> AstNodeEscape::match_info(std::string text, unsigned int start, bool ignoreAllMathced)
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

std::string AstNodeEscape::toString()
{
    return "EscapeNode[" + toEscapeString() + "]" + toOpString();
}

std::string AstNodeEscape::toPrettyString()
{
    return Font::colorRed + toEscapeString() + Font::colorReset + toOpString();
}