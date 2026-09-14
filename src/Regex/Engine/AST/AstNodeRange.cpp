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