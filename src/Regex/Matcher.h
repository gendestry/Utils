#pragma once
#include <string>
#include <memory>
#include "Engine/Tokenizer.h"
#include "Engine/Syntax.h"
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
        std::unique_ptr<Engine::Tokenizer> m_Tokenizer;
        std::unique_ptr<Engine::Syntax> m_Syntax;

        struct MatchInfo {
            unsigned int start;
            std::string match;
        };

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
        std::optional<std::string> find(const std::string &text) const;
        std::optional<MatchInfo> findInfo(const std::string &text) const;

        std::optional<std::vector<std::string>> findAll(const std::string &text) const;
        std::optional<std::vector<MatchInfo>> findAllInfo(const std::string &text) const;

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