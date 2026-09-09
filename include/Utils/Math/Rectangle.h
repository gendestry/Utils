//
// Created by bobi on 9. 9. 26.
//

#pragma once
#include "Point.h"
#include <algorithm>

namespace Utils::Maths
{
struct Rectangle
{
    Point pos;
    float width = 0.0f;
    float height = 0.0f;

    constexpr Rectangle() = default;
    constexpr Rectangle(Point pos, float width, float height) : pos(pos), width(width), height(height) {}
    constexpr Rectangle(float x, float y, float width, float height) : pos(x, y), width(width), height(height) {}

    static constexpr Rectangle fromBounds(Point topLeft, Point botRight)
    {
        return {topLeft, botRight.x - topLeft.x, botRight.y - topLeft.y};
    }

    static constexpr Rectangle fromCenter(Point center, float width, float height)
    {
        return {{center.x - width / 2, center.y - height / 2}, width, height};
    }

    constexpr float left() const { return pos.x; }
    constexpr float top() const { return pos.y; }
    constexpr float right() const { return pos.x + width; }
    constexpr float bottom() const { return pos.y + height; }

    constexpr Point topLeft() const { return pos; }
    constexpr Point botRight() const { return {right(), bottom()}; }
    constexpr Point center() const { return {pos.x + width / 2, pos.y + height / 2}; }
    constexpr Point size() const { return {width, height}; }

    constexpr float area() const { return width * height; }
    constexpr bool empty() const { return width <= 0.0f || height <= 0.0f; }

    constexpr bool contains(Point p) const
    {
        return p.x >= left() && p.x < right() && p.y >= top() && p.y < bottom();
    }

    constexpr bool contains(const Rectangle &o) const
    {
        return o.left() >= left() && o.right() <= right() && o.top() >= top() && o.bottom() <= bottom();
    }

    constexpr bool intersects(const Rectangle &o) const
    {
        return left() < o.right() && o.left() < right() && top() < o.bottom() && o.top() < bottom();
    }

    constexpr Rectangle intersection(const Rectangle &o) const
    {
        const Point tl{std::max(left(), o.left()), std::max(top(), o.top())};
        const Point br{std::min(right(), o.right()), std::min(bottom(), o.bottom())};
        if (br.x <= tl.x || br.y <= tl.y)
            return {};
        return fromBounds(tl, br);
    }

    constexpr Rectangle united(const Rectangle &o) const
    {
        if (empty())
            return o;
        if (o.empty())
            return *this;
        return fromBounds({std::min(left(), o.left()), std::min(top(), o.top())},
                          {std::max(right(), o.right()), std::max(bottom(), o.bottom())});
    }

    constexpr Rectangle translated(Point delta) const { return {pos + delta, width, height}; }

    constexpr Rectangle inset(float amount) const
    {
        return {{pos.x + amount, pos.y + amount}, std::max(0.0f, width - 2 * amount),
                std::max(0.0f, height - 2 * amount)};
    }

    constexpr bool operator==(const Rectangle &) const = default;
};
} // namespace Utils::Maths
