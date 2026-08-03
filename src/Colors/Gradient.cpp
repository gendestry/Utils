#include "Utils/Colors/Gradient.h"
#include "Utils/Math/Curve.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

using namespace Utils::Colors;

Gradient::Gradient(uint16_t size, bool hardCut) : m_size(size), m_hardCutoff(hardCut)
{
    m_gradient.resize(size);
}

void Gradient::setSize(uint16_t size)
{
    m_size = size;
    calculateGradient();
}

void Gradient::normalize()
{
    for (auto &seg : m_segments)
    {
        seg.norm = seg.percentage / m_total;
    }
}

bool Gradient::pushSegment(const GradientSegment &seg)
{
    if (seg.percentage <= 0.f)
    {
        return false;
    }
    m_total += seg.percentage;
    m_segments.push_back(seg);
    return true;
}

// Ramps from color2 to color1 over `length` entries, eased by a quarter sine.
std::vector<RGB> Gradient::gradient(const RGB &color1, const RGB &color2, const uint16_t length)
{
    std::vector<RGB> ret;
    ret.resize(length);

    const std::unique_ptr<Maths::Curve> curve = std::make_unique<Maths::SinusoidHalf>(length);

    for (uint16_t i = 0; i < length; i++)
    {
        const float t = (*curve)[i];
        const uint8_t r = static_cast<uint8_t>(color1.r * t + color2.r * (1.f - t));
        const uint8_t g = static_cast<uint8_t>(color1.g * t + color2.g * (1.f - t));
        const uint8_t b = static_cast<uint8_t>(color1.b * t + color2.b * (1.f - t));

        ret[i] = {r, g, b};
    }

    return ret;
}

void Gradient::calculateGradient()
{
    m_gradient.clear();
    if (m_segments.empty())
        return;

    if (m_segments.size() == 1)
    {
        m_gradient.resize(m_size);
        const auto &gc = m_segments[0].color;
        for (auto &g : m_gradient)
        {
            g = gc;
        }
        return;
    }

    m_gradient.reserve(m_size);

    float acc = 0.f;
    uint16_t start = 0;
    uint16_t end = 0;
    const size_t size = m_segments.size();

    // A hard cutoff is not cyclic: the last segment is only a target colour, it owns no
    // pixels, so the remaining segments are renormalized to share the whole strip.
    const size_t count = m_hardCutoff ? size - 1 : size;
    float total = m_total;
    if (m_hardCutoff)
    {
        total -= m_segments.back().percentage;
    }

    if (total <= 0.f)
    {
        m_gradient.resize(m_size);
        return;
    }

    for (size_t i = 0; i < count; i++)
    {
        auto &seg = m_segments[i];
        acc += seg.percentage / total;
        end = static_cast<uint16_t>(acc * m_size);
        if (end <= start)
        {
            continue; // skip empty/rounded-away segments
        }

        const auto &c1 = seg.color;
        const auto &c2 = m_segments[(i + 1) % size].color;
        auto grad = gradient(c2, c1, end - start);
        m_gradient.insert(m_gradient.end(), grad.begin(), grad.end());
        start = end;
    }

    m_gradient.resize(m_size); // rounding can leave it a pixel short
}

void Gradient::addSegment(const GradientSegment &seg)
{
    if (!pushSegment(seg))
    {
        return;
    }

    normalize();
    calculateGradient();
}

void Gradient::addSegments(std::initializer_list<GradientSegment> segs)
{
    bool added = false;
    for (const auto &seg : segs)
    {
        added |= pushSegment(seg);
    }

    if (!added)
    {
        return;
    }

    normalize();
    calculateGradient();
}
