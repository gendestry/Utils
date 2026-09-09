//
// Created by bobi on 9. 9. 26.
//

#pragma once

namespace Utils::Maths
{
struct Point
{
    float x = 0.0f;
    float y = 0.0f;

    constexpr Point() = default;
    constexpr Point(float x, float y) : x(x), y(y) {}

    constexpr Point operator+(const Point &o) const { return {x + o.x, y + o.y}; }
    constexpr Point operator-(const Point &o) const { return {x - o.x, y - o.y}; }
    constexpr Point operator*(float s) const { return {x * s, y * s}; }

    constexpr Point &operator+=(const Point &o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }

    constexpr Point &operator-=(const Point &o)
    {
        x -= o.x;
        y -= o.y;
        return *this;
    }

    constexpr bool operator==(const Point &) const = default;
};
}