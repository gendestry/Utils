#include "Colors.h"
#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
#include <sstream>
#include <print>

#include "../Math/Curve.h"

namespace Utils::Colors
{
    HSV rgbToHsv(const RGB& rgb)
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
                h = 60.0f * (fmod(((g - b) / delta), 6.0f));
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

    RGB hsvToRgb(const HSV& hsv)
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
        return RGB(static_cast<uint8_t>(r * 255.0f),
                static_cast<uint8_t>(g * 255.0f),
                static_cast<uint8_t>(b * 255.0f));
    }

    std::vector<RGB> gradient(const RGB& color1, const RGB& color2, const uint16_t length, bool half) {
        std::vector<RGB> ret;
        std::unique_ptr<Maths::Curve> curve;
        if (half)
        {
            curve = std::make_unique<Maths::SinusoidHalf>(length);
        }
        else
        {
            curve = std::make_unique<Maths::Sinusoid>(length);
        }

        // const Curve::Sinusoid curve(length);
        for (int i = 0; i < length; i++)
        {
            float t  = (*curve)[i];
            const uint8_t r = static_cast<uint8_t>(color1.r * t + color2.r * (1.f - t));
            const uint8_t g = static_cast<uint8_t>(color1.g * t + color2.g * (1.f - t));
            const uint8_t b = static_cast<uint8_t>(color1.b * t + color2.b * (1.f - t));
            ret.push_back(RGB(r,g,b));
        }

        return ret;
    }

    std::vector<RGB> makeGradient(const std::vector<RGB>& colors, const std::vector<float>& weights, int N)
    {
        std::vector<RGB> ret;
        float sum = 0.0f;
        for (float w : weights)
        {
            sum += w;
        }

        if (sum <= 0)
        {
            throw std::invalid_argument("Invalid weights.");
        }

        float acc = 0.0f;
        int start = 0;
        int end = 0;
        for (size_t i = 0; i < weights.size(); i++)
        {
            acc += weights[i] / sum;
            end = N * acc;

            auto c1 = colors[i % colors.size()];
            auto c2 = colors[(i + 1) % colors.size()];
            auto grad = gradient(c2, c1, end - start, true);
            ret.insert(ret.end(), grad.begin(), grad.end());
            start = end;
        }

        return ret;
    }

};