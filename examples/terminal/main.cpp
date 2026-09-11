//
// Created by bobi on 8. 9. 26.
//

#include "Terminal.h"
#include <iostream>
#include "Utils/Math/Rectangle.h"

int main()
{
    Terminal terminal;
    terminal.setCallback([&](Event& e)
    {
        EventDispatcher d(e);
        d.dispatch<EventCtrlC>([&](Event& event)
        {
            terminal.exit();
            return true;
        });
        d.dispatch<EventArrowLeft>([&](Event& event)
        {
            terminal.manipulate().moveCursorLeft();
            return true;
        });
        d.dispatch<EventArrowRight>([&](Event& event)
        {
            terminal.manipulate().moveCursorRight();
            return true;
        });
        d.dispatch<EventArrowUp>([&](Event& event)
        {
            terminal.manipulate().moveCursorUp();
            return true;
        });
        d.dispatch<EventArrowDown>([&](Event& event)
        {
            terminal.manipulate().moveCursorDown();
            return true;
        });
        d.dispatch<EventChar>([&](EventChar& event)
        {
            std::cout << event.get();
            return true;
        });
    });
    std::cout<<"Hello World!"<<std::endl;
    terminal.readInput();


    return 0;
}
