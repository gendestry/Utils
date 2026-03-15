//
// Created by bobi on 15. 03. 26.
//

#include "Colors/HSV.h"
#include <format>

using namespace Utils::Colors;

HSV::HSV(float hue, float saturation, float value)
    : h(hue), s(saturation), v(value)
{
}

float& HSV::operator[](uint8_t index)
{
    return hsv[index];
}

float HSV::operator[](uint8_t index) const
{
    return hsv[index];
}

std::string HSV::toString() const
{
    return std::format("Hue: {}, Sat: {}, Val {}\n", h, s, v);
}