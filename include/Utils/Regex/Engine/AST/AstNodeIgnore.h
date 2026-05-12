//
// Created by bobi on 11. 05. 26.
//

#pragma once
#include "AST.h"

namespace Utils::Regex::Engine {
    class AstNodeIgnore : public AstNodeOps
    {
    public:
        std::vector<AstNodeOps *> m_Ops;

        AstNodeIgnore(Location loc, std::vector<AstNodeOps *> ops, OpType optype = OpType::NONE) : AstNodeOps(loc), m_Ops(ops)
        {
            m_whatDo = IGNORE;
            m_OpType = optype;
        }

        ~AstNodeIgnore()
        {
            for (auto& op : m_Ops) {
                delete op;
            };
        }

        unsigned int _match(std::string text, unsigned int start, bool ignoreAllMathced = false) override;
        Match match(std::string text, unsigned int start, bool ignoreAllMathced = false) override;

        std::string toString() override;
        std::string toPrettyString() override;
    };
}