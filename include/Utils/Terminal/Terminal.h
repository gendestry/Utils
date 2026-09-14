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

    std::optional<char> readNext();
    // Waits up to `timeoutMs` for input; used to tell a lone ESC from the start of a sequence.
    bool inputPending(int timeoutMs);

    // `extra` holds modifier bits already known from the prefix (ESC before a key means Alt).
    std::unique_ptr<Events::Event> readKey(char c, uint8_t extra = 0);
    std::unique_ptr<Events::Event> readCsi(uint8_t extra);
    std::unique_ptr<Events::Event> readSs3(uint8_t extra);
    std::unique_ptr<Events::Event> readSgrMouse();

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