//
// Created by bobi on 11. 05. 26.
//

#pragma once
#include "AST.h"

namespace Utils::Regex::Engine {
    class AstNodeParen : public AstNodeOps
    {
    public:
        std::vector<std::vector<AstNodeOps *>> m_Ops;

        AstNodeParen(Location loc, std::vector<std::vector<AstNodeOps *>> ops, OpType optype = OpType::NONE) : AstNodeOps(loc), m_Ops(ops)
        {
            m_OpType = optype;
        }

        ~AstNodeParen()
        {
            for (auto &op : m_Ops)
            {
                for (auto &o : op)
                {
                    delete o;
                }
            }
        }

        unsigned int _match(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        // Match match(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        MatchInfo _match_info(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        // std::optional<MatchInfo> match_info(std::string text, unsigned int start, bool ignoreAllMathced = false) override;

        std::string toString() override;
        std::string toPrettyString() override;
    };
}