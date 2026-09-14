#pragma once
#include <list>
#include <string>
#include <memory>
#include "Utils/Regex/Engine/Tokenizer.h"
#include "Utils/Regex/Engine/Syntax.h"
#include "Engine/AST/AST.h"
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

    public:
        unsigned int lastMaxLength = 0;

        Matcher(const std::string& pattern);
        Matcher(const Matcher &other);
        Matcher(Matcher &&other) noexcept;

        Matcher &operator=(const Matcher &other);
        Matcher &operator=(Matcher &&other) noexcept;

        [[nodiscard]] const std::string &getPattern() const;

        [[nodiscard]] bool match(const std::string &text) const;
        [[nodiscard]] std::optional<Engine::MatchInfo> matchInfo(const std::string &text) const;
        [[nodiscard]] std::optional<Engine::MatchInfo> matchGroups(const std::string &text) const;
        [[nodiscard]] std::optional<Engine::MatchInfo> matchGroupsInfo(const std::string &text) const;

        [[nodiscard]] std::optional<std::string> find(const std::string &text) const;
        [[nodiscard]] std::optional<Engine::MatchInfo> findInfo(const std::string &text) const;

        [[nodiscard]] std::optional<Engine::MatchInfo> findGroupsInfo(const std::string &text) const;
        [[nodiscard]] std::optional<std::list<Engine::MatchInfo>> findAllGroupsInfo(const std::string &text) const;

        [[nodiscard]] std::optional<std::list<std::string>> findAll(const std::string &text);
        [[nodiscard]] std::optional<std::list<Engine::MatchInfo>> findAllInfo(const std::string &text);

        void printTokens() const;
        void printAst() const;
        void prettyPrint() const;

    };
};