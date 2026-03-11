#pragma once
#include <vector>
#include "token.h"
#include "ast.h"

namespace Regex
{
    using TokenArray = std::vector<Token>;
    using Pattern = std::vector<AstNodeOps *>;
    using OpType = AstNodeOps::OpType;
    using EscapeType = AstNodeEscape::EscapeType;
    using Pos = unsigned int;

    class Syntax
    {
    private:
        // our ast tree or pattern
        Pattern m_AstTree;

        // keeping tract of tokens and positions
        Pos m_TokenPos = 0;
        const TokenArray &m_Tokens;

        // for parsing
        AstNodeOps *m_Op;
        OpType m_OpType = OpType::NONE;
        EscapeType m_EscapeType = EscapeType::CHAR;

    public:
        Syntax(const TokenArray &tokens) : m_Tokens(tokens)
        {
        }

        ~Syntax()
        {
            for (auto &node : m_AstTree)
            {
                delete node;
            }
        }

        bool parse();

        bool isInter();

        bool isParen();
        bool isEscapeOp();
        bool isTxtOp();
        bool isRangeOp();

        bool isEscape();
        bool isOperator();

        void printAst();

        inline Pattern &getPattern()
        {
            return m_AstTree;
        }
    };
};