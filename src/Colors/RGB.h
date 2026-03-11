//
// Created by bobi on 8. 03. 26.
//

#pragma once
#include <string>
#include <cstdint>
#include <format>

namespace Utils::Colors
{
struct RGB
{
    union
    {
        struct
        {
            uint8_t r,g,b;
        };

        uint8_t rgb[3];
    };

    float pr = 0.f, pg = 0.f, pb = 0.f;

    RGB() = default;

    RGB(uint8_t red, uint8_t green, uint8_t blue)
        : r(red), g(green), b(blue)
    {
        // r = red;
        // g = green;
        // b = blue;

        toPercentage();
    }

    // explicit RGB(float red, float green, float blue)
    // {
    //     r = static_cast<uint8_t>(red * 255.f);
    //     g = static_cast<uint8_t>(green * 255.f);
    //     b = static_cast<uint8_t>(blue * 255.f);
    //
    //     toPercentage();
    // }

    RGB& toPercentage() {
        pr = (static_cast<float>(r) / 255.0f) * 100.0f;
        pg = (static_cast<float>(g) / 255.0f) * 100.0f;
        pb = (static_cast<float>(b) / 255.0f) * 100.0f;

        return *this;
    }

    uint8_t& operator[](uint8_t index)
    {
        return rgb[index];
    }

    uint8_t operator[](uint8_t index) const
    {
        return rgb[index];
    }

    [[nodiscard]] std::string toString() const
    {
        return std::format("R: {}, G: {}, B {}\n", r, g, b);
    }
};
}