//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include "Utils/Terminal/Interfaces/Renderable.h"
#include <functional>
#include <iostream>
#include <string>

namespace Utils::Terminal::Components
{
struct Button : public Iface::Renderable
{
    using Renderable::Renderable;

    // Sized to fit "[ text ]" on a single row.
    Button(float x, float y, std::string label)
        : Renderable(x, y, float(label.size() + 2), 1), text(std::move(label))
    {
    }

    std::string text;
    std::function<void()> onPress;

    bool focusable() const override { return true; }

    void onEvent(Events::Event& e) override
    {
        using namespace Events;
        EventDispatcher d(e);
        d.dispatch<EventEnter>([&](EventEnter&)
        {
            if (onPress)
                onPress();
            return true;
        });
        d.dispatch<EventChar>([&](EventChar& event)
        {
            if (event.get() != ' ')
                return false;
            if (onPress)
                onPress();
            return true;
        });
    }

    void render(Helper::TerminalManipulation& term) override
    {
        term.moveCursorToPosition(uint16_t(top()), uint16_t(left()));
        if (hasFocus)
            term.reverse();
        std::cout << "[" << text << "]";
        if (hasFocus)
            term.noReverse();
    }
};
}
