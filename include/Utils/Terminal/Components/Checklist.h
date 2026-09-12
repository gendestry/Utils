//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include "Utils/Terminal/Events/MouseEvent.h"
#include "Utils/Terminal/Interfaces/OnHover.h"
#include "Utils/Terminal/Interfaces/Renderable.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace Utils::Terminal::Components
{
// One "[x] text" row per item. Up/Down move between items, Space toggles the highlighted one.
//
// The bounds are sized from the items, so adding or renaming items after the widget is in a
// Screen means taking it out of the Screen's quadtree and inserting it again.
struct Checklist : public Iface::Renderable, public Iface::OnHover
{
    struct Item
    {
        std::string text;
        bool checked = false;
    };

    Checklist(float x, float y, const std::vector<std::string>& labels) : Renderable(x, y, 0, 0)
    {
        for (const std::string& label : labels)
        {
            items.push_back({label});
            width = std::max(width, float(label.size() + 4));
        }
        height = float(items.size());
    }

    static constexpr size_t NoRow = size_t(-1);

    std::vector<Item> items;
    size_t selected = 0;
    // Which row the cursor is over; hovering the widget isn't granular enough for a list.
    size_t hoveredRow = NoRow;
    std::function<void(size_t index, const Item& item)> onChange;

    bool focusable() const override { return true; }

    void onEvent(Events::Event& e) override
    {
        using namespace Events;
        EventDispatcher d(e);
        d.dispatch<EventChar>([&](EventChar& event)
        {
            if (event.get() != ' ' || selected >= items.size())
                return false;
            items[selected].checked = !items[selected].checked;
            if (onChange)
                onChange(selected, items[selected]);
            return true;
        });
        // At either end the arrow is left unhandled, so the Screen can move focus off the list.
        d.dispatch<EventArrowUp>([&](EventArrowUp&)
        {
            if (selected == 0)
                return false;
            --selected;
            return true;
        });
        d.dispatch<EventArrowDown>([&](EventArrowDown&)
        {
            if (selected + 1 >= items.size())
                return false;
            ++selected;
            return true;
        });

        // Hover doesn't claim the event: it's a side effect of motion, not a use for it.
        d.dispatch<EventMouseMoved>([&](EventMouseMoved& event)
        {
            hoveredRow = rowAt(event.y());
            return false;
        });
        d.dispatch<EventMousePressed>([&](EventMousePressed& event)
        {
            const size_t row = rowAt(event.y());
            if (!event.isLeft() || row == NoRow)
                return false;
            selected = row;
            items[row].checked = !items[row].checked;
            if (onChange)
                onChange(row, items[row]);
            return true;
        });
    }

    void render(Helper::TerminalManipulation& term) override
    {
        for (size_t i = 0; i < items.size(); ++i)
        {
            term.moveCursorToPosition(uint16_t(top() + i), uint16_t(left()));
            const bool highlight = hasFocus && i == selected;
            const bool hover = !highlight && i == hoveredRow;
            if (highlight)
                term.reverse();
            else if (hover)
                term.underline();
            std::cout << (items[i].checked ? "[x] " : "[ ] ") << items[i].text;
            if (highlight)
                term.noReverse();
            else if (hover)
                term.noUnderline();
        }
    }

  protected:
    void onHoverChanged(bool hovered) override
    {
        if (!hovered)
            hoveredRow = NoRow;
    }

  private:
    // `y` is an absolute terminal row; NoRow when it falls outside the list.
    size_t rowAt(int y) const
    {
        const int row = y - int(top());
        if (row < 0 || size_t(row) >= items.size())
            return NoRow;
        return size_t(row);
    }
};
}
