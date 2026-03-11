#pragma once

#include <string>
#include <vector>

#include "token.h"

namespace Regex
{

    class Tokenizer
    {
        std::string m_Pattern;
        std::vector<Token> m_Tokens;

    public:
        Tokenizer(std::string pattern)
        {
            this->m_Pattern = pattern;
        }

        void tokenize(bool ignore_whitespace = true);
        void print_tokens();
        const std::vector<Token> &get_tokens()
        {
            return m_Tokens;
        }
    };
};