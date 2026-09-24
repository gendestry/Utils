#pragma once
#include "Utils/Math/Point.h"
#include "Utils/Math/Rectangle.h"

#include <algorithm>
#include <optional>

namespace Utils::Terminal::Claude
{
// Size.x is the width, Size.y the height.
using Maths::Point;
using Maths::Rectangle;
using Maths::Size;

enum class Axis
{
    Horizontal,
    Vertical
};

// Where a child sits on its parent's cross axis when it is smaller than the space it was given.
enum class Align
{
    Start,
    Center,
    End,
    Stretch
};

// Space a Frame keeps between its own edge and its child: one cell per side for a border,
// n for padding.
struct Insets
{
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;

    static constexpr Insets all(float n) { return {n, n, n, n}; }

    constexpr float horizontal() const { return left + right; }
    constexpr float vertical() const { return top + bottom; }

    constexpr Rectangle deflate(const Rectangle& r) const
    {
        return {{r.pos.x + left, r.pos.y + top},
                std::max(0.0f, r.width - horizontal()),
                std::max(0.0f, r.height - vertical())};
    }
};

// What a node wants. Written by the caller or by a decorator (flex, width, ...), never by the
// layout pass -- arrange() writes the node's rectangle, not this.
struct LayoutSpec
{
    // Fixed size on that axis; replaces measure() and makes the node ignore grow and shrink there.
    std::optional<float> width;
    std::optional<float> height;

    // Share of the space left over on the parent's main axis. flex sets it to 1.
    float grow = 0.0f;

    // Share of the overflow given back when the parent is too small, down to minSize.
    float shrink = 1.0f;

    Size minSize{0.0f, 0.0f};
};
} // namespace Utils::Terminal::Claude
