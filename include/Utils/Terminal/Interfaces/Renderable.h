//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include "OnEvent.h"
#include "Utils/Terminal/Helper/Border.h"
#include "Utils/Terminal/Helper/TerminalManipulation.h"
#include "Utils/Math/Rectangle.h"
#include <optional>

namespace Utils::Terminal::Iface
{
struct Renderable : public Maths::Rectangle, public OnEvent
{
    using Rectangle::Rectangle;
    void setWidth(float w) {width = w;}
    void setHeight(float h) {width = h;}

    virtual void render(Helper::TerminalManipulation& term) = 0;

    // Maintained by the Screen, so widgets can draw themselves highlighted.
    bool hasFocus = false;

    // Drawn one cell outside the widget's rectangle, after render(). The widget's own bounds
    // don't grow, so leave a free cell around it.
    std::optional<Helper::Border> border;

    // Only focusable widgets receive input from the Screen.
    virtual bool focusable() const { return false; }

    // Returns false when the widget has no text cursor to show.
    virtual bool placeCursor(Helper::TerminalManipulation& term) const { return false; }
};
}