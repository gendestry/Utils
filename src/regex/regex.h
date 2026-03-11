#pragma once
#include <string>
#include <memory>
#include "tokenizer.h"
#include "syntax.h"

#define DEBUG 0
#define PRINT(stmt) \
    if (DEBUG)      \
    {               \
        stmt        \
    }

namespace Regex
{
    class Regex
    {
        bool m_Valid = true;
        unsigned int m_MaxMatch = 0;
        std::string m_Pattern;
        std::string m_Match;
        std::unique_ptr<Tokenizer> m_Tokenizer;
        std::unique_ptr<Syntax> m_Syntax;

    public:
        // Regex() = delete;
        Regex(std::string pattern);
        Regex(const Regex &other);
        Regex(Regex &&other) noexcept;

        Regex &operator=(const Regex &other);
        Regex &operator=(Regex &&other) noexcept;

        void printTokens();
        void printAst();
        void prettyPrint();

        bool match(const std::string &text);

        inline unsigned int getMaxMatch() const
        {
            return m_Match.size();
        }

        inline const std::string &getPattern() const
        {
            return m_Pattern;
        }

        inline const std::string &getMatch() const
        {
            return m_Match;
        }
    };
};