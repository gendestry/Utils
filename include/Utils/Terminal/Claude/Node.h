#pragma once
#include "Layout.h"
#include "Utils/Terminal/Helper/TerminalManipulation.h"
#include "Utils/Terminal/Interfaces/OnEvent.h"

#include <cstdint>
#include <functional>

namespace Utils::Terminal::Claude
{
// Every element of the tree. The node *is* its rectangle: arrange() writes pos/width/height,
// in absolute 0-based terminal cells -- the space Terminal reports mouse positions in -- so
// hit-testing and rendering never have to translate through parents. moveTo() is the one place
// that converts to the 1-based cursor positions the terminal draws with.
//
// Three passes, kept apart:
//   measure()  bottom-up, pure       -- what would I like to be, decoration included
//   arrange()  top-down, writes rect -- here is what you get
//   render()   top-down, draws       -- strictly inside the rect, never changes it
struct Node : Rectangle, Iface::OnEvent
{
    LayoutSpec layout;

    // Set by the container that adopts this node; the root has none.
    Node* parent = nullptr;

    // Maintained by the App.
    bool hasFocus = false;
    bool isHovered = false;

    // Natural size, including the node's own decoration (a Frame adds its insets).
    virtual Size measure() const { return {0.0f, 0.0f}; }

    // Smallest size the node can be shrunk to without breaking.
    virtual Size minimum() const { return layout.minSize; }

    // measure(), with any fixed size from the spec applied. This is what a parent reads.
    Size desired() const
    {
        Size s = measure();
        if (layout.width)
            s.x = *layout.width;
        if (layout.height)
            s.y = *layout.height;
        return s;
    }

    virtual void arrange(const Rectangle& r) { static_cast<Rectangle&>(*this) = r; }

    virtual void render(Helper::TerminalManipulation&) {}

    // Leaves have none. Frame has one, Box many.
    virtual void forEachChild(const std::function<void(Node&)>&) {}

    virtual bool focusable() const { return false; }

    // Whether the mouse can land on it. A plain Text is not, so a click passes through to the
    // node behind it.
    virtual bool hittable() const { return focusable(); }

    // Cursor to a 0-based cell.
    static void moveTo(Helper::TerminalManipulation& term, float column, float row)
    {
        term.moveCursorToPosition(uint16_t(row + 1.0f), uint16_t(column + 1.0f));
    }

    // Returns false when the node has no text cursor to show.
    virtual bool placeCursor(Helper::TerminalManipulation&) const { return false; }

    // Call after changing anything measure() reads. Marks the root, which the App checks before
    // the next render, so several changes in one event cost one layout pass.
    void markLayoutDirty()
    {
        Node* n = this;
        while (n->parent)
            n = n->parent;
        n->layoutDirty = true;
    }

    // Only read on the root.
    bool layoutDirty = true;
};
} // namespace Utils::Terminal::Claude
