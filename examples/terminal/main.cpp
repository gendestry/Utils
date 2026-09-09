//
// Created by bobi on 8. 9. 26.
//

#include "Terminal.h"
#include <iostream>

int main()
{
    Terminal terminal;

    const auto [rows, cols] = Terminal::getSize();
    std::cout << std::format("screen: {} cols x {} rows\r\n", cols, rows);

    terminal.add<Button>(2.0f, 3.0f, 9.0f, 1.0f, "[ open ]");
    terminal.add<Button>(2.0f, 5.0f, 9.0f, 1.0f, "[ save ]");
    terminal.add<Button>(20.0f, 3.0f, 9.0f, 1.0f, "[ quit ]");

    // Point query: what sits under a given cell.
    for (const Utils::Maths::Point probe : {Utils::Maths::Point(4.0f, 3.0f), Utils::Maths::Point(21.0f, 3.0f),
                                            Utils::Maths::Point(50.0f, 10.0f)})
    {
        Renderable *hit = terminal.hitTest(probe);
        std::cout << std::format("hit ({}, {}) -> {}\r\n", probe.x, probe.y,
                                 hit ? static_cast<Button *>(hit)->text : "nothing");
    }

    // Region query: everything a damaged rectangle would force a redraw of.
    const Utils::Maths::Rectangle damaged(0.0f, 0.0f, 15.0f, 6.0f);
    std::cout << std::format("{} widget(s) intersect the damaged region\r\n",
                             terminal.renderablesIn(damaged).size());

    terminal.render();
    std::cout << "\r\n";

    return 0;
}
