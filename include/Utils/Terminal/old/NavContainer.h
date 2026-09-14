//
// Created by bobi on 12. 9. 26.
//

#pragma once
#include "../Components/Container.h"

namespace Utils::Terminal::Components
{
// A Container that moves focus between its children with the arrow keys, the way the Screen does
// between top-level widgets. An arrow with nowhere left to go inside the container is left
// unhandled, so focus moves back out of it instead of sticking.
//
// Also the worked example of extending a Container: the routing is inherited untouched, and the
// only thing added is what to do with an event no child wanted.
struct NavContainer : public Container
{
    using Container::Container;

  protected:
    void onUnhandled(Events::Event& e) override
    {
        using namespace Events;
        EventDispatcher d(e);
        d.dispatch<EventArrowLeft>([&](EventArrowLeft&) { return focusTowards(focusables.left(focusedChild)); });
        d.dispatch<EventArrowRight>([&](EventArrowRight&) { return focusTowards(focusables.right(focusedChild)); });
        d.dispatch<EventArrowUp>([&](EventArrowUp&) { return focusTowards(focusables.up(focusedChild)); });
        d.dispatch<EventArrowDown>([&](EventArrowDown&) { return focusTowards(focusables.down(focusedChild)); });
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
