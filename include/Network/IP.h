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
        IP(std::string ip);

        uint8_t& operator[](uint8_t index);

        std::string str() const;
    };
}