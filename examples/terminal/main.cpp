//
// Created by bobi on 8. 9. 26.
//

// #include "../../include/Utils/Terminal/old/Screen.h"
#include "Utils/Storage/Quadtree.h"
#include "Utils/Terminal/Components/Button.h"
#include "Utils/Terminal/Components/Checklist.h"
#include "Utils/Terminal/Components/InputText.h"
#include "Utils/Terminal/Components/Label.h"
#include "Utils/Terminal/Events/MouseEvent.h"
#include "Utils/Terminal/Terminal.h"
#include "Utils/Terminal/TerminalApplication.h"
#include "Utils/Terminal/old/NavContainer.h"
#include "Utils/Terminal/old/Screen.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace Utils::Terminal;
using namespace Utils::Terminal::Components;
using namespace Utils::Terminal::Events;

// 1: the Claude layout layer (include/Utils/Terminal/Claude). 0: the demos below, unchanged.
#define CLAUDE_LAYOUT_DEMO 0

#if CLAUDE_LAYOUT_DEMO
#include "Utils/Terminal/Claude/App.h"

int main()
{
    namespace ui = Utils::Terminal::Claude;
    using namespace Utils::Terminal::Claude::Dsl;

    Terminal terminal;

    // Retained tree: grab() keeps pointers into it, and they stay valid while the App holds it.
    ui::Text* counter = nullptr;
    ui::Text* status = nullptr;
    ui::Text* lastEvent = nullptr;
    int count = 0;

    auto document = vbox({
        hbox({
            text("one") | border,
            text("two") | border | flex,
            text("three") | border | flex,
        }),

        hbox({
            button("-", [&] {
                counter->set("Count: " + std::to_string(--count));
                status->set("decremented");
            }),
            text("Count: 0") | grab(counter) | flex,
            button("+", [&] {
                counter->set("Count: " + std::to_string(++count));
                status->set("incremented");
            }),
        }) | gap(1) | padding(ui::Insets{0, 1, 0, 1}) | border,

        hbox({
            button("Wide button", [&] { status->set("wide"); }) | flex,
            filler(),
            button("Quit", [&] { terminal.exit(); }),
        }) | borderStyled(ui::Stroke::Double),

        text("-") | grab(status) | padding(ui::Insets{0, 0, 0, 1}),
        text("-") | grab(lastEvent) | padding(ui::Insets{0, 0, 0, 1}),
        text("Tab / arrows / mouse, Enter or Space to press, Ctrl-C to quit") | padding(ui::Insets{0, 0, 0, 1}),
    });

    ui::App app(terminal, std::move(document));

    app.tap([&](Event& e) { lastEvent->set("Event: " + e.toString()); });

    app.setCallback([&](Event& e) {
        EventDispatcher d(e);
        d.dispatch<EventCtrlC>([&](EventCtrlC&) {
            terminal.exit();
            return true;
        });
    });

    terminal.setCallback([&](Event& e) {
        app.onEvent(e);
        app.render();
    });

    app.render();
    terminal.readInput();

    terminal.manipulate().clearScreenAndMoveHome();
    terminal.manipulate().showCursor();
    terminal.manipulate().flush();
    return 0;
}
#elif 1

namespace Temp
{
class Container : public Iface::Renderable
{
public:
    explicit Container(const Rectangle& bounds)
        : Renderable(bounds.pos, bounds.width, bounds.height), focusables(bounds)
    {
    }

    Container(float x, float y, float width, float height)
        : Container(Rectangle{x, y, width, height})
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
        static_cast<Rectangle&>(ref) = ref.translated(pos);
        children.push_back(std::move(child));

        if (ref.focusable())
        {
            focusables.insert(&ref);
            if (!focusedChild)
                focusedChild = &ref;
        }

        return ref;
    }

    void render(Helper::TerminalManipulation& term) override
    {}
};
}

int main()
{
    Terminal terminal;
    Application app(terminal);
    auto [rows, cols] = Terminal::getSize();
    // auto& c = app.add<Temp::Container>(2,2,cols-2, rows-2);
    // c.border = Helper::Border{};
    auto& label = app.add<Label>(0,2);
    label.text = "Label: 0";
    // // auto& counterRow = app.add<NavContainer>(2, 5, 22, 1);
    // // counterRow.border = Helper::Border{};
    auto& decrement = app.add<Button>(0, 0, "-");
    auto& counter = app.add<Label>(6, 0);
    auto& increment = app.add<Button>(17, 0, "+");
    // // //
    int count = 0;
    counter.text = "Count: 0";
    decrement.onPress = [&] { counter.text = "Count: " + std::to_string(--count); };
    increment.onPress = [&] { counter.text = "Count: " + std::to_string(++count); };
    auto& todo = app.add<Checklist>(2, 8, std::vector<std::string>{"Milk", "Eggs", "Bread"});


    terminal.setCallback([&](Events::Event & e)
    {
        app.onEvent(e);
        label.text = e.toString();
        app.render(terminal.manipulate());
    });

    app.setCallback([&](Event& e)
    {
        EventDispatcher d(e);
        d.dispatch<EventCtrlC>([&](EventCtrlC&)
        {
           terminal.exit();
           return true;
        });
    });

    app.render(terminal.manipulate());
    terminal.manipulate().flush();
    terminal.readInput();

    terminal.manipulate().clearScreenAndMoveHome();
    terminal.manipulate().showCursor();
    terminal.manipulate().flush();
}
#else
int main()
{
    Terminal terminal;

    // Terminal rows and columns are 1-based; fall back to a generous area if the size is unknown.
    auto [rows, cols] = Terminal::getSize();
    Screen screen({0, 0, float(std::max(cols, 200) + 1), float(std::max(rows, 100) + 1)});

    auto& counterRow = screen.add<NavContainer>(2, 5, 22, 1);
    counterRow.border = Helper::Border{};
    auto& decrement = counterRow.add<Button>(0, 0, "-");
    auto& counter = counterRow.add<Label>(6, 0, 10, 1);
    auto& increment = counterRow.add<Button>(17, 0, "+");

    int count = 0;
    counter.text = "Count: 0";
    decrement.onPress = [&] { counter.text = "Count: " + std::to_string(--count); };
    increment.onPress = [&] { counter.text = "Count: " + std::to_string(++count); };

    terminal.setCallback([&](Event& e)
    {
        screen.onEvent(e);
        screen.render(terminal.manipulate());
    });
    screen.setCallback([&](Event& e)
    {
        EventDispatcher d(e);
        d.dispatch<EventCtrlC>([&](EventCtrlC&)
        {
            terminal.exit();
            return true;
        });
    });
    screen.add<Label>(1, 1, 6, 1).text = "Name: ";
    auto& nameInput = screen.add<InputText>(7, 1, 30, 1);

    auto& quit = screen.add<Button>(23, 3, "Quit");

    // The counter row is a nav container: its children are placed relative to it, and it passes events
    // down to them, so Left and Right move between the two buttons inside the border.
    // auto& counterRow = screen.add<NavContainer>(2, 5, 22, 1);
    // counterRow.border = Helper::Border{};
    // auto& decrement = counterRow.add<Button>(0, 0, "-");
    // auto& counter = counterRow.add<Label>(6, 0, 10, 1);
    // auto& increment = counterRow.add<Button>(17, 0, "+");
    //
    // // Shifted in by one cell so the border has room.
    auto& todo = screen.add<Checklist>(2, 8, std::vector<std::string>{"Milk", "Eggs", "Bread"});
    todo.border = Helper::Border{};
    //
    auto& showSelected = screen.add<Button>(1, 13, "Show selected");
    //
    auto& status = screen.add<Label>(1, 15, 40, 1);
    //
    // int count = 0;
    // counter.text = "Count: 0";
    //
    nameInput.onSubmit = [&](const std::string& value) { status.text = "Submitted: " + value; };
    quit.onPress = [&] { terminal.exit(); };
    // decrement.onPress = [&] { counter.text = "Count: " + std::to_string(--count); };
    // increment.onPress = [&] { counter.text = "Count: " + std::to_string(++count); };
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
            mousePos.text = "Mouse: x=" + std::to_string(me.x()) + " y=" + std::to_string(me.y()) + " " + me.mods().toString();
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
#endif