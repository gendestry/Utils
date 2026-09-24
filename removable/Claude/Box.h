#pragma once
#include "Node.h"

#include <cmath>
#include <memory>
#include <vector>

namespace Utils::Terminal::Claude
{
// Many children laid out along one axis -- hbox or vbox. A Box only sizes; it draws nothing of
// its own and routes no events. Wrap it in a Frame for a border or padding.
struct Box : Node
{
    Axis axis = Axis::Horizontal;
    Align align = Align::Stretch;
    float gap = 0.0f;
    std::vector<std::unique_ptr<Node>> children;

    explicit Box(Axis axis) : axis(axis) {}

    Node& add(std::unique_ptr<Node> child)
    {
        child->parent = this;
        children.push_back(std::move(child));
        markLayoutDirty();
        return *children.back();
    }

    Size measure() const override
    {
        return accumulate([](const Node& n) { return n.desired(); });
    }

    Size minimum() const override
    {
        const Size s = accumulate([](const Node& n) { return n.minimum(); });
        return {std::max(s.x, layout.minSize.x), std::max(s.y, layout.minSize.y)};
    }

    void arrange(const Rectangle& r) override
    {
        Node::arrange(r);
        if (children.empty())
            return;

        const float avail = std::max(0.0f, mainOf(r.size()) - gapTotal());

        std::vector<float> sizes = baseSizes();
        distribute(sizes, avail);
        const std::vector<float> cells = snapToCells(sizes);

        float offset = 0.0f;
        for (size_t i = 0; i < children.size(); ++i)
        {
            children[i]->arrange(cellRect(r, offset, cells[i], *children[i]));
            offset += cells[i] + gap;
        }
    }

    void render(Helper::TerminalManipulation& term) override
    {
        for (auto& child : children)
            child->render(term);
    }

    void forEachChild(const std::function<void(Node&)>& fn) override
    {
        for (auto& child : children)
            fn(*child);
    }

  private:
    float mainOf(Size s) const { return axis == Axis::Horizontal ? s.x : s.y; }
    float crossOf(Size s) const { return axis == Axis::Horizontal ? s.y : s.x; }

    Size fromAxes(float main, float cross) const
    {
        return axis == Axis::Horizontal ? Size{main, cross} : Size{cross, main};
    }

    float gapTotal() const { return gap * float(children.size() > 0 ? children.size() - 1 : 0); }

    // A fixed size on the main axis opts the child out of grow and shrink.
    bool fixedMain(const Node& n) const
    {
        return axis == Axis::Horizontal ? n.layout.width.has_value() : n.layout.height.has_value();
    }

    bool fixedCross(const Node& n) const
    {
        return axis == Axis::Horizontal ? n.layout.height.has_value() : n.layout.width.has_value();
    }

    template<typename F> Size accumulate(F sizeOf) const
    {
        float main = gapTotal();
        float cross = 0.0f;
        for (const auto& child : children)
        {
            const Size s = sizeOf(*child);
            main += mainOf(s);
            cross = std::max(cross, crossOf(s));
        }
        return fromAxes(main, cross);
    }

    // 1. What every child would like, before anyone is given more or less.
    std::vector<float> baseSizes() const
    {
        std::vector<float> sizes;
        sizes.reserve(children.size());
        for (const auto& child : children)
            sizes.push_back(mainOf(child->desired()));
        return sizes;
    }

    // 2. Hand out the difference: surplus by grow, overflow by shrink weighted by size, so a wide
    //    child gives back more than a narrow one. No child goes below its minimum.
    void distribute(std::vector<float>& sizes, float avail) const
    {
        float used = 0.0f;
        for (float s : sizes)
            used += s;

        const float slack = avail - used;

        if (slack > 0.0f)
        {
            float totalGrow = 0.0f;
            for (const auto& child : children)
                if (!fixedMain(*child))
                    totalGrow += child->layout.grow;

            if (totalGrow <= 0.0f)
                return;

            for (size_t i = 0; i < children.size(); ++i)
                if (!fixedMain(*children[i]))
                    sizes[i] += slack * children[i]->layout.grow / totalGrow;
        }
        else if (slack < 0.0f)
        {
            float totalWeight = 0.0f;
            for (size_t i = 0; i < children.size(); ++i)
                if (!fixedMain(*children[i]))
                    totalWeight += children[i]->layout.shrink * sizes[i];

            if (totalWeight <= 0.0f)
                return;

            for (size_t i = 0; i < children.size(); ++i)
            {
                if (fixedMain(*children[i]))
                    continue;
                const float weight = children[i]->layout.shrink * sizes[i];
                const float floor = mainOf(children[i]->minimum());
                sizes[i] = std::max(floor, sizes[i] + slack * weight / totalWeight);
            }
        }
    }

    // 3. A terminal has no fractional cells. Round the running total rather than each size, so
    //    the remainder carries forward: 20 columns split three ways is 7+6+7, not 7+7+7 or a hole.
    static std::vector<float> snapToCells(const std::vector<float>& sizes)
    {
        std::vector<float> cells;
        cells.reserve(sizes.size());
        float total = 0.0f;
        float placed = 0.0f;
        for (float s : sizes)
        {
            total += s;
            const float end = std::round(total);
            cells.push_back(std::max(0.0f, end - placed));
            placed = std::max(placed, end);
        }
        return cells;
    }

    // 4. The child's rectangle: `offset` and `main` on the main axis, `align` on the cross axis.
    Rectangle cellRect(const Rectangle& r, float offset, float main, const Node& child) const
    {
        const float crossAvail = crossOf(r.size());
        float cross = std::min(crossOf(child.desired()), crossAvail);
        float crossOffset = 0.0f;

        if (align == Align::Stretch && !fixedCross(child))
            cross = crossAvail;
        else if (align == Align::Center)
            crossOffset = std::floor((crossAvail - cross) / 2.0f);
        else if (align == Align::End)
            crossOffset = crossAvail - cross;

        if (axis == Axis::Horizontal)
            return {{r.left() + offset, r.top() + crossOffset}, main, cross};
        return {{r.left() + crossOffset, r.top() + offset}, cross, main};
    }
};
} // namespace Utils::Terminal::Claude
