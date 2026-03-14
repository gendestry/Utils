//
// Created by bobi on 8. 03. 26.
//

#pragma once
#include <vector>
#include <cstdint>

namespace Utils::Maths
{
    std::vector<float> linearInterpolation(float start, float end, uint32_t n, bool mirror = false);
    std::vector<float> linearInterpolationMirrored(float start, float end, uint32_t n);
    std::vector<uint16_t> make_range(uint16_t start, uint16_t end);
    int getRandomInt(int min, int max);

}