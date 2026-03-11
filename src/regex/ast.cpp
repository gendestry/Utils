#include "ast.h"
#include "../utils/font.h"
#include <iostream>

using namespace Utils;

namespace Regex
{

    unsigned int AstNodeParen::_match(std::string text, unsigned int st)
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

    Match AstNodeParen::match(std::string text, unsigned int start)
    {
        unsigned int s = start;
        unsigned int m = _match(text, start);

        switch (m_OpType)
        {
        case NONE:
            return {m != s, m};
        case PLUS:
            if (m != s)
            {
                s = m;
                m = _match(text, s);

                while (m != s)
                {
                    s = m;
                    m = _match(text, s);
                }

                return {true, m};
            }

            return {false, s};
        case ASTERIX:
            while (m != s)
            {
                s = m;
                m = _match(text, s);
            }

            return {true, m};
        case QUESTION_MARK:
            return {true, m};
        }

        return {false, s};
    }

    unsigned int AstNodeEscape::_match(std::string text, unsigned int start)
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

    Match AstNodeEscape::match(std::string text, unsigned int start)
    {
        if (start >= text.size())
            return {false, start};

        unsigned int s = start;
        unsigned int m = _match(text, start);

        switch (m_OpType)
        {
        case NONE:
            return {m != s, m};
        case PLUS:
            if (m != s)
            {
                s = m;
                m = _match(text, s);

                while (m != s)
                {
                    s = m;
                    m = _match(text, s);
                }

                return {true, m};
            }

            return {false, s};
        case ASTERIX:
            while (m != s)
            {
                s = m;
                m = _match(text, s);
            }

            return {true, m};
        case QUESTION_MARK:
            return {true, m};
        }

        return {false, s};
    }

    unsigned int AstNodeTxt::_match(std::string text, unsigned int start)
    {
        if (start >= text.size() || start + txt.size() > text.size())
            return start;

        if (text.substr(start, txt.size()) == txt)
            return start + txt.size();

        return start;
    }

    Match AstNodeTxt::match(std::string text, unsigned int start)
    {
        if (start >= text.size() || start + txt.size() > text.size())
            return {false, start};

        unsigned int s = start;
        unsigned int m = _match(text, start);

        switch (m_OpType)
        {
        case NONE:
            return {m != s, m};
        case PLUS:
            if (m != s)
            {
                s = m;
                m = _match(text, s);

                while (m != s)
                {
                    s = m;
                    m = _match(text, s);
                }

                return {true, m};
            }

            return {false, s};
        case ASTERIX:
            while (m != s)
            {
                s = m;
                m = _match(text, s);
            }

            return {true, m};
        case QUESTION_MARK:
            return {true, m};
        }

        return {false, s};
    }

    unsigned int AstNodeRange::_match(std::string text, unsigned int start)
    {
        if (start >= text.size())
            return start;

        if (m_Start <= text[start] && text[start] <= m_End)
            return start + 1;

        return start;
    }

    Match AstNodeRange::match(std::string text, unsigned int start)
    {
        unsigned int s = start;
        unsigned int m = _match(text, start);

        switch (m_OpType)
        {
        case NONE:
            return {m != s, m};
        case PLUS:
            if (m != s)
            {
                s = m;
                m = _match(text, s);

                while (m != s)
                {
                    s = m;
                    m = _match(text, s);
                }

                return {true, m};
            }

            return {false, s};
        case ASTERIX:
            while (m != s)
            {
                s = m;
                m = _match(text, s);
            }

            return {true, m};
        case QUESTION_MARK:
            return {true, m};
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

    std::string AstNodeEscape::toString()
    {
        return "EscapeNode[" + toEscapeString() + "]" + toOpString();
    }

    std::string AstNodeTxt::toString()
    {
        return "TxtNode['" + txt + "']" + toOpString();
    }

    std::string AstNodeRange::toString()
    {
        return "RangeNode[" + std::to_string(m_Start) + "-" + std::to_string(m_End) + "]" + toOpString();
    }

    std::string AstNodeParen::toPrettyString()
    {
        std::string str = Font::fmagenta + "(";
        for (auto &op : m_Ops)
        {
            for (auto &o : op)
            {
                str += o->toPrettyString();
            }
            str += " | ";
        }

        str = str.substr(0, str.size() - 3);
        str += Font::fmagenta + ")";
        return str + toOpString() + Font::reset;
    }

    std::string AstNodeEscape::toPrettyString()
    {
        return Font::fred + toEscapeString() + toOpString() + Font::reset;
    }

    std::string AstNodeTxt::toPrettyString()
    {
        return Font::fblue + "'" + txt + toOpString() + "'" + Font::reset;
    }

    std::string AstNodeRange::toPrettyString()
    {
        std::string ret = "[";
        ret += m_Start;
        ret += "-";
        ret += m_End;
        ret += "]";
        return Font::fgreen + ret + toOpString() + Font::reset;
    }
};