#pragma once
#include <string>
#include <memory>
#include "Tokenizer.h"
#include "Syntax.h"
#include <optional>

#define DEBUG 0
#define PRINT(stmt) \
    if (DEBUG)      \
    {               \
        stmt        \
    }

namespace Utils::Regex
{
    class Matcher
    {
        bool m_Valid = true;
        unsigned int m_MaxMatch = 0;
        std::string m_Pattern;
        std::string m_Match;
        std::unique_ptr<Tokenizer> m_Tokenizer;
        std::unique_ptr<Syntax> m_Syntax;

    public:
        // Regex() = delete;
        Matcher(std::string pattern);
        Matcher(const Matcher &other);
        Matcher(Matcher &&other) noexcept;

        Matcher &operator=(const Matcher &other);
        Matcher &operator=(Matcher &&other) noexcept;

        void printTokens();
        void printAst();
        void prettyPrint();

        bool match(const std::string &text);
        std::optional<unsigned int> findFirst(const std::string &text);

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