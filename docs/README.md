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
| [layout.png](layout.png) — [source](layout.puml) | containers, flex, and who owns the quadtree |
| [layout-pass.png](layout-pass.png) — [source](layout-pass.puml) | one layout pass, measure to render |
| [claude.png](claude.png) — [source](claude.puml) | the implemented layer in `include/Utils/Terminal/Claude` |

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

## Layout: measure up, arrange down

A container doesn't draw and doesn't route — it *sizes*. Three passes, deliberately kept apart:

```cpp
virtual Size measure() const;              // bottom-up, pure     — what would I like to be?
virtual void arrange(const Rectangle& r);  // top-down, writes    — here is what you get
virtual void render(TerminalManipulation&); // top-down, draws    — reads the rect, never writes
```

`Renderable` already *is* a `Rectangle`, so `arrange()` writing into `pos/width/height` is the
entire layout result — in **absolute** screen cells, with no parent-relative coordinate space.
That one choice is what keeps everything else flat: hit-testing, rendering and focus all work on
the widget's own rect without walking back up through parents.

`Box` (horizontal or vertical) distributes its main axis in four steps: base sizes from
`measure()`, leftover space split by `LayoutSpec::grow`, **snap to whole cells carrying the
remainder**, then place. The snap has to happen once per container — a terminal has no
fractional cells, and rounding each child independently turns `1fr 1fr 1fr` over 20 columns into
`6+6+6` and a hole.

## The DSL is sugar over construction, not a per-frame rebuild

```cpp
auto& row = app.add(hbox({
    button("-", [&]{ counter.text = std::to_string(--count); }),
    label(counter) | flex,
    button("+", [&]{ counter.text = std::to_string(++count); }),
}) | border);
```

`Element` is `unique_ptr<Renderable>`, and `operator|` just applies a `Decorator`. Two kinds:
**wrappers** insert a node (`border`, `color`), **modifiers** set a field on the node they are
handed (`flex` sets `spec.grow = 1`). The pipeline builds the retained tree **once** — which is
why `focused` and the quadtree may hold pointers into it. An immediate-mode library rebuilds this
every frame and therefore has to split the tree in two, ephemeral elements for layout and retained
components for state; that tax isn't worth paying for a library where the caller holds
`auto& counter` and writes to it.

## Who owns the quadtree

`Application` — the only thing that owns the root and runs the layout pass. Never a container:
rects are absolute, so a per-container index would re-partition space the parent already
partitioned.

It is also no longer the hit-test structure. Hit-testing walks the retained tree — `O(depth)`, a
dozen comparisons, correct z-order and clipping for free. What a tree walk *cannot* answer is
"nearest focusable in this direction", so the quadtree stays as the **navigation index** behind
`left/right/up/down`, rebuilt at the end of every `relayout()` (its contract requires removal
before a move, and a full rebuild is cheap at terminal sizes).

`relayout()` runs on construction, on `SIGWINCH`, and after any change to a natural size — not
per frame and not per event.

## Order of work

1. `hittable()` split from `focusable()`, and a second quadtree (or one tree plus the mask)
   for hit-testing. Everything else depends on it.
2. `Invalidatable` / `Invalidator` + the dirty set, with `Screen::render` drawing only dirty
   widgets. Fixes the flicker on its own.
3. `OnClick`, `OnFocus`, then `OnKey` / `OnScroll` / `OnDrag` — moving `hasFocus` out of
   `Renderable` and the mouse handling out of `Button::onEvent` as each lands.
4. `Capabilities` mask, replacing the per-event `dynamic_cast` in `HoverTracker`.

Steps 1 and 2 stand alone; nothing after them is required for the layer to work.
