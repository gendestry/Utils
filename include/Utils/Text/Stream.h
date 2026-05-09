#pragma once
#include <format>
#include <string>
#include <utility>

#include "String.h"

namespace Utils::Text {
    class Stream
    {
        std::stringstream ss;
    public:
        Stream& operator<<(auto s)
        {
            ss << s;
            return *this;
        }

        template<typename... Args>
        void add(Args&&... args)
        {
            ss << String::concat(std::forward<Args>(args)...);
        }

        template<typename... Args>
        void addFormatted(const std::string& format, Args&&... args)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            ss << msg;
        }

        std::string end()
        {
            std::string temp = ss.str();
            ss.clear();
            return temp;
        }
    };
}