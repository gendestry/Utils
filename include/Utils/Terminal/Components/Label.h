//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include "Utils/Terminal/Interfaces/Renderable.h"
#include <iostream>
#include <string>

namespace Utils::Terminal::Components
{
struct Label : public Iface::Renderable
{
    using Renderable::Renderable;

    std::string text;

    void render(Helper::TerminalManipulation& term) override
    {
        term.moveCursorToPosition(uint16_t(top()), uint16_t(left()));
        std::cout << text;
    }
};
}