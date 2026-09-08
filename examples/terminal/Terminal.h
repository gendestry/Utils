#pragma once
// #include "History.h"
#include "TerminalManipulation.h"
// #include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <termios.h>
#include <unistd.h>

struct OnEvents
{
    virtual void onHover(std::function<void()> callback) = 0;
    virtual void onEnter(std::function<void()> callback) = 0;
};

struct Renderable
{
    uint16_t x = 0, y = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    Renderable(uint16_t x, uint16_t y, uint16_t width, uint16_t height) : x(x), y(y), width(width), height(height) {}

    virtual void render() = 0;
};

struct Button : public Renderable
{
    std::string text;

    Button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, std::string text)
        : Renderable(x, y, width, height), text(text)
    {}

    void render() override
    {
        std::cout << text;
    };
};

class Terminal
{
    termios original{};
    std::vector<Renderable*> renderables;
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
    // Terminal(std::function<void()> exitCallback);
    ~Terminal() { tcsetattr(STDIN_FILENO, TCSANOW, &original); }

    std::pair<int, int> getSize();

    void addRenderable(Renderable *renderable)
    {
        renderables.push_back(renderable);
    }

    void render()
    {
        term.clearScreenAndMoveHome();
        for (auto it = renderables.begin(); it != renderables.end(); ++it)
        {
            term.moveCursorToPosition((*it)->y, (*it)->x);
            (*it)->render();
        }
        term.flush();
    }


    void readInput();
    void draw(const std::string &input);
}; // namespace Utils::Terminal