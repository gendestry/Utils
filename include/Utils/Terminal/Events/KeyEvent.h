#pragma once
#include <cstdint>
#include <string>
#include "Event.h"

namespace Utils::Terminal::Events
{

enum class Mod : uint8_t { None=0, Shift=1, Alt=2, Ctrl=4, Super=8 };

constexpr Mod operator|(Mod a, Mod b) { return static_cast<Mod>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }

struct Mods
{
    uint8_t bits = 0;

    static Mods decodeMods(int param)   // param from ESC[1;<param><final>, encoded as 1 + bits
    {
        return { static_cast<uint8_t>(param > 0 ? (param - 1) & 0xF : 0) };
    }

    bool has(Mod m) const
    {
        return (bits & static_cast<uint8_t>(m)) == static_cast<uint8_t>(m);
    }

    bool shift() const { return has(Mod::Shift); }
    bool alt() const { return has(Mod::Alt); }
    bool ctrl() const { return has(Mod::Ctrl); }
    bool super() const { return has(Mod::Super); }

    bool operator==(Mod m) const { return bits == static_cast<uint8_t>(m); }

    // "ctrl+alt", or "" when nothing is held.
    std::string toString() const
    {
        std::string s;
        if (ctrl()) s += "ctrl+";
        if (alt()) s += "alt+";
        if (shift()) s += "shift+";
        if (super()) s += "super+";
        if (!s.empty())
            s.pop_back();
        return s;
    }
};

class KeyEvent : public Event
{
protected:
    Mods _mods;

public:
    KeyEvent(Mods mods = {}) : _mods(mods) {}

    const Mods& mods() const { return _mods; }
    bool shift() const { return _mods.shift(); }
    bool alt() const { return _mods.alt(); }
    bool ctrl() const { return _mods.ctrl(); }
    bool super() const { return _mods.super(); }

    // e.g. "ARROW_LEFT ctrl+shift"
    std::string toString() const override
    {
        std::string s = getName();
        if (_mods.bits)
            s += " " + _mods.toString();
        return s;
    }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);
};

class EventCtrlC : public KeyEvent
{
public:
    using KeyEvent::KeyEvent;
    EVENT_CLASS_TYPE(CTRL_C);
};

class EventEnter : public KeyEvent
{
public:
    using KeyEvent::KeyEvent;
    EVENT_CLASS_TYPE(ENTER);
};

class EventBackspace : public KeyEvent
{
public:
    using KeyEvent::KeyEvent;
    EVENT_CLASS_TYPE(BACKSPACE);
};

class EventTab : public KeyEvent
{
public:
    using KeyEvent::KeyEvent;
    EVENT_CLASS_TYPE(TAB);
};

class EventArrowUp : public KeyEvent
{
public:
    using KeyEvent::KeyEvent;
    EVENT_CLASS_TYPE(ARROW_UP);
};

class EventArrowDown : public KeyEvent
{
public:
    using KeyEvent::KeyEvent;
    EVENT_CLASS_TYPE(ARROW_DOWN);
};

class EventArrowLeft : public KeyEvent
{
public:
    using KeyEvent::KeyEvent;
    EVENT_CLASS_TYPE(ARROW_LEFT);
};

class EventArrowRight : public KeyEvent
{
public:
    using KeyEvent::KeyEvent;
    EVENT_CLASS_TYPE(ARROW_RIGHT);
};

class EventChar : public KeyEvent
{
    char _c;
public:
    EventChar(char c, Mods mods = {}) : KeyEvent(mods), _c(c) {}
    EVENT_CLASS_TYPE(CHAR);

    char get() const { return _c; }
};

enum class Key
{
    Char, Escape, Home, End, PageUp, PageDown, Insert, Delete,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
};

// Any key without a dedicated event class, and chords like Ctrl+Alt+7:
//   e.is(Mod::Ctrl | Mod::Alt, U'7')    e.is(Mod::None, Key::Delete)
// Modifiers must match exactly, so a Ctrl+7 binding doesn't fire on Ctrl+Alt+7.
class EventKey : public KeyEvent
{
    Key _key;
    char32_t _ch;

public:
    EventKey(Key key, char32_t ch = 0, Mods mods = {}) : KeyEvent(mods), _key(key), _ch(ch) {}
    EVENT_CLASS_TYPE(KEY);

    Key key() const { return _key; }
    char32_t ch() const { return _ch; } // only meaningful when key() == Key::Char

    std::string toString() const override
    {
        std::string s = KeyEvent::toString();
        if (_key == Key::Char)
            s += " '" + (_ch < 128 ? std::string(1, char(_ch)) : "U+" + std::to_string(_ch)) + "'";
        else
            s += " key=" + std::to_string(static_cast<int>(_key));
        return s;
    }

    bool is(Mod m, char32_t c) const { return _key == Key::Char && _mods == m && _ch == c; }
    bool is(Mod m, Key k) const { return _key == k && _mods == m; }
};
}