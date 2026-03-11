#pragma once
#include <sstream>
#include <format>
#include <print>

namespace Utils::Text
{
    // class String
    // {
        void concat(std::stringstream& ss) { }
        
        template<typename T, typename... Args> 
        void concat(std::stringstream& ss, T&& value, Args&&... args)
        { 
            ss << std::forward<T>(value); 
            concat(ss, std::forward<Args>(args)...); 
        } 
    // public:
        template<typename... Args> 
        std::string concat(Args&&... args)
        { 
            std::stringstream ss; 
            concat(ss, std::forward<Args>(args)...); 
            return ss.str(); 
        } 

        template<typename... Args>
        std::string format(const std::string& format, Args&&... args)
        {
            return std::format(format, std::forward<Args>(args)...);
        }
    // };


}
