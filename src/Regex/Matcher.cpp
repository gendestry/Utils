#include <iostream>
#include "Matcher.h"
#include "Text/Font.h"

using namespace Utils;

namespace Utils::Regex
{
    Matcher::Matcher(std::string pattern) : m_Pattern(pattern)
    {
        m_Tokenizer = std::make_unique<Engine::Tokenizer>(pattern);
        m_Tokenizer->tokenize();
        m_Tokenizer->print_tokens();

        m_Syntax = std::make_unique<Engine::Syntax>(m_Tokenizer->get_tokens());
        m_Valid = m_Syntax->parse();

        if (!m_Valid)
        {
            std::cout << "Error: invalid regex syntax" << std::endl;
        }
    }

    Matcher::Matcher(const Matcher &other)
        : m_Pattern(other.m_Pattern),
          m_Valid(other.m_Valid),
          m_Tokenizer(std::make_unique<Engine::Tokenizer>(*other.m_Tokenizer)),
          m_Syntax(std::make_unique<Engine::Syntax>(*other.m_Syntax))
    {
    }

    Matcher::Matcher(Matcher &&other) noexcept
        : m_Pattern(std::move(other.m_Pattern)),
          m_Tokenizer(std::move(other.m_Tokenizer)),
          m_Syntax(std::move(other.m_Syntax)),
          m_Valid(other.m_Valid)
    {
    }

    // operators
    Matcher &Matcher::operator=(const Matcher &other)
    {
        if (this != &other)
        {
            m_Pattern = other.m_Pattern;
            m_Valid = other.m_Valid;
            m_Tokenizer = std::make_unique<Engine::Tokenizer>(*other.m_Tokenizer);
            m_Syntax = std::make_unique<Engine::Syntax>(*other.m_Syntax);
        }

        return *this;
    }

    Matcher &Matcher::operator=(Matcher &&other) noexcept
    {
        if (this != &other)
        {
            m_Pattern = std::move(other.m_Pattern);
            m_Tokenizer = std::move(other.m_Tokenizer);
            m_Syntax = std::move(other.m_Syntax);
            m_Valid = other.m_Valid;
        }

        return *this;
    }

    void Matcher::printTokens()
    {
        m_Tokenizer->print_tokens();
    }

    void Matcher::printAst()
    {
        if (m_Valid)
            m_Syntax->printAst();
    }

    void Matcher::prettyPrint()
    {
        if (m_Valid)
        {
            for (auto &p : m_Syntax->getPattern())
            {
                std::cout << p->toPrettyString();
            }
            std::cout << std::endl;
        }
    }

    bool Matcher::match(const std::string &text)
    {
        if (!m_Valid)
            return false;

        m_Match = "";
        Engine::Pattern &pattern = m_Syntax->getPattern();
        unsigned int start = 0;

        for (Engine::Pos i = 0; i < pattern.size(); i++)
        {
            PRINT(std::cout << "\n   Matching: " << pattern[i]->toPrettyString() << " => ";)

            auto [matched, current] = pattern[i]->match(text, start);
            if (matched)
            {
                std::string matchedText = text.substr(start, current - start);
                PRINT(std::cout << "Matched: '" << matchedText << "' ";)
                m_Match += matchedText;
                m_MaxMatch = std::max(m_MaxMatch, current);
            }
            else
            {
                PRINT(std::cout << "Not matched" << std::endl;)
                return false;
            }
            start = current;
        }

        // m_MaxMatch = start;
        // return start;
        return true;
    }

    std::optional<std::string> Matcher::find(const std::string &text) const {
        auto info = findInfo(text);
        if (info.has_value()) {
            return info.value().match;
        }

        return {};
    }

    std::optional<Matcher::MatchInfo> Matcher::findInfo(const std::string &text) const {
        if (!m_Valid)
            return {};

        std::string match;

        Engine::Pattern &pattern = m_Syntax->getPattern();
        unsigned int start = 0;

        std::string ctext = std::string(text);
        unsigned int subs = 0;
        Engine::Pos i = 0;
        for (; i < pattern.size(); i++)
        {
            PRINT(std::cout << "\n   Matching: " << pattern[i]->toPrettyString() << " => ";)

            auto [matched, current] = pattern[i]->match(ctext, start);
            if (matched)
            {
                std::string matchedText = ctext.substr(start, current - start);
                PRINT(std::cout << "Matched: '" << matchedText << "' ";)
                match += matchedText;
            }
            else
            {
                PRINT(std::cout << "Not matched" << std::endl;)
                if (ctext.size() == 1) {
                    break;
                }
                ctext = ctext.substr(1);
                subs++;
                i--;
            }
            start = current;
        }

        if (i < pattern.size()) {
            return {};
        }
        // m_MaxMatch = start;
        return MatchInfo{subs, match};
    }


    std::optional<std::vector<std::string>> Matcher::findAll(const std::string &text) const {
        auto info = findAllInfo(text);

        if (info.has_value()) {
            std::vector<std::string> matches;
            for (const auto& v : info.value()) {
                matches.push_back(v.match);
            }
            return matches;
        }

        return {};
    }

    std::optional<std::vector<Matcher::MatchInfo>> Matcher::findAllInfo(const std::string &text) const {
        if (!m_Valid)
            return {};

        std::vector<MatchInfo> matches;
        std::string ctext = std::string(text);

        unsigned int acc = 0;
        while (true) {
            std::optional<MatchInfo> match = findInfo(ctext);
            if (match.has_value()) {
                auto value = match.value();
                acc += value.start;
                value.start = acc;
                matches.push_back(value);
                acc += value.match.size();

                ctext = text.substr(acc);
                if (ctext.empty()) {
                    break;
                }
            }
            else {
                break;
            }
        }

        if (!matches.empty()) {
            return matches;
        }

        return {};
    }
};