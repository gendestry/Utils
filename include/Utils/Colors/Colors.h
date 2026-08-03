#pragma once
// #include <cstdint>
// #include <vector>
#include "Utils/Colors/HSV.h"
#include "Utils/Colors/RGB.h"

namespace Utils::Colors
{
HSV rgbToHsv(const RGB &rgb);
RGB hsvToRgb(const HSV &hsv);

// Interpolate `from` -> `to`, with t clamped to [0, 1]: t = 0 yields `from`, t = 1 yields `to`.
// Component-wise in RGB space. Mixing complementary colours dips through grey.
RGB lerpInRGB(const RGB &from, const RGB &to, float t);
// Interpolates hue along the shorter way around the wheel, so mixing complementary colours
// stays saturated. If one endpoint is greyscale its (meaningless) hue is ignored.
RGB lerpInHSV(const RGB &from, const RGB &to, float t);

// std::vector<RGB> gradient(const RGB& color1, const RGB& color2, uint16_t length, bool half =
// false); std::vector<RGB> makeGradient(const std::vector<RGB>& colors, const std::vector<float>&
// weights, int N);

const RGB White(255, 255, 255);
const RGB Red(255, 0, 0);
const RGB Yellow(255, 100, 0);
const RGB Green(0, 255, 0);
const RGB Blue(0, 0, 255);
}; // namespace Utils::Colors
