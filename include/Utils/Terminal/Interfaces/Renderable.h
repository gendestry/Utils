//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include "OnEvent.h"
#include "Utils/Terminal/Helper/TerminalManipulation.h"
#include "Utils/Math/Rectangle.h"

namespace Utils::Terminal::Iface
{
struct Renderable : public Maths::Rectangle, public OnEvent
{
    using Rectangle::Rectangle;

    virtual void render(Helper::TerminalManipulation& term) = 0;

    // Only focusable widgets receive input from the Screen.
    virtual bool focusable() const { return false; }
    virtual void placeCursor(Helper::TerminalManipulation& term) const {}
};
}