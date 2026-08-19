#pragma once
#include "History.h"
#include "TerminalManipulation.h"
// #include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <termios.h>
#include <unistd.h>

namespace Utils::Terminal
{
class Terminal
{
  private:
    struct CursorPos
    {
        uint16_t x = 0;
        uint16_t y = 0;
    };

    termios original{};
    CursorPos cursor;
    TerminalManipulation term;
    History history;

    std::function<void()> m_exitCallack;

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
        CTRL_ARROW_RIGHT
    };

    std::optional<char> readNext();

    std::optional<Escape> isEscapeCharacter(char in);

    void handleEnter(std::string &input);
    void handleBackspace(std::string &input);

    void handleArrowLeft();
    void handleArrowRight(std::string &input);
    void handleArrowUp(std::string &input);
    void handleArrowDown(std::string &input);

  public:
    Terminal(std::function<void()> exitCallback);
    ~Terminal() { tcsetattr(STDIN_FILENO, TCSANOW, &original); }

    void readInput()
    {
        std::string input;

        while (true)
        {

            auto opt = readNext();
            if (!opt.has_value())
            {
                m_exitCallack();
                return;
            }

            char c = opt.value();

            auto escapeOpt = isEscapeCharacter(c);
            if (escapeOpt.has_value())
            {
                // escape
                Escape esc = escapeOpt.value();
                if (esc == Escape::CTRL_C || esc == Escape::CTRL_D)
                {
                    // std::cout << "\r\033[2K\n";
                    m_exitCallack();
                    return;
                }

                if (esc == Escape::ENTER)
                {
                    handleEnter(input);
                }

                if (esc == Escape::BACKSPACE)
                {
                    handleBackspace(input);
                }

                if (esc == Escape::ARROW_LEFT)
                {
                    handleArrowLeft();
                }

                if (esc == Escape::ARROW_RIGHT)
                {
                    handleArrowRight(input);
                }

                if (esc == Escape::ARROW_UP)
                {
                    handleArrowUp(input);
                }

                if (esc == Escape::ARROW_DOWN)
                {
                    handleArrowDown(input);
                }

                continue;
            }

            input.insert(input.begin() + cursor.x, c);
            cursor.x++;
            draw(input);
        }
    }

    void draw(const std::string &input);
};
}; // namespace Utils::Terminal