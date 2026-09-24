#pragma once
#include "Node.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

namespace Utils::Terminal::Claude
{
enum class Stroke
{
    Single,
    Rounded,
    Double,
    Heavy
};

// One child, with space kept around it. Border and padding are the same node: a border is
// insets of 1 plus a stroke drawn on those cells, padding is insets with no stroke.
//
// The invariant that keeps nesting correct: whatever measure() adds, arrange() takes away.
// The stroke is drawn ON the frame's own rectangle, never outside it, so two bordered siblings
// can sit side by side without overwriting each other.
struct Frame : Node
{
    std::unique_ptr<Node> child;
    Insets insets;
    std::optional<Stroke> stroke;

    Frame(std::unique_ptr<Node> content, Insets insets, std::optional<Stroke> stroke = std::nullopt)
        : child(std::move(content)), insets(insets), stroke(stroke)
    {
        if (child)
            child->parent = this;
    }

    Size measure() const override
    {
        const Size c = child ? child->desired() : Size{};
        return {c.x + insets.horizontal(), c.y + insets.vertical()};
    }

    // Never smaller than the insets, so a squeezed border stays a border instead of turning into
    // stray glyphs.
    Size minimum() const override
    {
        const Size c = child ? child->minimum() : Size{};
        return {std::max(layout.minSize.x, c.x + insets.horizontal()),
                std::max(layout.minSize.y, c.y + insets.vertical())};
    }

    void arrange(const Rectangle& r) override
    {
        Node::arrange(r);
        if (child)
            child->arrange(insets.deflate(r));
    }

    void render(Helper::TerminalManipulation& term) override
    {
        if (stroke)
            drawStroke(term);
        if (child)
            child->render(term);
    }

    void forEachChild(const std::function<void(Node&)>& fn) override
    {
        if (child)
            fn(*child);
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
        switch (*stroke)
        {
        case Stroke::Single:
            return {"┌", "┐", "└", "┘", "─", "│"};
        case Stroke::Double:
            return {"╔", "╗", "╚", "╝", "═", "║"};
        case Stroke::Heavy:
            return {"┏", "┓", "┗", "┛", "━", "┃"};
        case Stroke::Rounded:
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

    void drawStroke(Helper::TerminalManipulation& term) const
    {
        const int w = int(width);
        const int h = int(height);
        if (w < 2 || h < 2)
            return;

        const Glyphs g = glyphs();
        const float r = right() - 1.0f;
        const float b = bottom() - 1.0f;

        moveTo(term, left(), top());
        std::cout << g.topLeft << repeat(g.horizontal, w - 2) << g.topRight;

        for (float row = top() + 1.0f; row < b; ++row)
        {
            moveTo(term, left(), row);
            std::cout << g.vertical;
            moveTo(term, r, row);
            std::cout << g.vertical;
        }

        moveTo(term, left(), b);
        std::cout << g.botLeft << repeat(g.horizontal, w - 2) << g.botRight;
    }
};
} // namespace Utils::Terminal::Claude
