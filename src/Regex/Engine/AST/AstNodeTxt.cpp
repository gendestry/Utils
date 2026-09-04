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

std::string AstNodeTxt::toString()
{
    return "TxtNode['" + txt + "']" + toOpString();
}

std::string AstNodeTxt::toPrettyString()
{
    return Font::colorBlue + "'" + txt + "'" + Font::colorReset + toOpString();
}