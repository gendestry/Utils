//
// Created by bobi on 8. 03. 26.
//

#pragma once
#include <cstdint>
#include <string>

namespace Utils::Colors
{
struct RGB;
struct HSV
{
    union
    {
        struct
        {
            float h = 0.f, s = 0.f, v = 0.f;
        };
        float hsv[3];
    };

    HSV() = default;

    HSV(float hue, float saturation, float value);

    float &operator[](uint8_t index);
    float operator[](uint8_t index) const;

    [[nodiscard]] RGB toRGB() const;
    [[nodiscard]] std::string toString() const;
};
} // namespace Utils::Colors
