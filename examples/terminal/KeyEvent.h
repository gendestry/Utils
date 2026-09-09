#pragma once
#include <string>

enum class EventType
{
    CTRL_C,
    CTRL_D,
    ENTER,
    BACKSPACE,
    TAB,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    CTRL_ARROW_LEFT,
    CTRL_ARROW_RIGHT,
};

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
								virtual EventType getEventType() const override { return getStaticType(); }\
								virtual const char* getName() const override { return #type; }

struct Mods { bool shift, alt, ctrl, super; };

static Mods decodeMods(int param)   // param from ESC[1;<param><final>
{
    int m = (param > 0 ? param - 1 : 0);
    return { bool(m & 1), bool(m & 2), bool(m & 4), bool(m & 8) };
}

class Event
{
public:
    virtual ~Event() = default;

    bool handled = false;

    virtual EventType getEventType() const = 0;
    virtual const char* getName() const = 0;
    virtual std::string toString() const { return getName(); }
};

class EventEnter : public Event
{
public:
    EVENT_CLASS_TYPE(ENTER);
};

class EventBackspace : public Event
{
public:
    EVENT_CLASS_TYPE(BACKSPACE);
};

class EventTab : public Event
{
public:
    EVENT_CLASS_TYPE(TAB);
};

class EventArrowUp : public Event
{
public:
    EVENT_CLASS_TYPE(ARROW_UP);
};

class EventArrowDown : public Event
{
public:
    EVENT_CLASS_TYPE(ARROW_DOWN);
};

class EventArrowLeft : public Event
{
public:
    EVENT_CLASS_TYPE(ARROW_LEFT);
};

class EventArrowRight : public Event
{
public:
    EVENT_CLASS_TYPE(ARROW_RIGHT);
};

class EventDispatcher
{
public:
    EventDispatcher(Event& event)
        : m_event(event)
    {
    }
    template<typename T, typename F>
    bool dispatch(const F& func)
    {
        if (m_event.getEventType() == T::getStaticType())
        {
            m_event.handled |= func(static_cast<T&>(m_event));
            return true;
        }
        return false;
    }
private:
    Event& m_event;
};
