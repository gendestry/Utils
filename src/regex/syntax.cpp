#include "syntax.h"
#include <iostream>

namespace Regex
{
    bool Syntax::parse()
    {
        if (isInter())
        {
            while (isInter())
                ;
            return true;
        }

        return false;
    }

    bool Syntax::isInter()
    {
        Pos old = m_TokenPos;
        if (isParen())
        {
            m_AstTree.push_back(m_Op);
            // std::cout << "Paren: " << old << ", " << m_TokenPos << std::endl;
            return true;
        }
        else if (isEscapeOp())
        {
            m_AstTree.push_back(m_Op);
            // std::cout << "Escape: " << old << ", " << m_TokenPos << std::endl;
            return true;
        }
        else if (isTxtOp())
        {
            m_AstTree.push_back(m_Op);
            // std::cout << "Txt: " << old << ", " << m_TokenPos << std::endl;
            return true;
        }
        else if (isRangeOp())
        {
            m_AstTree.push_back(m_Op);
            // std::cout << "Range: " << old << ", " << m_TokenPos << std::endl;
            return true;
        }

        m_TokenPos = old;
        return false;
    }

    bool Syntax::isParen()
    {
        std::vector<std::vector<AstNodeOps *>> or_ops;
        std::vector<AstNodeOps *> ops;

        auto isOr = [&]() -> bool
        {
            Pos old = m_TokenPos;
            if (m_Tokens[m_TokenPos++].type == Token::OR)
            {
                if (isParen() || isEscapeOp() || isTxtOp() || isRangeOp())
                {
                    ops.push_back(m_Op);
                    while (isParen() || isEscapeOp() || isTxtOp() || isRangeOp())
                    {
                        ops.push_back(m_Op);
                    }

                    or_ops.push_back(ops);
                    ops.clear();
                    return true;
                }
            }

            m_TokenPos = old;
            return false;
        };

        Pos old = m_TokenPos;

        if (m_Tokens[m_TokenPos].type == Token::LPAREN)
        {
            // lpar {inter+} (or inter+)+ rpar operators?
            m_TokenPos++;
            if (isParen() || isEscapeOp() || isTxtOp() || isRangeOp())
            {
                ops.push_back(m_Op);
                while (isParen() || isEscapeOp() || isTxtOp() || isRangeOp())
                {
                    ops.push_back(m_Op);
                }

                or_ops.push_back(ops);
                ops.clear();
            }
            else
            {
                m_TokenPos = old;
                return false;
            }

            // lpar inter+ {(or inter+)}+ rpar operators?
            if (!isOr())
            {
                m_TokenPos = old;
                return false;
            }

            // lpar inter+ (or inter+){+} rpar operators?
            while (isOr())
                ;

            if (m_Tokens[m_TokenPos++].type == Token::RPAREN)
            {
                isOperator();
                m_Op = new AstNodeParen({m_Tokens[old].startPos, m_Tokens[m_TokenPos].endPos}, or_ops, m_OpType);
                return true;
            }
        }

        m_TokenPos = old;
        return false;
    }

    bool Syntax::isEscapeOp()
    {
        Pos old = m_TokenPos;
        if (isEscape())
        {
            isOperator();

            m_Op = new AstNodeEscape({m_Tokens[old].startPos, m_Tokens[m_TokenPos].endPos}, m_EscapeType, m_OpType);
            return true;
        }

        m_TokenPos = old;
        return false;
    }

    bool Syntax::isTxtOp()
    {
        if (m_TokenPos >= m_Tokens.size())
            return false;

        Pos old = m_TokenPos;
        if (m_Tokens[m_TokenPos].type == Token::TXT)
        {
            m_TokenPos++;
            isOperator();

            m_Op = new AstNodeTxt({m_Tokens[old].startPos, m_Tokens[m_TokenPos].endPos}, m_Tokens[old].txt_value, m_OpType);
            return true;
        }

        m_TokenPos = old;
        return false;
    }

    bool Syntax::isRangeOp()
    {
        auto isValidCharToken = [&]() -> bool
        {
            return m_Tokens[m_TokenPos].type == Token::C || m_Tokens[m_TokenPos].type == Token::CC || m_Tokens[m_TokenPos].type == Token::N;
        };

        if (m_TokenPos >= m_Tokens.size())
            return false;

        Pos old = m_TokenPos;
        if (m_Tokens[m_TokenPos].type == Token::LBRACK)
        {
            m_TokenPos++;
            if (isValidCharToken())
            {
                m_TokenPos++;
                if (m_Tokens[m_TokenPos].type == Token::MINUS)
                {
                    m_TokenPos++;
                    if (isValidCharToken())
                    {
                        if (m_Tokens[old + 1].type != m_Tokens[m_TokenPos].type)
                        {
                            m_TokenPos = old;
                            return false;
                        }

                        m_TokenPos++;
                        if (m_Tokens[m_TokenPos].type == Token::RBRACK)
                        {
                            m_TokenPos++;
                            isOperator();

                            m_Op = new AstNodeRange({m_Tokens[old].startPos, m_Tokens[m_TokenPos].endPos}, m_Tokens[old + 1].c_value, m_Tokens[old + 3].c_value, m_OpType);
                            return true;
                        }
                    }
                }
            }
        }

        m_TokenPos = old;
        return false;
    }

    bool Syntax::isEscape()
    {
        if (m_TokenPos >= m_Tokens.size())
            return false;

        switch (m_Tokens[m_TokenPos].type)
        {
        case Token::CHAR:
            m_EscapeType = EscapeType::CHAR;
            break;
        case Token::BIG_CHAR:
            m_EscapeType = EscapeType::BIG_CHAR;
            break;
        case Token::ANY_CHAR:
            m_EscapeType = EscapeType::ANY_CHAR;
            break;
        case Token::DIGIT:
            m_EscapeType = EscapeType::DIGIT;
            break;
        case Token::NEWLINE:
            m_EscapeType = EscapeType::NEWLINE;
            break;
        default:
            return false;
        }

        m_TokenPos++;
        return true;
    }

    bool Syntax::isOperator()
    {
        if (m_TokenPos >= m_Tokens.size())
        {
            m_OpType = OpType::NONE;
            return false;
        }

        switch (m_Tokens[m_TokenPos].type)
        {
        case Token::PLUS:
            m_OpType = AstNodeOps::PLUS;
            break;
        case Token::ASTERIX:
            m_OpType = AstNodeOps::ASTERIX;
            break;
        case Token::QUESTION_MARK:
            m_OpType = AstNodeOps::QUESTION_MARK;
            break;
        default:
            m_OpType = OpType::NONE;
            return false;
        }

        m_TokenPos++;
        return true;
    }

    void Syntax::printAst()
    {
        for (auto &node : m_AstTree)
        {
            std::cout << node->toString() << std::endl;
        }
    }
};