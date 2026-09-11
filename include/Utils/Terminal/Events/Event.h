#pragma once
#include <string>

namespace Utils::Terminal::Events
{
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
    CHAR,
    MOUSE_PRESSED,
    MOUSE_RELEASED,
    MOUSE_MOVED,
    MOUSE_DRAGGED,
    MOUSE_SCROLLED
};

#define BIT(x) (1 << (x))

enum EventCategory
{
    None = 0,
    EventCategoryApplication    = BIT(0),
    EventCategoryInput          = BIT(1),
    EventCategoryKeyboard       = BIT(2),
    EventCategoryMouse          = BIT(3),
    EventCategoryMouseButton    = BIT(4)
};

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
								virtual EventType getEventType() const override { return getStaticType(); }\
								virtual const char* getName() const override { return #type; }

class Event
{
public:
    virtual ~Event() = default;

    bool handled = false;

    virtual EventType getEventType() const = 0;
    virtual const char* getName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string toString() const { return getName(); }

    bool IsInCategory(EventCategory category)
    {
        return GetCategoryFlags() & category;
    }
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
}