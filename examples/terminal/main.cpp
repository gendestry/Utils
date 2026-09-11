//
// Created by bobi on 8. 9. 26.
//

#include "Utils/Storage/Quadtree.h"
#include "Utils/Terminal/Components/Button.h"
#include "Utils/Terminal/Components/Checklist.h"
#include "Utils/Terminal/Components/Group.h"
#include "Utils/Terminal/Components/InputText.h"
#include "Utils/Terminal/Components/Label.h"
#include "Utils/Terminal/Events/MouseEvent.h"
#include "Utils/Terminal/Terminal.h"
#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace Utils::Terminal;
using namespace Utils::Terminal::Components;
using namespace Utils::Terminal::Events;

// Owns the widgets. Events go to the focused widget first; Tab and the arrow keys move focus;
// anything still unhandled goes to the Screen's own callback.
//
// Focusable widgets are also indexed in a quadtree, which picks the neighbour an arrow key
// points at. The tree reads a widget's bounds, so a widget that moves or resizes has to be
// removed from `focusables` first and inserted again afterwards.
class Screen : public Iface::OnEvent
{
    std::vector<std::unique_ptr<Iface::Renderable>> widgets;
    Utils::Quadtree<Iface::Renderable> focusables;
    Iface::Renderable* focused = nullptr;

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
    explicit Screen(const Utils::Maths::Rectangle& bounds) : focusables(bounds) {}

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

    void onEvent(Event& e) override
    {
        // Mouse events go by position, not focus: whatever sits under the cursor gets them,
        // and a press moves focus there. Only focusables are in the quadtree, so a click on
        // a plain Label falls through to the Screen's own callback.
        if (e.IsInCategory(EventCategoryMouse))
        {
            auto& me = static_cast<MouseEvent&>(e);
            auto under = focusables.query(Utils::Maths::Point{float(me.x()), float(me.y())});
            Iface::Renderable* target = under.empty() ? nullptr : under.back();

            if (target)
            {
                if (e.getEventType() == EventType::MOUSE_PRESSED)
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
            EventDispatcher d(e);
            d.dispatch<EventTab>([&](EventTab&)
            {
                focusNext();
                return true;
            });
            d.dispatch<EventArrowLeft>([&](EventArrowLeft&) { return focusTowards(focusables.left(focused)); });
            d.dispatch<EventArrowRight>([&](EventArrowRight&) { return focusTowards(focusables.right(focused)); });
            d.dispatch<EventArrowUp>([&](EventArrowUp&) { return focusTowards(focusables.up(focused)); });
            d.dispatch<EventArrowDown>([&](EventArrowDown&) { return focusTowards(focusables.down(focused)); });
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

int main()
{
    Terminal terminal;

    // Terminal rows and columns are 1-based; fall back to a generous area if the size is unknown.
    auto [rows, cols] = Terminal::getSize();
    Screen screen({0, 0, float(std::max(cols, 200) + 1), float(std::max(rows, 100) + 1)});

    screen.add<Label>(1, 1, 6, 1).text = "Name: ";
    auto& nameInput = screen.add<InputText>(7, 1, 30, 1);

    auto& quit = screen.add<Button>(23, 3, "Quit");

    // The counter row is a group: its children are placed relative to it, and it passes events
    // down to them, so Left and Right move between the two buttons inside the border.
    auto& counterRow = screen.add<Group>(2, 5, 22, 1);
    counterRow.border = Helper::Border{};
    auto& decrement = counterRow.add<Button>(0, 0, "-");
    auto& counter = counterRow.add<Label>(6, 0, 10, 1);
    auto& increment = counterRow.add<Button>(17, 0, "+");

    // Shifted in by one cell so the border has room.
    auto& todo = screen.add<Checklist>(2, 8, std::vector<std::string>{"Milk", "Eggs", "Bread"});
    todo.border = Helper::Border{};

    auto& showSelected = screen.add<Button>(1, 13, "Show selected");

    auto& status = screen.add<Label>(1, 15, 40, 1);

    int count = 0;
    counter.text = "Count: 0";

    nameInput.onSubmit = [&](const std::string& value) { status.text = "Submitted: " + value; };
    quit.onPress = [&] { terminal.exit(); };
    decrement.onPress = [&] { counter.text = "Count: " + std::to_string(--count); };
    increment.onPress = [&] { counter.text = "Count: " + std::to_string(++count); };
    auto printSelected = [&]
    {
        std::string selected;
        for (const Checklist::Item& item : todo.items)
        {
            if (!item.checked)
                continue;
            if (!selected.empty())
                selected += ", ";
            selected += item.text;
        }
        status.text = "Selected: " + (selected.empty() ? "nothing" : selected);
    };
    todo.onChange = [&](size_t, const Checklist::Item&) { printSelected(); };
    showSelected.onPress = printSelected;

    // Events no widget handled.
    screen.setCallback([&](Event& e)
    {
        EventDispatcher d(e);
        d.dispatch<EventCtrlC>([&](EventCtrlC&)
        {
            terminal.exit();
            return true;
        });
    });

    // Mouse position and the last event of any kind. Hooked on the Terminal rather than the
    // Screen so it sees every event, not just the ones no widget handled.
    auto& mousePos = screen.add<Label>(1, 17, 60, 1);
    auto& lastEvent = screen.add<Label>(1, 18, 60, 1);
    mousePos.text = "Mouse: -";
    lastEvent.text = "Event: -";

    terminal.setCallback([&](Event& e)
    {
        if (e.IsInCategory(EventCategoryMouse))
        {
            auto& me = static_cast<MouseEvent&>(e);
            std::string mods;
            if (me.mods().ctrl)  mods += " +ctrl";
            if (me.mods().alt)   mods += " +alt";
            if (me.mods().shift) mods += " +shift";
            mousePos.text = "Mouse: x=" + std::to_string(me.x()) + " y=" + std::to_string(me.y()) + mods;
        }
        lastEvent.text = "Event: " + e.toString();

        screen.onEvent(e);
        screen.render(terminal.manipulate());
    });

    screen.render(terminal.manipulate());
    terminal.manipulate().flush();
    terminal.readInput();

    terminal.manipulate().clearScreenAndMoveHome();
    terminal.manipulate().showCursor();
    terminal.manipulate().flush();
    return 0;
}
