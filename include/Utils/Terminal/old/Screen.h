#pragma once
#include "../Events/MouseEvent.h"
#include "../Interfaces/OnEvent.h"
#include "../Interfaces/OnHover.h"
#include "../Interfaces/Renderable.h"
#include "Utils/Storage/Quadtree.h"

namespace Utils::Terminal
{

// Owns the widgets. Events go to the focused widget first; Tab and the arrow keys move focus;
// anything still unhandled goes to the Screen's own callback.
//
// Focusable widgets are also indexed in a quadtree, which picks the neighbour an arrow key
// points at. The tree reads a widget's bounds, so a widget that moves or resizes has to be
// removed from `focusables` first and inserted again afterwards.
class Screen : public Iface::OnEvent
{
    std::vector<std::unique_ptr<Iface::Renderable>> widgets;
    Quadtree<Iface::Renderable> focusables;
    Iface::Renderable* focused = nullptr;
    Iface::HoverTracker hovered;

    void focus(Iface::Renderable* widget)
    {
        if (focused)
            focused->hasFocus = false;
        focused = widget;
        if (focused)
            focused->hasFocus = true;
    }

    bool focusTowards(std::optional<Iface::Renderable*> next)
    {
        if (!next)
            return false;
        focus(*next);
        return true;
    }

    void focusNext()
    {
        if (widgets.empty())
            return;

        size_t start = 0;
        for (size_t i = 0; i < widgets.size(); ++i)
            if (widgets[i].get() == focused)
                start = i + 1;

        for (size_t n = 0; n < widgets.size(); ++n)
        {
            Iface::Renderable* candidate = widgets[(start + n) % widgets.size()].get();
            if (candidate->focusable())
            {
                focus(candidate);
                return;
            }
        }
    }

  public:
    // `bounds` must hold the center of every focusable widget, or it can't be navigated to.
    explicit Screen(const Maths::Rectangle& bounds) : focusables(bounds) {}

    template<typename T, typename... Args>
    T& add(Args&&... args)
    {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *widget;
        widgets.push_back(std::move(widget));

        if (ref.focusable())
        {
            focusables.insert(&ref);
            if (!focused)
                focus(&ref);
        }

        return ref;
    }

    void onEvent(Events::Event & e) override
    {
        // Mouse events go by position, not focus: whatever sits under the cursor gets them,
        // and a press moves focus there. Only focusables are in the quadtree, so a click on
        // a plain Label falls through to the Screen's own callback.
        if (e.IsInCategory(Events::EventCategoryMouse))
        {
            auto& me = static_cast<Events::MouseEvent &>(e);
            auto under = focusables.query(Maths::Point{float(me.x()), float(me.y())});
            Iface::Renderable* target = under.empty() ? nullptr : under.back();

            // Also with no target, so leaving the last widget clears its highlight.
            hovered.set(target);

            if (target)
            {
                if (e.getEventType() == Events::EventType::MOUSE_PRESSED)
                    focus(target);
                target->onEvent(e);
            }

            if (!e.handled)
                OnEvent::onEvent(e);
            return;
        }

        if (focused)
            focused->onEvent(e);

        if (!e.handled)
        {
            Events::EventDispatcher d(e);
            // d.dispatch<EventTab>([&](EventTab&)
            // {
            //     focusNext();
            //     return true;
            // });
            d.dispatch<Events::EventArrowLeft>([&](Events::EventArrowLeft &) { return focusTowards(focusables.left(focused)); });
            d.dispatch<Events::EventArrowRight>([&](Events::EventArrowRight &) { return focusTowards(focusables.right(focused)); });
            d.dispatch<Events::EventArrowUp>([&](Events::EventArrowUp &) { return focusTowards(focusables.up(focused)); });
            d.dispatch<Events::EventArrowDown>([&](Events::EventArrowDown &) { return focusTowards(focusables.down(focused)); });
        }

        if (!e.handled)
            OnEvent::onEvent(e);
    }

    void render(Helper::TerminalManipulation& term)
    {
        term.hideCursor();
        term.clearScreenAndMoveHome();

        for (auto& widget : widgets)
        {
            widget->render(term);
            if (widget->border)
                widget->border->draw(term, *widget);
        }

        if (focused && focused->placeCursor(term))
            term.showCursor();
    }
};

}