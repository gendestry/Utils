#include "Utils/Colors/Colors.h"
#include "Utils/Colors/HSV.h"
#include "Utils/Colors/RGB.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Utils::Colors
{
namespace
{
float mix(const float a, const float b, const float t) { return a + (b - a) * t; }
} // namespace

HSV rgbToHsv(const RGB &rgb) { return rgb.toHSV(); }

RGB hsvToRgb(const HSV &hsv) { return hsv.toRGB(); }

RGB lerpInRGB(const RGB &from, const RGB &to, float t)
{
    t = std::clamp(t, 0.f, 1.f);

    // +0.5f so the result rounds instead of truncating towards `from`.
    return {static_cast<uint8_t>(mix(from.r, to.r, t) + 0.5f),
            static_cast<uint8_t>(mix(from.g, to.g, t) + 0.5f),
            static_cast<uint8_t>(mix(from.b, to.b, t) + 0.5f)};
}

RGB lerpInHSV(const RGB &from, const RGB &to, float t)
{
    t = std::clamp(t, 0.f, 1.f);

    const HSV a = from.toHSV();
    const HSV b = to.toHSV();

    // A greyscale endpoint has no meaningful hue, so borrow the other's and only fade saturation.
    float ha = a.h;
    float hb = b.h;
    if (a.s <= 0.00001f)
    {
        ha = hb;
    }
    else if (b.s <= 0.00001f)
    {
        hb = ha;
    }

    // Take the shorter arc around the wheel: a 350 -> 10 mix passes through red, not through cyan.
    float delta = hb - ha;
    if (delta > 180.f)
    {
        delta -= 360.f;
    }
    else if (delta < -180.f)
    {
        delta += 360.f;
    }

    float h = std::fmod(ha + delta * t, 360.f);
    if (h < 0.f)
    {
        h += 360.f;
    }

    return HSV(h, mix(a.s, b.s, t), mix(a.v, b.v, t)).toRGB();
}

}; // namespace Utils::Colors
