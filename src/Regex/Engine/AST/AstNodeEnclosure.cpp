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

Match AstNodeEnclosure::match(std::string text, unsigned int start, bool ignoreAllMathced)
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

bool AstNodeEnclosure::shouldCapture() const {
    for (auto &op : m_Ops) {
        if (op->shouldCapture())
            return true;
    }

    return false;
}


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