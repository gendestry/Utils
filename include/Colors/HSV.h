//
// Created by bobi on 8. 03. 26.
//

#pragma once
#include <string>
#include <cstdint>
#include <format>

namespace Utils::Colors
{
    struct HSV
    {
        union
        {
            struct
            {
                float h,s,v;
            };
            float hsv[3];
        };

        HSV() = default;

        HSV(float hue, float saturation, float value)
        {
            h = hue;
            s = saturation;
            v = value;
        }

        float& operator[](uint8_t index)
        {
            return hsv[index];
        }

        float operator[](uint8_t index) const
        {
            return hsv[index];
        }

        [[nodiscard]] std::string toString() const
        {
            return std::format("Hue: {}, Sat: {}, Val {}\n", h, s, v);
        }
    };
}