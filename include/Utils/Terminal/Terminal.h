#pragma once
#include "Helper/TerminalManipulation.h"
#include "Interfaces/OnEvent.h"

#include <memory>
#include <optional>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <utility>

namespace Utils::Terminal
{
class Terminal : public Iface::OnEvent
{
    termios original{};
    Helper::TerminalManipulation term;
    bool reading = true;

    enum class Escape
    {
        CTRL_C,
        CTRL_D,
        ENTER,
        BACKSPACE,
        ARROW_UP,
        ARROW_DOWN,
        ARROW_LEFT,
        ARROW_RIGHT,
        CTRL_ARROW_LEFT,
        CTRL_ARROW_RIGHT,
        TAB,
        MOUSE
    };

    // A mouse report carries a position and a button, which Escape can't hold, so the parser
    // builds the event itself and leaves it here for makeEvent() to hand over.
    std::unique_ptr<Events::Event> pendingMouse;

    std::optional<char> readNext();
    std::optional<Escape> isEscapeCharacter(char in);
    std::unique_ptr<Events::Event> readSgrMouse();
    std::unique_ptr<Events::Event> makeEvent(Escape esc);

    void handleEnter(std::string &input);

public:
    Terminal();
    ~Terminal()
    {
        term.disableMouse();
        term.flush();
        tcsetattr(STDIN_FILENO, TCSANOW, &original);
    }

    static std::pair<int, int> getSize();
    Helper::TerminalManipulation &manipulate(){return term;}

    void exit() { reading = false; }
    void readInput();
    void draw(const std::string &input);
}; // namespace Utils::Terminal
}