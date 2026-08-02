#pragma once
#include "Utils/Colors/RGB.h"
#include <cstdint>
#include <initializer_list>
#include <vector>

namespace Utils::Colors
{
struct GradientSegment
{
    RGB color;
    float percentage = 100.f;
    float norm = 100.f;
};

class Gradient
{
    std::vector<GradientSegment> m_segments;
    float m_total = 0.f;
    uint16_t m_size = 1;
    bool m_hardCutoff;

    std::vector<RGB> m_gradient;

    void normalize();
    bool pushSegment(const GradientSegment &seg);
    std::vector<RGB> gradient(const RGB &color1, const RGB &color2, uint16_t length, bool half);
    void calculateGradient();

  public:
    explicit Gradient(uint16_t size, bool hardCut = true);

    void setSize(uint16_t size);
    void addSegment(const GradientSegment &seg);
    void addSegments(std::initializer_list<GradientSegment> segs);

    [[nodiscard]] const std::vector<RGB> &get() const { return m_gradient; }
};
} // namespace Utils::Colors
