#pragma once
// #include "History.h"
#include "TerminalManipulation.h"
#include "Utils/Math/Rectangle.h"
#include "Utils/Storage/Quadtree.h"
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

class Terminal
{
    termios original{};
    TerminalManipulation term;

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
        TAB
    };

    std::optional<char> readNext();
    std::optional<Escape> isEscapeCharacter(char in);

    void handleEnter(std::string &input);
    void handleBackspace(std::string &input);

    void handleArrowLeft();
    void handleArrowRight(std::string &input);
    void handleArrowUp(std::string &input);
    void handleArrowDown(std::string &input);
    void handleTab(std::string &input);

  public:
    Terminal();
    ~Terminal() { tcsetattr(STDIN_FILENO, TCSANOW, &original); }

    static std::pair<int, int> getSize();

    void readInput();
    void draw(const std::string &input);
}; // namespace Utils::Terminal