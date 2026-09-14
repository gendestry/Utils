//
// Created by bobi on 12. 9. 26.
//

#pragma once
#include <functional>

namespace Utils::Terminal::Iface
{
// Opt-in hover state, the way OnEvent is opt-in behaviour: a widget that wants to know when the
// cursor is over it inherits this alongside Renderable, and one that doesn't costs nothing.
//
//     struct PlainLabel : Label {};                     // never hovers
//     struct LitLabel   : Label, Iface::OnHover         // hovers
//     {
//         void render(Helper::TerminalManipulation& term) override
//         {
//             if (isHovered) term.underline();
//             Label::render(term);
//             if (isHovered) term.noUnderline();
//         }
//     };
//
// The state is maintained from outside by whoever routes mouse events -- the Screen for its
// widgets, a Container for its children -- so widgets read isHovered in render() rather than
// handling an event for it. Side effects on crossing the boundary go in onEnter / onLeave, or in
// an onHoverChanged() override for widgets that have their own state to reset.
struct OnHover
{
    using Callback = std::function<void()>;

    bool isHovered = false;
    Callback onEnter;
    Callback onLeave;

    virtual ~OnHover() = default;

    // Idempotent, because motion is reported for every cell crossed inside a widget and only
    // the first of those reports is a change.
    void setHovered(bool value)
    {
        if (isHovered == value)
            return;

        isHovered = value;
        onHoverChanged(value);

        if (value)
        {
            if (onEnter)
                onEnter();
        }
        else if (onLeave)
            onLeave();
    }

  protected:
    // For widgets owning children or per-row state that has to be cleared when the cursor leaves.
    virtual void onHoverChanged(bool /*hovered*/) {}
};

// The other half, for whoever does the routing: remembers what was under the cursor last time, so
// that leaving a widget gets noticed. The terminal reports no such thing -- it only ever says
// where the cursor now is.
//
// set() takes any widget and works out whether it hovers at all, so a router doesn't have to care
// which of its children inherit OnHover. Moving onto one that doesn't clears the previous, which
// is what should happen: the cursor did leave it.
class HoverTracker
{
    OnHover* current = nullptr;

  public:
    OnHover* get() const { return current; }

    template<typename T>
    void set(T* widget)
    {
        setHovered(dynamic_cast<OnHover*>(widget));
    }

    void clear() { setHovered(nullptr); }

  private:
    void setHovered(OnHover* next)
    {
        if (current == next)
            return;

        if (current)
            current->setHovered(false);

        current = next;

        if (current)
            current->setHovered(true);
    }
};
}
