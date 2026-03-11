#pragma once
#include <cstdint>
#include <format>
#include <iosfwd>
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

        std::string end()
        {
            std::string temp = ss.str();
            ss.clear();
            return temp;
        }

        template<typename... Args>
        void addFormatted(std::string format, Args&&... args)
        {
            auto msg = std::vformat(format, std::make_format_args(args...));
            ss << msg;
        }

        static std::string pad(uint32_t n, std::string by)
        {
            Stream s;
            for(uint32_t i = 0; i < n; i++)
            {
                s << by;
            }

            return s.end();
        }
    };
}