//
// Created by bobi on 16. 03. 26.
//

#pragma once
#include <format>
#include <print>
#include <string>
#include "Colors/Font.h"
#include <cstdint>

namespace Utils
{
    class Logger
    {
    public:
        enum Level : uint8_t
        {
            NOTSET      = 0U,
            DEBUGGING   = 1U,
            INFO        = 2U,
            ERROR       = 3U,
        };
    private:
        static uint32_t scopeSize;
        static Level s_level;

        uint32_t m_padOffset = 0;
        Level m_level = NOTSET;
        std::string m_scope;

        mutable std::string usedScope;

        std::string getScopePadding() const;
        std::string scopePadding() const;

    public:
        explicit Logger(std::string scope);
        explicit Logger(Logger& other, std::string scope);

        static void setLevel(Level level);
        void setLoggerLevel(Level level);

        void incPadOffset();
        void decPadOffset();

        void toggleScope();
        std::string scopeStr() const;

        void print(const std::string& text) const;
        void println(const std::string& text) const;

        template<typename... Args>
        void print(const std::string& format, Args&&... args)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            std::print("{}{}", scopePadding(), msg);
        }

        template<typename... Args>
        void println(const std::string& format, Args&&... args)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            std::println("{}{}", scopePadding(), msg);
        }

        template<typename... Args>
        void printColor(const std::string& color,
                        const std::string& format,
                        Args&&... args)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            std::print("{}{}{}{}", scopePadding(), color, msg, Font::colorReset);
        }

        template<typename... Args>
        void printlnColor(const std::string& color,
                          const std::string& format,
                          Args&&... args)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            std::println("{}{}{}{}", scopePadding(), color, msg, Font::colorReset);
        }

        template<typename... Args>
        void debug(const std::string& format,
                    Args&&... args)
        {
            const Level level = m_level == NOTSET ? s_level : m_level;
            if(level <= Level::DEBUGGING)
            {
                auto msg = std::vformat(format, std::make_format_args(args...));
                std::println("{}{}{}{}{}", Font::colorItalic, Font::colorByRGB(140,140,140), scopePadding(), msg, Font::colorReset);
            }
        }

        template<typename... Args>
        void error(const std::string& format,
                    Args&&... args)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            std::println("{}{}{}{}", Font::colorRed, scopePadding(), msg, Font::colorReset);
        }
    };
}

