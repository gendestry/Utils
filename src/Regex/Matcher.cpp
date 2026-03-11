#include <iostream>
#include "Matcher.h"
#include "Text/Font.h"

using namespace Utils;

namespace Utils::Regex
{
    Matcher::Matcher(std::string pattern) : m_Pattern(pattern)
    {
        m_Tokenizer = std::make_unique<Tokenizer>(pattern);
        m_Tokenizer->tokenize();
        m_Tokenizer->print_tokens();

        m_Syntax = std::make_unique<Syntax>(m_Tokenizer->get_tokens());
        m_Valid = m_Syntax->parse();

        if (!m_Valid)
        {
            std::cout << "Error: invalid regex syntax" << std::endl;
        }
    }

    Matcher::Matcher(const Matcher &other)
        : m_Pattern(other.m_Pattern),
          m_Valid(other.m_Valid),
          m_Tokenizer(std::make_unique<Tokenizer>(*other.m_Tokenizer)),
          m_Syntax(std::make_unique<Syntax>(*other.m_Syntax))
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
            m_Tokenizer = std::make_unique<Tokenizer>(*other.m_Tokenizer);
            m_Syntax = std::make_unique<Syntax>(*other.m_Syntax);
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
        Pattern &pattern = m_Syntax->getPattern();
        unsigned int start = 0;

        for (Pos i = 0; i < pattern.size(); i++)
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
        return start;
    }

    std::optional<unsigned int> Matcher::findFirst(const std::string &text)
    {
        if (!m_Valid)
            return {};

        m_Match = "";
        Pattern &pattern = m_Syntax->getPattern();
        unsigned int start = 0;

        std::string ctext = std::string(text);
        unsigned int subs = 0;
        Pos i = 0;
        for (; i < pattern.size(); i++)
        {
            PRINT(std::cout << "\n   Matching: " << pattern[i]->toPrettyString() << " => ";)

            auto [matched, current] = pattern[i]->match(ctext, start);
            if (matched)
            {
                std::string matchedText = ctext.substr(start, current - start);
                PRINT(std::cout << "Matched: '" << matchedText << "' ";)
                m_Match += matchedText;
                m_MaxMatch = std::max(m_MaxMatch, current);
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
        return subs;
    }
};