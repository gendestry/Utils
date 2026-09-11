#pragma once
#include <string>
#include "Event.h"

namespace Utils::Terminal::Events
{

struct Mods { bool shift, alt, ctrl, super; };

inline Mods decodeMods(int param)   // param from ESC[1;<param><final>
{
    int m = (param > 0 ? param - 1 : 0);
    return { bool(m & 1), bool(m & 2), bool(m & 4), bool(m & 8) };
}

class KeyEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);
};

class EventCtrlC : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(CTRL_C);
};

class EventEnter : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(ENTER);
};

class EventBackspace : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(BACKSPACE);
};

class EventTab : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(TAB);
};

class EventArrowUp : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(ARROW_UP);
};

class EventArrowDown : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(ARROW_DOWN);
};

class EventArrowLeft : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(ARROW_LEFT);
};

class EventArrowRight : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(ARROW_RIGHT);
};

class EventChar : public KeyEvent
{
    char _c;
public:
    EventChar(char c) : _c(c) {}
    EVENT_CLASS_TYPE(CHAR);

    char get() const { return _c; }
};
}