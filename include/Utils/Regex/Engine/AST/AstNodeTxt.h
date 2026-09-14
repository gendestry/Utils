//
// Created by bobi on 11. 05. 26.
//

#pragma once
#include "AST.h"

namespace Utils::Regex::Engine {
    class AstNodeTxt : public AstNodeOps
    {
    public:
        std::string txt;
        AstNodeTxt(Location loc, std::string txt, OpType optype = OpType::NONE) : AstNodeOps(loc), txt(txt)
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