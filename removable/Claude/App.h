#pragma once
#include "Dsl.h"
#include "Node.h"
#include "Utils/Storage/Quadtree.h"
#include "Utils/Terminal/Events/KeyEvent.h"
#include "Utils/Terminal/Events/MouseEvent.h"
#include "Utils/Terminal/Terminal.h"

#include <memory>
#include <utility>
#include <vector>

namespace Utils::Terminal::Claude
{
// Owns the tree, runs the layout pass and routes events. The only owner of the quadtree:
// rects are absolute, so no container needs an index of its own.
//
// Routing, in order:
//   taps     see every event, handle nothing
//   mouse    by position: hit-test the tree, hover, focus on press, deliver
//   keys     to the focused node
//   nav      Tab walks the tree in order; arrows ask the quadtree for the nearest focusable
//   fallback the App's own callback, for whatever nobody handled
class App : public Iface::OnEvent
{
    Terminal& m_terminal;
    std::unique_ptr<Node> m_root;

    // Focusables only, and only for arrow navigation -- "nearest in this direction" is the one
    // question a tree walk can't answer. Hit-testing walks the tree instead. Rebuilt on every
    // layout pass, since the quadtree needs an object removed before it moves.
    std::unique_ptr<Quadtree<Node>> m_nav;

    Node* m_focused = nullptr;
    Node* m_hovered = nullptr;
    std::vector<Callback> m_taps;
    std::pair<int, int> m_laidOutFor{0, 0};

  public:
    App(Terminal& terminal, Element root) : m_terminal(terminal), m_root(root.release())
    {
        relayout();
        walk(*m_root, [&](Node& n) {
            if (!m_focused && n.focusable())
                focus(&n);
        });
    }

    Node& root() { return *m_root; }
    Node* focused() const { return m_focused; }

    // Observers: called first with every event, before any routing. Must not set `handled`.
    void tap(Callback cb) { m_taps.push_back(std::move(cb)); }

    void relayout()
    {
        auto [rows, cols] = Terminal::getSize();
        if (rows <= 0 || cols <= 0)
        {
            rows = 24;
            cols = 80;
        }
        m_laidOutFor = {rows, cols};

        const Rectangle screen{0.0f, 0.0f, float(cols), float(rows)};
        m_root->arrange(screen);

        m_nav = std::make_unique<Quadtree<Node>>(screen);
        walk(*m_root, [&](Node& n) {
            if (n.focusable() && !n.empty())
                m_nav->insert(&n);
        });

        m_root->layoutDirty = false;
    }

    void onEvent(Events::Event& e) override
    {
        for (auto& t : m_taps)
            t(e);

        if (routeMouse(e))
            return;

        if (m_focused)
            m_focused->onEvent(e);

        if (!e.handled)
            routeNavigation(e);

        if (!e.handled)
            OnEvent::onEvent(e);
    }

    // Lays out first if anything asked for it (or the terminal was resized), so a burst of
    // changes costs one layout pass.
    void render()
    {
        if (m_root->layoutDirty || m_laidOutFor != Terminal::getSize())
            relayout();

        auto& term = m_terminal.manipulate();
        term.hideCursor();
        term.clearScreenAndMoveHome();
        m_root->render(term);
        if (m_focused && m_focused->placeCursor(term))
            term.showCursor();
        term.flush();
    }

    // Deepest hittable node under `p`; later siblings win, since they are drawn on top.
    Node* hitTest(Point p) { return hitTest(*m_root, p); }

  private:
    static void walk(Node& n, const std::function<void(Node&)>& fn)
    {
        fn(n);
        n.forEachChild([&](Node& child) { walk(child, fn); });
    }

    static Node* hitTest(Node& n, Point p)
    {
        if (!n.contains(p))
            return nullptr;

        Node* found = n.hittable() ? &n : nullptr;
        n.forEachChild([&](Node& child) {
            if (Node* hit = hitTest(child, p))
                found = hit;
        });
        return found;
    }

    void focus(Node* n)
    {
        if (m_focused)
            m_focused->hasFocus = false;
        m_focused = n;
        if (m_focused)
            m_focused->hasFocus = true;
    }

    // Also called with nullptr, so moving off a node clears its highlight.
    void hover(Node* n)
    {
        if (m_hovered == n)
            return;
        if (m_hovered)
            m_hovered->isHovered = false;
        m_hovered = n;
        if (m_hovered)
            m_hovered->isHovered = true;
    }

    bool routeMouse(Events::Event& e)
    {
        if (!e.IsInCategory(Events::EventCategoryMouse))
            return false;

        auto& me = static_cast<Events::MouseEvent&>(e);
        Node* target = hitTest(Point{float(me.x()), float(me.y())});

        hover(target);

        if (target)
        {
            if (e.getEventType() == Events::EventType::MOUSE_PRESSED && target->focusable())
                focus(target);
            target->onEvent(e);
        }

        if (!e.handled)
            OnEvent::onEvent(e);
        return true;
    }

    void routeNavigation(Events::Event& e)
    {
        using namespace Events;
        EventDispatcher d(e);
        d.dispatch<EventTab>([&](EventTab&) { return focusNext(); });
        d.dispatch<EventArrowLeft>([&](EventArrowLeft&) { return focusTowards(m_nav->left(m_focused)); });
        d.dispatch<EventArrowRight>([&](EventArrowRight&) { return focusTowards(m_nav->right(m_focused)); });
        d.dispatch<EventArrowUp>([&](EventArrowUp&) { return focusTowards(m_nav->up(m_focused)); });
        d.dispatch<EventArrowDown>([&](EventArrowDown&) { return focusTowards(m_nav->down(m_focused)); });
    }

    bool focusTowards(std::optional<Node*> next)
    {
        if (!m_focused || !next)
            return false;
        focus(*next);
        return true;
    }

    bool focusNext()
    {
        std::vector<Node*> order;
        walk(*m_root, [&](Node& n) {
            if (n.focusable())
                order.push_back(&n);
        });
        if (order.empty())
            return false;

        size_t next = 0;
        for (size_t i = 0; i < order.size(); ++i)
            if (order[i] == m_focused)
                next = (i + 1) % order.size();

        focus(order[next]);
        return true;
    }
};
} // namespace Utils::Terminal::Claude
