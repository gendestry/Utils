// Bare Terminal usage: no components, no TerminalApplication -- just raw input events and
// TerminalManipulation for drawing. Move the '@' with the arrow keys or click to place it;
// the last event is shown on the top line. 'q' or Ctrl+C quits.

#include "Utils/Terminal/Events/KeyEvent.h"
#include "Utils/Terminal/Events/MouseEvent.h"
#include "Utils/Terminal/Terminal.h"

#include <algorithm>
#include <iostream>

using namespace Utils::Terminal;
using namespace Utils::Terminal::Events;

int main()
{
    Terminal terminal;
    auto &term = terminal.manipulate();

    // Rows/columns are 1-based for moveCursorToPosition; row 1 is the status line.
    auto [rows, cols] = Terminal::getSize();
    int row = std::max(2, rows / 2);
    int col = std::max(1, cols / 2);
    std::string last = "(none)";

    auto draw = [&]
    {
        term.clearScreenAndMoveHome();
        term.reverse();
        std::cout << " Last event: " << last << "  |  arrows/click to move, q to quit ";
        term.resetFormatting();
        term.moveCursorToPosition(row, col);
        term.bold();
        std::cout << '@';
        term.resetFormatting();
        term.moveCursorToPosition(row, col);
    };

    term.hideCursor();
    draw();
    term.flush();

    terminal.setCallback([&](Event &e)
    {
        std::tie(rows, cols) = Terminal::getSize();
        last = e.toString();

        EventDispatcher d(e);
        d.dispatch<EventCtrlC>([&](auto &) { terminal.exit(); return true; });
        d.dispatch<EventChar>([&](EventChar &c)
        {
            if (c.get() == 'q')
                terminal.exit();
            return true;
        });
        d.dispatch<EventArrowUp>([&](auto &) { row = std::max(2, row - 1); return true; });
        d.dispatch<EventArrowDown>([&](auto &) { row = std::min(rows, row + 1); return true; });
        d.dispatch<EventArrowLeft>([&](auto &) { col = std::max(1, col - 1); return true; });
        d.dispatch<EventArrowRight>([&](auto &) { col = std::min(cols, col + 1); return true; });
        // Mouse coordinates are 0-based.
        d.dispatch<EventMousePressed>([&](EventMousePressed &m)
        {
            row = std::max(2, m.y() + 1);
            col = m.x() + 1;
            return true;
        });

        draw(); // readInput() flushes after each event
    });

    terminal.readInput();

    term.clearScreenAndMoveHome();
    term.showCursor();
    term.flush();
}
