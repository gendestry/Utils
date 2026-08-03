//
// Created by bobi on 15. 03. 26.
//

#include "Utils/Colors/RGB.h"
#include "Utils/Colors/HSV.h"

#include <algorithm>
#include <cmath>
#include <format>

using namespace Utils::Colors;

RGB::RGB(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) { toPercentage(); }

RGB &RGB::toPercentage()
{
    pr = (static_cast<float>(r) / 255.0f) * 100.0f;
    pg = (static_cast<float>(g) / 255.0f) * 100.0f;
    pb = (static_cast<float>(b) / 255.0f) * 100.0f;

    return *this;
}

uint8_t &RGB::operator[](uint8_t index) { return rgb[index]; }

uint8_t RGB::operator[](uint8_t index) const { return rgb[index]; }

HSV RGB::toHSV() const
{
    // Normalize RGB to [0, 1]
    float r = rgb[0] / 255.0f;
    float g = rgb[1] / 255.0f;
    float b = rgb[2] / 255.0f;

    // Find min and max values of R, G, B
    float maxVal = std::max({r, g, b});
    float minVal = std::min({r, g, b});
    float delta = maxVal - minVal;

    // Compute Hue
    float h = 0.0f;
    if (delta > 0.00001f)
    { // Avoid division by zero
        if (maxVal == r)
        {
            h = 60.0f * (std::fmod(((g - b) / delta), 6.0f));
        }
        else if (maxVal == g)
        {
            h = 60.0f * (((b - r) / delta) + 2.0f);
        }
        else if (maxVal == b)
        {
            h = 60.0f * (((r - g) / delta) + 4.0f);
        }
    }
    if (h < 0.0f)
        h += 360.0f; // Ensure hue is positive

    // Compute Saturation
    float s = (maxVal > 0.00001f) ? (delta / maxVal) : 0.0f;

    // Compute Value
    float v = maxVal;

    return {h, s, v};
}

std::string RGB::toString() const { return std::format("R: {}, G: {}, B {}\n", r, g, b); }
