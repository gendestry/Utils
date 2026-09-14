#pragma once
#include "Event.h"
#include "KeyEvent.h" // Mods

namespace Utils::Terminal::Events
{

enum class MouseButton
{
    Left = 0,
    Middle = 1,
    Right = 2,
    None = 3
};

inline const char* toString(MouseButton button)
{
    switch (button)
    {
    case MouseButton::Left:
        return "Left";
    case MouseButton::Middle:
        return "Middle";
    case MouseButton::Right:
        return "Right";
    default:
        return "None";
    }
}

// Position is a 1-based terminal cell (column, row) -- the same space widget rectangles live in,
// so it can be fed straight to a hit test without conversion.
class MouseEvent : public Event
{
protected:
    int _x, _y;
    Mods _mods;

public:
    MouseEvent(int x, int y, Mods mods) : _x(x), _y(y), _mods(mods) {}

    int x() const { return _x; }
    int y() const { return _y; }
    const Mods& mods() const { return _mods; }
    bool shift() const { return _mods.shift(); }
    bool alt() const { return _mods.alt(); }
    bool ctrl() const { return _mods.ctrl(); }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);

protected:
    // " (x, y)" plus " ctrl+alt" when modifiers are held; shared by the toString()s below.
    std::string posAndMods() const
    {
        std::string s = " (" + std::to_string(_x) + ", " + std::to_string(_y) + ")";
        if (_mods.bits)
            s += " " + _mods.toString();
        return s;
    }
};

// Carries a button so a handler can tell left from right; the dispatcher only matches on the
// event type, so filtering is done with button() / isLeft() inside the callback.
class MouseButtonEvent : public MouseEvent
{
protected:
    MouseButton _button;

public:
    MouseButtonEvent(MouseButton button, int x, int y, Mods mods)
        : MouseEvent(x, y, mods), _button(button)
    {
    }

    MouseButton button() const { return _button; }
    bool isLeft() const { return _button == MouseButton::Left; }
    bool isMiddle() const { return _button == MouseButton::Middle; }
    bool isRight() const { return _button == MouseButton::Right; }

    // Exact modifier match: e.is(Mod::Ctrl, MouseButton::Left) is false for Ctrl+Shift+click.
    bool is(Mod m, MouseButton b) const { return _button == b && _mods == m; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput);
};

class EventMousePressed : public MouseButtonEvent
{
public:
    using MouseButtonEvent::MouseButtonEvent;
    EVENT_CLASS_TYPE(MOUSE_PRESSED);

    std::string toString() const override
    {
        return std::string(getName()) + " " + Events::toString(_button) + posAndMods();
    }
};

class EventMouseReleased : public MouseButtonEvent
{
public:
    using MouseButtonEvent::MouseButtonEvent;
    EVENT_CLASS_TYPE(MOUSE_RELEASED);

    std::string toString() const override
    {
        return std::string(getName()) + " " + Events::toString(_button) + posAndMods();
    }
};

// Motion with no button held. Motion *with* a button held is an EventMouseDragged instead.
class EventMouseMoved : public MouseEvent
{
public:
    using MouseEvent::MouseEvent;
    EVENT_CLASS_TYPE(MOUSE_MOVED);

    std::string toString() const override
    {
        return std::string(getName()) + posAndMods();
    }
};

// Motion while a button is held down. Arrives repeatedly as the cursor moves, between the
// EventMousePressed that started the drag and the EventMouseReleased that ends it -- the
// terminal reports no separate "still holding" tick, so a stationary hold emits nothing.
class EventMouseDragged : public MouseButtonEvent
{
public:
    using MouseButtonEvent::MouseButtonEvent;
    EVENT_CLASS_TYPE(MOUSE_DRAGGED);

    std::string toString() const override
    {
        return std::string(getName()) + " " + Events::toString(_button) + posAndMods();
    }
};

// `delta` is +1 per wheel notch up, -1 per notch down.
class EventMouseScrolled : public MouseEvent
{
    int _delta;

public:
    EventMouseScrolled(int x, int y, Mods mods, int delta)
        : MouseEvent(x, y, mods), _delta(delta)
    {
    }

    int delta() const { return _delta; }
    bool up() const { return _delta > 0; }
    bool down() const { return _delta < 0; }

    EVENT_CLASS_TYPE(MOUSE_SCROLLED);

    std::string toString() const override
    {
        return std::string(getName()) + " " + std::to_string(_delta) + posAndMods();
    }
};
}
