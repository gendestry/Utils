#include "Utils/Regex/Engine/AST/AstNodeParen.h"

#include "Utils/Colors/Font.h"

using namespace Utils::Regex::Engine;

unsigned int AstNodeParen::_match(std::string text, unsigned int st, bool ignoreAllMathced)
{
    unsigned int start = st;
    for (auto &subPattern : m_Ops)
    {
        bool allMatched = true;
        for (unsigned int i = 0; i < subPattern.size(); i++)
        {
            auto &op = subPattern[i];
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
    }

    return st;
}

Match AstNodeParen::match(std::string text, unsigned int start, bool ignoreAllMathced)
{
    unsigned int s = start;
    unsigned int m = _match(text, start, ignoreAllMathced);

    switch (m_OpType) {
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

std::string AstNodeParen::toString()
{
    std::string str = "AstOrNode[";
    for (auto &op : m_Ops)
    {
        for (auto &o : op)
        {
            str += o->toString();
        }
        str += " | ";
    }

    str = str.substr(0, str.size() - 3);
    str += "]";
    return str + toOpString();
}

std::string AstNodeParen::toPrettyString()
{
    std::string str = Font::colorMagenta + "(";
    for (auto &op : m_Ops)
    {
        for (auto &o : op)
        {
            str += o->toPrettyString();
        }
        str += " | ";
    }

    str += Font::colorMagenta + ")";
    return str + Font::colorReset + toOpString();
}