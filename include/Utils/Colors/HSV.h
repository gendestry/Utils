//
// Created by bobi on 8. 03. 26.
//

#pragma once
#include <string>
#include <cstdint>

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

        HSV(float hue, float saturation, float value);

        float& operator[](uint8_t index);
        float operator[](uint8_t index) const;

        [[nodiscard]] std::string toString() const;
    };
}