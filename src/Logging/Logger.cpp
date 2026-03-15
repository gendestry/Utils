//
// Created by bobi on 16. 03. 26.
//

#include "Logging/Logger.h"
#include "Text/Stream.h"

namespace Utils
{
    uint32_t Logger::scopeSize = 0;
    Logger::Level Logger::s_level = Level::ERROR;

    Logger::Logger(std::string scope)
        : m_scope(std::move(scope))
    {
        if(scope.size() > scopeSize)
        {
            scopeSize = scope.size();
        }
        toggleScope();
    }

    Logger::Logger(Logger& other, std::string scope)
    {
        m_scope = other.m_scope + scope;
        if(scope.size() > scopeSize)
        {
            scopeSize = scope.size();
        }
        toggleScope();
    }

    void Logger::setLevel(Level level)
    {
        s_level = level;
    }

    void Logger::setLoggerLevel(Level level)
    {
        m_level = level;
    }

    void Logger::incPadOffset()
    {
        m_padOffset++;
    }

    void Logger::decPadOffset()
    {
        m_padOffset--;
    }

    std::string Logger::scopePadding() const
    {
        return usedScope + String::pad(m_padOffset, "  ");
    }



    std::string Logger::getScopePadding() const
    {
        // Utils::Stream s;
        // for(uint32_t i = 0; i < (9 - m_scope.size()); i++){
        //     s<< " ";
        // }

        // return s.end();
        return "";
    }

    void Logger::toggleScope()
    {
        if(usedScope.empty())
        {
            usedScope = Utils::String::concat("[", m_scope, getScopePadding(), "] ");
        }
        else
        {
            usedScope.clear();
        }
    }

    std::string Logger::scopeStr() const
    {
        return "[" + m_scope + getScopePadding() + "] ";
    }
}