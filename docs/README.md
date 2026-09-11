# Terminal widget architecture

A design for the `Utils::Terminal` widget layer: one opt-in interface per kind of event,
callbacks or overrides to react to them, and a repaint that the widgets ask for instead of
the main loop guessing.

Diagrams (PlantUML source beside the rendered PNGs):

| diagram | shows |
|---|---|
| [interfaces.png](interfaces.png) — [source](interfaces.puml) | the mixin catalogue and the shape they all share |
| [widgets.png](widgets.png) — [source](widgets.puml) | which widget implements which interface |
| [invalidation.png](invalidation.png) — [source](invalidation.puml) | one mouse report, end to end |
| [layers.png](layers.png) — [source](layers.puml) | events go down, invalidation goes up |

Regenerate after editing a `.puml`:

```sh
distrobox enter puml -- plantuml -charset UTF-8 -tpng -o . docs/*.puml
```

## One interface per event

Every interface has the same three parts:

```cpp
struct OnHover
{
    bool isHovered = false;                 // state   — read it in render()
    Callback onEnter, onLeave;              // compose — assign a lambda
protected:
    virtual void onHoverChanged(bool) {}    // inherit — override the hook
};
```

State, because a widget usually wants to *draw* differently, not *do* something. Callbacks,
so a caller can attach behaviour without subclassing. A protected hook, so a subclass can add
behaviour without stealing the callback. The existing `OnEvent` and `OnHover` already work this
way; `OnClick`, `OnDrag`, `OnScroll`, `OnKey` and `OnFocus` follow it.

```cpp
struct PlainLabel : Label {};                       // never hovers, never clicks

struct LitLabel : Label, Iface::OnHover             // hovers
{
    using Label::Label;
    void render(Helper::TerminalManipulation& term) override
    {
        if (isHovered) term.underline();
        Label::render(term);
        if (isHovered) term.noUnderline();
    }
};
```

`OnFocus` moves `hasFocus` off `Renderable`, where it currently sits for every widget whether
or not it can be focused.

## Implementing an interface is how you subscribe

There is no separate subscription list to keep in sync — the set of interfaces a widget
implements *is* what it listens to. `Screen::add` works the mask out once, when the widget is
added:

```cpp
Capabilities caps = Capabilities::of(widget);   // one dynamic_cast per interface, once
```

Routing then tests a bitmask. No RTTI per event, and a container whose whole subtree ignores
the mouse is skipped in one test.

It also gives `pointer-events: none` for free: a `Label` implements no mouse interface, so it
is never hit-tested and a click passes through to whatever is behind it. That needs
`hittable()` split from `focusable()` — today the quadtree indexes only focusables, so a
widget must be Tab-focusable just to be hoverable.

## Repaint is a signal, not an event

A hovered button needs to redraw. The tempting move is a `RenderEvent` through the same
dispatcher, but events and repaints travel in opposite directions:

|  | events | invalidation |
|---|---|---|
| direction | Screen → widget | widget → Screen |
| carries | input | nothing but "I changed" |
| consumable | yes, `handled` stops it | no — swallowing it loses a repaint |
| fan | one target | one path to the root |

So invalidation is its own channel, two small interfaces:

```cpp
struct Invalidatable { Invalidator* owner; void invalidate() { if (owner) owner->invalidate(*this); } };
struct Invalidator   { virtual void invalidate(Renderable&) = 0; };   // Container, Screen
```

`Container` forwards up; `Screen` collects into a dirty set. **Each mixin's setter calls
`invalidate()` itself**, so `setHovered(true)` repaints the button with no widget code at all —
a widget only calls `invalidate()` by hand when it changes something of its own (a label's
text, a counter).

## Why it's worth doing

The main loop currently re-renders everything after every event:

```cpp
terminal.setCallback([&](Event& e) { screen.onEvent(e); screen.render(term); });
```

Mouse mode `1003` reports every cell the cursor crosses, so that is a full clear-and-redraw
per cell — the flicker. With a dirty set, motion *inside* one widget repaints nothing, and
crossing a boundary repaints two widgets. The render pass drains the dirty set once per input
burst, so several events in one `read()` cost one repaint and one flush.

## Order of work

1. `hittable()` split from `focusable()`, and a second quadtree (or one tree plus the mask)
   for hit-testing. Everything else depends on it.
2. `Invalidatable` / `Invalidator` + the dirty set, with `Screen::render` drawing only dirty
   widgets. Fixes the flicker on its own.
3. `OnClick`, `OnFocus`, then `OnKey` / `OnScroll` / `OnDrag` — moving `hasFocus` out of
   `Renderable` and the mouse handling out of `Button::onEvent` as each lands.
4. `Capabilities` mask, replacing the per-event `dynamic_cast` in `HoverTracker`.

Steps 1 and 2 stand alone; nothing after them is required for the layer to work.
