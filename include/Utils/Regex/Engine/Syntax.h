#pragma once
#include <vector>
#include "Token.h"
#include "AST/AST.h"
#include "AST/AstNodeEscape.h"

namespace Utils::Regex::Engine
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
        bool m_Lazy = false;

        unsigned int range1 = 0, range2 = 0;

        // The operator flags live on Syntax while parsing; hand them to the node just built.
        void assignOp() {
            if (m_Op != nullptr)
                m_Op->m_Lazy = m_Lazy;
            assignRange();
        }

        void assignRange() {
            if (m_OpType == AstNodeOps::RANGE && m_Op != nullptr) {
                m_Op->m_Range.start = range1;
                m_Op->m_Range.end = range2;
                range1 = 0;
                range2 = 0;
            }
        }

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

        bool isCapture();
        bool isNotCapture();
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