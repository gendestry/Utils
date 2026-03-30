#pragma once
#include <cstdint>
#include <string>

namespace Utils::Network
{
    struct IP
    {
        union {
            struct {
                uint8_t a,b,c,d;
            };

            uint8_t bytes[4] = {0, 0, 0, 0};
        };

        IP() = default;
        IP(const IP &other);
        explicit IP(const std::string& ip);

        IP& operator=(const std::string& ip);

        void setIP(const std::string& ip);

        [[nodiscard]] uint8_t& operator[](uint8_t index);

        [[nodiscard]] std::string str() const;
        [[nodiscard]] const char* cstr() const { return str().c_str(); }; // ne dela
    };
}