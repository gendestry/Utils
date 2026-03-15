#pragma once
#include <list>
#include <string>
#include <memory>
#include "Regex/Engine/Tokenizer.h"
#include "Regex/Engine/Syntax.h"
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
        std::string m_Pattern;
        std::unique_ptr<Engine::Tokenizer> m_Tokenizer;
        std::unique_ptr<Engine::Syntax> m_Syntax;


        struct MatchInfo {
            unsigned int start;
            std::string match;
        };

    public:
        unsigned int lastMaxLength = 0;

        // Regex() = delete;
        Matcher(const std::string& pattern);
        Matcher(const Matcher &other);
        Matcher(Matcher &&other) noexcept;

        Matcher &operator=(const Matcher &other);
        Matcher &operator=(Matcher &&other) noexcept;

        [[nodiscard]] const std::string &getPattern() const;

        [[nodiscard]] bool match(const std::string &text) const;
        [[nodiscard]] std::optional<std::string> find(const std::string &text) const;
        [[nodiscard]] std::optional<MatchInfo> findInfo(const std::string &text) const;

        [[nodiscard]] std::optional<std::list<std::string>> findAll(const std::string &text);
        [[nodiscard]] std::optional<std::list<MatchInfo>> findAllInfo(const std::string &text);

        void printTokens() const;
        void printAst() const;
        void prettyPrint() const;

    };
};