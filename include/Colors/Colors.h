#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Colors/RGB.h"
#include "HSV.h"


namespace Utils::Colors
{
    HSV rgbToHsv(const RGB& rgb);
    RGB hsvToRgb(const HSV& hsv);

    std::vector<RGB> gradient(const RGB& color1, const RGB& color2, uint16_t length, bool half = false);
    std::vector<RGB> makeGradient(const std::vector<RGB>& colors, const std::vector<float>& weights, int N);

    const RGB White(255, 255, 255);
    const RGB Red(255, 0, 0);
    const RGB Yellow(255, 100, 0);
    const RGB Green(0, 255, 0);
    const RGB Blue(0, 0, 255);
};
