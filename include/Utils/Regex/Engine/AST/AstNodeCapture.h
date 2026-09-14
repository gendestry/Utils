//
// Created by bobi on 11. 05. 26.
//

#pragma once
#include "AST.h"

namespace Utils::Regex::Engine {
    class AstNodeCapture : public AstNodeOps
    {
    public:
        std::vector<AstNodeOps *> m_Ops;

        AstNodeCapture(Location loc, std::vector<AstNodeOps *> ops, OpType optype = OpType::NONE) : AstNodeOps(loc), m_Ops(ops)
        {
            m_whatDo = CAPTURE;
            m_OpType = optype;
        }

        ~AstNodeCapture()
        {
            for (auto& op : m_Ops) {
                delete op;
            };
        }

        unsigned int _match(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        MatchInfo _match_info(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        std::vector<MatchInfo> match_info_candidates(std::string text, unsigned int start, bool ignoreAllMatched) override;

        std::string toString() override;
        std::string toPrettyString() override;
    };

}