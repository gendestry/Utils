//
// Created by bobi on 8. 03. 26.
//

#pragma once
#include <string>
#include "Colors/RGB.h"

namespace Utils::Text::Font
{
    constexpr std::string colorGreen = "\x1B[32m";
    constexpr std::string colorYellow = "\x1B[33m";
    constexpr std::string colorBlue = "\x1B[34m";
    constexpr std::string colorReset = "\x1B[0m";
    constexpr std::string colorDim = "\x1B[2m";
    constexpr std::string colorItalic = "\x1B[3m";

    std::string colorByRGB(uint8_t r, uint8_t g, uint8_t b, bool fg);
    std::string colorByRGB(const Colors::RGB& color, bool fg);
}