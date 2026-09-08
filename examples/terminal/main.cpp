//
// Created by bobi on 8. 9. 26.
//

#include "Terminal.h"
#include <iostream>

int main()
{
    Terminal terminal;
    Renderable* a = new Button(2,3,0,0,"yrd");
    Renderable* b = new Button(5,7,0,0,"asdasdsad");
    // std::cout << "Terminal" << std::endl;
    terminal.addRenderable(a);
    terminal.addRenderable(b);
    terminal.render();
    // terminal.addR(Button{2,3,0,0,"yrd"});
    // auto s = terminal.getSize();
    // std::cout << std::format("{},{}", s.first, s.second) << std::endl;
    // terminal.readInput();
    return 0;
}