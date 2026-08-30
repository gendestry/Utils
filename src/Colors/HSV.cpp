//
// Created by bobi on 15. 03. 26.
//

#include "Utils/Colors/HSV.h"
#include "Utils/Colors/RGB.h"

#include <cmath>
#include <format>

using namespace Utils::Colors;

HSV::HSV(float hue, float saturation, float value) : h(hue), s(saturation), v(value) {}

float &HSV::operator[](uint8_t index) { return hsv[index]; }

float HSV::operator[](uint8_t index) const { return hsv[index]; }

RGB HSV::toRGB() const
{
    float h = hsv[0]; // Hue in degrees
    float s = hsv[1]; // Saturation in [0, 1]
    float v = hsv[2]; // Value in [0, 1]

    h = std::fmod(h, 720.f);
    h = h > 360.f ? 720.f - h : h;

    float r = 0.0f, g = 0.0f, b = 0.0f;

    int hi = static_cast<int>(std::floor(h / 60.0f)) % 6; // Sector of the color wheel (0-5)
    float f = (h / 60.0f) - hi;                           // Fractional part of h/60
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    switch (hi)
    {
    case 0:
        r = v, g = t, b = p;
        break;
    case 1:
        r = q, g = v, b = p;
        break;
    case 2:
        r = p, g = v, b = t;
        break;
    case 3:
        r = p, g = q, b = v;
        break;
    case 4:
        r = t, g = p, b = v;
        break;
    case 5:
        r = v, g = p, b = q;
        break;
    }

    // Convert to 0-255 range
    return RGB(static_cast<uint8_t>(r * 255.0f + 0.5f), static_cast<uint8_t>(g * 255.0f + 0.5f),
               static_cast<uint8_t>(b * 255.0f + 0.5f));
}

std::string HSV::toString() const { return std::format("Hue: {}, Sat: {}, Val {}\n", h, s, v); }
