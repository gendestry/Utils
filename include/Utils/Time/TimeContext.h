#pragma once
#include <cmath>
#include <cstdint>

namespace Utils::Time
{
struct TimeContext
{
    double now = 0.0;   // seconds since engine start
    float dt = 0.f;     // seconds since last frame
    uint64_t frame = 0; // monotonic frame counter

    // Sawtooth phase in [0, 1) cycling at `hz` cycles per second. Effects
    // (fades, chases) animate off this so they stay frame-rate independent.
    [[nodiscard]] float phase(float hz) const
    {
        if (hz <= 0.f)
            return 0.f;
        const double p = now * static_cast<double>(hz);
        return static_cast<float>(p - std::floor(p));
    }
};
} // namespace Utils::Time
