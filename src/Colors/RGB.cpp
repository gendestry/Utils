//
// Created by bobi on 15. 03. 26.
//

#include "Colors/RGB.h"
#include <format>

using namespace Utils::Colors;

RGB::RGB(uint8_t red, uint8_t green, uint8_t blue)
        : r(red), g(green), b(blue)
{
    toPercentage();
}

RGB& RGB::toPercentage() {
    pr = (static_cast<float>(r) / 255.0f) * 100.0f;
    pg = (static_cast<float>(g) / 255.0f) * 100.0f;
    pb = (static_cast<float>(b) / 255.0f) * 100.0f;

    return *this;
}

uint8_t& RGB::operator[](uint8_t index)
{
    return rgb[index];
}

uint8_t RGB::operator[](uint8_t index) const
{
    return rgb[index];
}

std::string RGB::toString() const
{
    return std::format("R: {}, G: {}, B {}\n", r, g, b);
}