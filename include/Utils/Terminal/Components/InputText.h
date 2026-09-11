//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include "Utils/Terminal/Interfaces/Renderable.h"
#include <cctype>
#include <functional>
#include <iostream>
#include <string>

namespace Utils::Terminal::Components
{
struct InputText : public Iface::Renderable
{
    using Renderable::Renderable;

    std::string text;
    size_t cursor = 0;
    std::function<void(const std::string&)> onSubmit;

    bool focusable() const override { return true; }

    void onEvent(Events::Event & e) override
    {
        using namespace Events;
        EventDispatcher d(e);
        d.dispatch<EventChar>([&](EventChar & event)
        {
            char c = event.get();
            // Ignore control bytes and keep one free cell for the cursor at the end.
            if (std::isprint(static_cast<unsigned char>(c)) && text.size() + 1 < static_cast<size_t>(width))
            {
                text.insert(cursor, 1, c);
                ++cursor;
            }
            return true;
        });
        d.dispatch<EventBackspace>([&](EventBackspace &)
        {
            if (cursor > 0)
                text.erase(--cursor, 1);
            return true;
        });
        d.dispatch<EventArrowLeft>([&](EventArrowLeft&)
        {
            if (cursor > 0)
                --cursor;
            return true;
        });
        d.dispatch<EventArrowRight>([&](EventArrowRight&)
        {
            if (cursor < text.size())
                ++cursor;
            return true;
        });
        d.dispatch<EventEnter>([&](EventEnter&)
        {
            if (onSubmit)
                onSubmit(text);
            text.clear();
            cursor = 0;
            return true;
        });
    }

    void render(Helper::TerminalManipulation& term) override
    {
        term.moveCursorToPosition(uint16_t(top()), uint16_t(left()));
        term.underline();
        std::cout << text << std::string(static_cast<size_t>(width) - text.size(), ' ');
        term.noUnderline();
    }

    void placeCursor(Helper::TerminalManipulation& term) const override
    {
        term.moveCursorToPosition(uint16_t(top()), uint16_t(left() + cursor));
    }
};
}