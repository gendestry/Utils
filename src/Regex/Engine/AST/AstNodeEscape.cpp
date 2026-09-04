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
    case ANY:
        return start + 1;
    }

    return start;
}

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
    case ANY:
        matchInfo.match = text[start];
        matchInfo.start = start + 1;
        break;
    }

    return matchInfo;
}
std::string AstNodeEscape::toString()
{
    return "EscapeNode[" + toEscapeString() + "]" + toOpString();
}

std::string AstNodeEscape::toPrettyString()
{
    return Font::colorRed + toEscapeString() + Font::colorReset + toOpString();
}