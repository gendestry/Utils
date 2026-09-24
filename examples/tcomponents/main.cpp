//
// Created by bobi on 8. 9. 26.
//

#include "Utils/Terminal/Events/MouseEvent.h"
#include "Utils/Terminal/Interfaces/Renderable.h"
#include "Utils/Terminal/Terminal.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace Utils::Terminal;
using namespace Utils::Terminal::Events;

// struct Base : public Terminal::OnEvent
// {
//     Terminal terminal_;
//     std::function<bool(char c)> onCharacter = [](char c){return false;};
//     std::function<bool()> onEnter = [](){return false;};
//     std::function<bool()> onBackspace = []() {return false;};
//     std::function<bool()> onCtrlC = []() {exit(0); return false;};
//
// public:
//     void onEvent(Events::Event &event) override
//     {
//         Events::EventDispatcher dispatcher(event);
//         dispatcher.dispatch<Events::EventChar>([&](Events::EventChar &e){return onCharacter(e.get());});
//         dispatcher.dispatch<Events::EventEnter>([&](Events::EventEnter &e){return onEnter();});
//         dispatcher.dispatch<EventBackspace>([&](EventBackspace& ) {return onBackspace();});
//         dispatcher.dispatch<EventCtrlC>([&](EventCtrlC& ) {return onCtrlC();});
//     };
// };


class Application : public Terminal::OnEvent
{
    Terminal& m_terminal;
    std::vector<std::unique_ptr<Iface::Renderable>> widgets;

public:
    Application(Terminal& terminal)
        : m_terminal(terminal)
    {}

    template<typename T, typename... Args>
    T& add(Args&&... args)
    {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *widget;
        widgets.push_back(std::move(widget));

        // if (ref.focusable())
        // {
        //     quadtree.insert(&ref);
        //     if (!focused)
        //         focus(&ref);
        // }

        return ref;
    }

    void onEvent(Events::Event & e) override
    {

        if (!e.handled)
            OnEvent::onEvent(e);
    }

    void render(Helper::TerminalManipulation& term)
    {

    }
};

int main()
{
    Terminal terminal;
    auto [rows, cols] = Terminal::getSize();
    auto& t = terminal.manipulate();

    // std::vector<std::string> preds = {"asdf", "qwerr", "basd"};

    std::string text = "";
    auto cursor = 0;

    terminal.setCallback([&](Events::Event & event)
    {
        EventDispatcher d(event);
        d.dispatch<EventChar>([&](EventChar& e)
        {
            text.insert(cursor,1, e.get());
            cursor++;
            return true;
        });
        d.dispatch<EventBackspace>([&](EventBackspace&)
        {
            if (cursor > 0)
                text.erase(--cursor, 1);
            return true;
        });
        d.dispatch<EventCtrlC>([&](EventCtrlC& e)
        {
            exit(1);
            return false;
        });

        t.moveCursorToPosition(1, 1);
        t.clearLineFromCursor();
        std::cout << text;
        t.moveCursorToPosition(1, cursor + 1);
        terminal.manipulate().flush();
    });
    terminal.manipulate().clearScreenAndMoveHome();
    terminal.manipulate().flush();
    // input.render(terminal.manipulate());

    terminal.readInput();

    // terminal.manipulate().clearScreenAndMoveHome();
    // terminal.manipulate().showCursor();
    // terminal.manipulate().flush();
}
