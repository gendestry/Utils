//
// Created by bobi on 11. 9. 26.
//

#pragma once
#include "Utils/Storage/Quadtree.h"
#include "Utils/Terminal/Events/MouseEvent.h"
#include "Utils/Terminal/Interfaces/OnHover.h"
#include "Utils/Terminal/Interfaces/Renderable.h"
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace Utils::Terminal::Components
{
// A rectangle that owns child widgets and passes events down to them.
//
// Children are placed relative to the container's top-left corner and translated into screen
// coordinates as they are added, so the container's own rectangle has to hold all of them.
//
// Focus works the way the Screen's does: the focused child sees the event first, the arrow keys
// move between children, and an arrow with nowhere left to go inside the container stays
// unhandled, so whatever owns the container can move focus out of it.
struct Container : public Iface::Renderable, public Iface::OnHover
{
    explicit Container(const Maths::Rectangle& bounds)
        : Renderable(bounds.pos, bounds.width, bounds.height), focusables(bounds)
    {
    }

    Container(float x, float y, float width, float height)
        : Container(Maths::Rectangle{x, y, width, height})
    {
    }

    std::vector<std::unique_ptr<Iface::Renderable>> children;
    Utils::Quadtree<Iface::Renderable> focusables;
    Iface::Renderable* focusedChild = nullptr;
    Iface::HoverTracker hoveredChild;

    // `args` are the child's constructor arguments, with its position relative to the container.
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

    // Always true, because children are usually added after the container itself is, and whoever
    // owns the container decides then whether it can be focused. A container with no focusable child
    // leaves every key unhandled, so focus moves straight back out of it.
    bool focusable() const override { return true; }

    // Routing only: the container claims nothing of its own. Mouse events go to the child under
    // the cursor, everything else to the focused child, and whatever no child handled is offered
    // to onUnhandled() before the container's own callback. Subclasses extend by overriding
    // onUnhandled(), not this -- that keeps children first and the callback last. Overriding
    // onEvent is still open to a subclass that has to see events before its children do.
    void onEvent(Events::Event& e) override
    {
        using namespace Events;

        if (e.IsInCategory(EventCategoryMouse))
        {
            auto& me = static_cast<MouseEvent&>(e);
            auto under = focusables.query(Maths::Point{float(me.x()), float(me.y())});
            Iface::Renderable* target = under.empty() ? nullptr : under.back();

            // Also with no target: the cursor can be inside the container but between children.
            hoveredChild.set(target);

            if (target)
            {
                if (e.getEventType() == EventType::MOUSE_PRESSED)
                    focusedChild = target;
                target->onEvent(e);
            }
        }
        else if (focusedChild)
        {
            focusedChild->onEvent(e);
        }

        if (!e.handled)
            onUnhandled(e);

        if (!e.handled)
            OnEvent::onEvent(e);
    }

    void render(Helper::TerminalManipulation& term) override
    {
        for (auto& child : children)
        {
            // Only the focused child of a focused container draws itself highlighted.
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

  protected:
    // Anything no child took. Empty here -- a plain container adds no behaviour of its own.
    // Set e.handled in an override to stop it reaching the container's callback.
    virtual void onUnhandled(Events::Event& /*e*/) {}

    // The Screen only clears the container itself, so the child under the cursor has to be let go
    // of here -- otherwise it stays highlighted after the cursor has left the container entirely.
    void onHoverChanged(bool hovered) override
    {
        if (!hovered)
            hoveredChild.clear();
    }
};
}
