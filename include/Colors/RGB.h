//
// Created by bobi on 8. 03. 26.
//

#pragma once
#include <string>
#include <cstdint>

namespace Utils::Colors
{
struct RGB
{
    union
    {
        struct
        {
            uint8_t r = 0, g = 0, b = 0;
        };

        uint8_t rgb[3];
    };

    float pr = 0.f, pg = 0.f, pb = 0.f;

    RGB() = default;
    RGB(uint8_t red, uint8_t green, uint8_t blue);

    RGB& toPercentage();

    uint8_t& operator[](uint8_t index);
    uint8_t operator[](uint8_t index) const;

    [[nodiscard]] std::string toString() const;
};
}