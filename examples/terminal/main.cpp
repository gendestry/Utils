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
#if 0
class Application : public Iface::OnEvent
{
    Terminal& m_terminal;
    std::vector<std::unique_ptr<Iface::Renderable>> widgets;
    Utils::Quadtree<Iface::Renderable> quadtree;

public:
    Application(Terminal& terminal)
    : m_terminal(terminal), quadtree({0,0, (float)terminal.getSize().first, (float)terminal.getSize().second})
    {

    }

    template<typename T, typename... Args>
    T& add(Args&&... args)
    {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *widget;
        widgets.push_back(std::move(widget));

        if (ref.focusable())
        {
            // focusables.insert(&ref);
            // if (!focused)
            //     focus(&ref);
        }

        return ref;
    }

    void onEvent(Events::Event & e) override
    {
        std::cout << e.toString() << std::endl;
        Iface::OnEvent::onEvent(e);   // let the callback see it
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

        // if (focused && focused->placeCursor(term))
        //     term.showCursor();
    }
};

int main()
{
    Terminal terminal;
    Application app(terminal);
    auto& label = app.add<Label>(0,0,10,0);
    label.text = "Label: 0";

    terminal.setCallback([&](Events::Event & e)
    {
        app.onEvent(e);
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
#endif