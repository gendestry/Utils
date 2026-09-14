#pragma once
#include "Box.h"
#include "Frame.h"
#include "Widgets.h"

#include <cassert>
#include <concepts>
#include <functional>
#include <initializer_list>
#include <memory>

namespace Utils::Terminal::Claude
{
// Carries one node from a builder to wherever it ends up: a container, a decorator, the App.
//
// It exists for brace lists. hbox({ a, b, c }) goes through std::initializer_list, whose elements
// are const and can only be copied -- a plain unique_ptr can't come out of one. So copying an
// Element *moves* the node, and the member is mutable so that is legal on a const element.
// Treat it as a hand-off: once passed on, the original is empty.
class Element
{
    mutable std::unique_ptr<Node> m_node;

  public:
    Element() = default;

    template<std::derived_from<Node> T>
    Element(std::unique_ptr<T> node) : m_node(std::move(node))
    {
    }

    Element(const Element& other) : m_node(std::move(other.m_node)) {}
    Element(Element&& other) noexcept : m_node(std::move(other.m_node)) {}
    Element& operator=(const Element& other)
    {
        m_node = std::move(other.m_node);
        return *this;
    }
    Element& operator=(Element&& other) noexcept
    {
        m_node = std::move(other.m_node);
        return *this;
    }

    Node* get() const { return m_node.get(); }
    Node* operator->() const { return m_node.get(); }
    explicit operator bool() const { return bool(m_node); }

    std::unique_ptr<Node> release() const { return std::move(m_node); }
};

using Decorator = std::function<Element(Element)>;

// text("x") | border | flex  ==  flex(border(text("x")))
inline Element operator|(Element e, const Decorator& decorate)
{
    return decorate(std::move(e));
}

namespace Dsl
{
// ---- leaves -------------------------------------------------------------------------------

inline Element text(std::string s) { return std::make_unique<Text>(std::move(s)); }

inline Element button(std::string label, std::function<void()> onPress)
{
    return std::make_unique<Button>(std::move(label), std::move(onPress));
}

inline Element filler() { return std::make_unique<Filler>(); }

// ---- containers ---------------------------------------------------------------------------

inline Element box(Axis axis, std::initializer_list<Element> children)
{
    auto b = std::make_unique<Box>(axis);
    for (const Element& child : children)
        if (child)
            b->add(child.release());
    return b;
}

inline Element hbox(std::initializer_list<Element> children) { return box(Axis::Horizontal, children); }
inline Element vbox(std::initializer_list<Element> children) { return box(Axis::Vertical, children); }

// ---- wrappers: insert a node --------------------------------------------------------------

inline Element border(Element e)
{
    return std::make_unique<Frame>(e.release(), Insets::all(1.0f), Stroke::Rounded);
}

inline Decorator borderStyled(Stroke stroke)
{
    return [stroke](Element e) -> Element {
        return std::make_unique<Frame>(e.release(), Insets::all(1.0f), stroke);
    };
}

inline Decorator padding(float n)
{
    return [n](Element e) -> Element { return std::make_unique<Frame>(e.release(), Insets::all(n)); };
}

inline Decorator padding(Insets insets)
{
    return [insets](Element e) -> Element { return std::make_unique<Frame>(e.release(), insets); };
}

// ---- modifiers: change the node they are handed -------------------------------------------
//
// Order matters. A Box reads the spec of its direct children only, so
//   text("x") | border | flex   grows the bordered frame, border and all
//   text("x") | flex | border   marks the text inside, and the frame around it stays put

inline Element flex(Element e)
{
    e->layout.grow = 1.0f;
    return e;
}

inline Decorator grow(float weight)
{
    return [weight](Element e) {
        e->layout.grow = weight;
        return e;
    };
}

inline Decorator width(float w)
{
    return [w](Element e) {
        e->layout.width = w;
        return e;
    };
}

inline Decorator height(float h)
{
    return [h](Element e) {
        e->layout.height = h;
        return e;
    };
}

// Box only.
inline Decorator gap(float g)
{
    return [g](Element e) {
        auto* b = dynamic_cast<Box*>(e.get());
        assert(b && "gap() applies to hbox / vbox");
        if (b)
            b->gap = g;
        return e;
    };
}

// Box only.
inline Decorator align(Align a)
{
    return [a](Element e) {
        auto* b = dynamic_cast<Box*>(e.get());
        assert(b && "align() applies to hbox / vbox");
        if (b)
            b->align = a;
        return e;
    };
}

// Keeps a pointer to the node for later -- the tree is built once and retained, so it stays
// valid for as long as the App holds the tree. Put it before any wrapper:
//   text("0") | grab(counter) | border     counter is the Text
//   text("0") | border | grab(counter)     counter would be the Frame, and the cast fails
template<std::derived_from<Node> T>
Decorator grab(T*& slot)
{
    return [&slot](Element e) {
        slot = dynamic_cast<T*>(e.get());
        assert(slot && "grab() target has a different type -- is it after a wrapper?");
        return e;
    };
}
} // namespace Dsl
} // namespace Utils::Terminal::Claude
