#pragma once
#include "TerminalManipulation.h"
#include "Utils/Math/Rectangle.h"
#include <cstdint>
#include <iostream>
#include <string>

namespace Utils::Terminal::Helper
{
// A box-drawing frame one cell outside a rectangle, so whatever is drawn inside stays untouched.
//
// The frame needs a free row above and below and a free column left and right of the rectangle.
// Terminal rows and columns start at 1, so a rectangle at row or column 1 has no room for it.
struct Border
{
    enum class Style
    {
        Single,
        Rounded,
        Double,
        Heavy
    };

    Style style = Style::Rounded;

    void draw(TerminalManipulation& term, const Maths::Rectangle& rect) const
    {
        const Glyphs g = glyphs();
        const int inner = int(rect.width);
        const int left = int(rect.left()) - 1;
        const int right = int(rect.right());
        const int top = int(rect.top()) - 1;
        const int bottom = int(rect.bottom());

        term.moveCursorToPosition(uint16_t(top), uint16_t(left));
        std::cout << g.topLeft << repeat(g.horizontal, inner) << g.topRight;

        for (int row = top + 1; row < bottom; ++row)
        {
            term.moveCursorToPosition(uint16_t(row), uint16_t(left));
            std::cout << g.vertical;
            term.moveCursorToPosition(uint16_t(row), uint16_t(right));
            std::cout << g.vertical;
        }

        term.moveCursorToPosition(uint16_t(bottom), uint16_t(left));
        std::cout << g.botLeft << repeat(g.horizontal, inner) << g.botRight;
    }

  private:
    struct Glyphs
    {
        const char* topLeft;
        const char* topRight;
        const char* botLeft;
        const char* botRight;
        const char* horizontal;
        const char* vertical;
    };

    Glyphs glyphs() const
    {
        switch (style)
        {
        case Style::Single:
            return {"┌", "┐", "└", "┘", "─", "│"};
        case Style::Double:
            return {"╔", "╗", "╚", "╝", "═", "║"};
        case Style::Heavy:
            return {"┏", "┓", "┗", "┛", "━", "┃"};
        case Style::Rounded:
            break;
        }
        return {"╭", "╮", "╰", "╯", "─", "│"};
    }

    // The glyphs are several bytes each, so std::string(n, c) can't repeat them.
    static std::string repeat(const char* s, int n)
    {
        std::string out;
        for (int i = 0; i < n; ++i)
            out += s;
        return out;
    }
};
} // namespace Utils::Terminal::Helper
