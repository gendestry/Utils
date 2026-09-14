//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include "Utils/Storage/Quadtree.h"
#include "Utils/Terminal/Interfaces/Renderable.h"
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace Utils::Terminal::Components
{
// A rectangle that owns child widgets and passes events down to them.
//
// Children are placed relative to the group's top-left corner and translated into screen
// coordinates as they are added, so the group's own rectangle has to hold all of them.
//
// Focus works the way the Screen's does: the focused child sees the event first, the arrow keys
// move between children, and an arrow with nowhere left to go inside the group stays unhandled,
// so whatever owns the group can move focus out of it.
struct Group : public Iface::Renderable
{
    explicit Group(const Maths::Rectangle& bounds)
        : Renderable(bounds.pos, bounds.width, bounds.height), focusables(bounds)
    {
    }

    Group(float x, float y, float width, float height) : Group(Maths::Rectangle{x, y, width, height}) {}

    std::vector<std::unique_ptr<Iface::Renderable>> children;
    Utils::Quadtree<Iface::Renderable> focusables;
    Iface::Renderable* focusedChild = nullptr;

    // `args` are the child's constructor arguments, with its position relative to the group.
    template<typename T, typename... Args>
    T& add(Args&&... args)
    {
        auto child = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *child;
        static_cast<Maths::Rectangle&>(ref) = ref.translated(pos);
        children.push_back(std::move(child));

        if (ref.focusable())
        {
            focusables.insert(&ref);
            if (!focusedChild)
                focusedChild = &ref;
        }

        return ref;
    }

    // Always true, because children are usually added after the group itself is, and whoever
    // owns the group decides then whether it can be focused. A group with no focusable child
    // leaves every key unhandled, so focus moves straight back out of it.
    bool focusable() const override { return true; }

    void onEvent(Events::Event& e) override
    {
        using namespace Events;
        if (focusedChild)
            focusedChild->onEvent(e);

        if (!e.handled)
        {
            EventDispatcher d(e);
            d.dispatch<EventArrowLeft>([&](EventArrowLeft&) { return focusTowards(focusables.left(focusedChild)); });
            d.dispatch<EventArrowRight>([&](EventArrowRight&) { return focusTowards(focusables.right(focusedChild)); });
            d.dispatch<EventArrowUp>([&](EventArrowUp&) { return focusTowards(focusables.up(focusedChild)); });
            d.dispatch<EventArrowDown>([&](EventArrowDown&) { return focusTowards(focusables.down(focusedChild)); });
        }

        if (!e.handled)
            OnEvent::onEvent(e);
    }

    void render(Helper::TerminalManipulation& term) override
    {
        for (auto& child : children)
        {
            // Only the focused child of a focused group draws itself highlighted.
            child->hasFocus = hasFocus && child.get() == focusedChild;
            child->render(term);
            if (child->border)
                child->border->draw(term, *child);
        }
    }

    bool placeCursor(Helper::TerminalManipulation& term) const override
    {
        return focusedChild != nullptr && focusedChild->placeCursor(term);
    }

  private:
    bool focusTowards(std::optional<Iface::Renderable*> next)
    {
        if (!next)
            return false;
        focusedChild = *next;
        return true;
    }
};
}
