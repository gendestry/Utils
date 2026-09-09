#pragma once
#include <string>

enum class EventType
{
    CTRL_C,
    CTRL_D,
    ENTER,
    BACKSPACE,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    CTRL_ARROW_LEFT,
    CTRL_ARROW_RIGHT,
    TAB
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

class Event
{
public:
    virtual ~Event() = default;

    bool handled = false;

    virtual EventType getEventType() const = 0;
    virtual const char* getName() const = 0;
    virtual std::string toString() const { return getName(); }
};

class EventDispatcher
{
public:
    EventDispatcher(Event& event)
        : m_event(event)
    {
    }

    // F will be deduced by the compiler
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
//
// inline std::ostream& operator<<(std::ostream& os, const Event& e)
// {
//     return os << e.toString();
// }
