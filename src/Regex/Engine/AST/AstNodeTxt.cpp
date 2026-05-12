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

Match AstNodeTxt::match(std::string text, unsigned int start, bool ignoreAllMathced)
{
    if (start >= text.size() || start + txt.size() > text.size()) {
        if (isOtional()) {
            return {true, start};
        }
        return {false, start};
    }

    unsigned int s = start;
    unsigned int m = _match(text, start, ignoreAllMathced);

    switch (m_OpType)
    {
        case NONE:
            return {m != s, m};
        case PLUS:
            if (m != s)
            {
                s = m;
                m = _match(text, s, ignoreAllMathced);

                while (m != s)
                {
                    s = m;
                    m = _match(text, s, ignoreAllMathced);
                }

                return {true, m};
            }

            return {false, s};
        case ASTERIX:
            while (m != s)
            {
                s = m;
                m = _match(text, s, ignoreAllMathced);
            }

            return {true, m};
        case QUESTION_MARK:
            return {true, m};
        case RANGE:
            auto r1 = m_Range.start;
            auto r2 = m_Range.end;
            int i = 0;
            // min ammount
            for (; i < r1; i++) {
                if (m != s) {
                    s = m;
                    m = _match(text, s, ignoreAllMathced);
                }
                else {
                    return {false, s};
                }
            }

            while (m != s)
            {
                s = m;
                m = _match(text, s, ignoreAllMathced);
                i++;
            }

            if (i <= r2)
            {
                return {true, m};
            }

            return {false, s};
    }


    return {false, s};
}

std::string AstNodeTxt::toString()
{
    return "TxtNode['" + txt + "']" + toOpString();
}

std::string AstNodeTxt::toPrettyString()
{
    return Font::colorBlue + "'" + txt + "'" + Font::colorReset + toOpString();
}