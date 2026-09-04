//
// Created by bobi on 11. 05. 26.
//

#pragma once
#include "AST.h"

namespace Utils::Regex::Engine {
    class AstNodeEscape : public AstNodeOps
    {
    public:
        enum EscapeType
        {
            CHAR,
            BIG_CHAR,
            ANY_CHAR,
            DIGIT,
            NEWLINE,
            ANY
        } m_EscapeType;

        std::string toEscapeString()
        {
            switch (m_EscapeType)
            {
                case CHAR:
                    return "\\c";
                case BIG_CHAR:
                    return "\\C";
                case ANY_CHAR:
                    return "\\T";
                case DIGIT:
                    return "\\d";
                case NEWLINE:
                    return "\\n";
                case ANY:
                    return "\\A";
                default:
                    return "\\X";
            }
        }

        AstNodeEscape(Location loc, EscapeType escapeType, OpType optype = OpType::NONE) : AstNodeOps(loc), m_EscapeType(escapeType)
        {
            m_OpType = optype;
        }

        unsigned int _match(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        // Match match(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        MatchInfo _match_info(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        // std::optional<MatchInfo> match_info(std::string text, unsigned int start, bool ignoreAllMathced = false) override;

        std::string toString() override;
        std::string toPrettyString() override;
    };
}