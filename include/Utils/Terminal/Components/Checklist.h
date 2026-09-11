//
// Created by bobi on 11. 9. 26.
//

#pragma once
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
struct Checklist : public Iface::Renderable
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

    std::vector<Item> items;
    size_t selected = 0;
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
    }

    void render(Helper::TerminalManipulation& term) override
    {
        for (size_t i = 0; i < items.size(); ++i)
        {
            term.moveCursorToPosition(uint16_t(top() + i), uint16_t(left()));
            const bool highlight = hasFocus && i == selected;
            if (highlight)
                term.reverse();
            std::cout << (items[i].checked ? "[x] " : "[ ] ") << items[i].text;
            if (highlight)
                term.noReverse();
        }
    }
};
}
