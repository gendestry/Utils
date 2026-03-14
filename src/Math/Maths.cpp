//
// Created by bobi on 8. 03. 26.
//

#include "../../include/Math/Maths.h"
#include <algorithm>
#include <ranges>
#include <random>

namespace Utils::Maths
{

std::vector<float> linearInterpolation(float start, float end, uint32_t n, bool mirror)
{
    if (n <= 0)
        return {};

    // If mirrored, we only generate half and reflect
    const uint32_t baseCount = mirror ? (n + 1) / 2 : n;

    std::vector<float> base(baseCount);

    const float step = (end - start) / (static_cast<float>(baseCount) - 1.f);
    for (int i = 0; i < baseCount; i++)
    {
        base[i] = start + i * step;
    }

    if (!mirror)
        return base;

    std::vector<float> result;
    result.reserve(n);

    // Forward
    for (float v : base)
        result.push_back(v);

    // Backward (skip first and last to avoid duplicates)
    for (int i = base.size() - 2; i > 0; i--)
        result.push_back(base[i]);

    return result;
}

std::vector<float> linearInterpolationMirrored(float start, float end, uint32_t n) {
    bool odd = n % 2 == 1;
    int num = odd ? (n / 2 + 1) : (n / 2);
    auto lin = Maths::linearInterpolation(start, end, num);
    auto ret = lin;
    if (odd)
    {
        ret.erase(ret.end());
    }

    std::reverse(lin.begin(), lin.end());
    ret.insert(ret.end(), lin.begin(), lin.end());
    return ret;
}


std::vector<uint16_t> make_range(uint16_t start, uint16_t end)
{
    auto view = std::ranges::iota_view(start, end); // [start, end)
    return {view.begin(), view.end()};
};

int getRandomInt(int min, int max) {
    std::random_device seed;
    std::mt19937 gen{seed()}; // seed the generator
    std::uniform_int_distribution<> dist{min, max}; // set min and max
    return dist(gen); // generate number
}

}
