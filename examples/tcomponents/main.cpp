//
// Created by bobi on 8. 9. 26.
//

#include "Utils/Terminal/Components/InputText.h"
#include "Utils/Terminal/Events/MouseEvent.h"
#include "Utils/Terminal/Terminal.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace Utils::Terminal;
using namespace Utils::Terminal::Events;


int main()
{
    Terminal terminal;
    auto [rows, cols] = Terminal::getSize();
    auto& t = terminal.manipulate();

    std::vector<std::string> preds = {"asdf", "qwerr", "basd"};

    std::string text = "";
    auto cursor = 0;

    terminal.setCallback([&](Events::Event & e)
    {
        EventDispatcher d(e);
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

    terminal.manipulate().clearScreenAndMoveHome();
    terminal.manipulate().showCursor();
    terminal.manipulate().flush();
}
