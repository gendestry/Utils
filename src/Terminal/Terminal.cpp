#include "../../include/Utils/Terminal/Terminal.h"
#include "Utils/Colors/Font.h"
#include "Utils/Terminal/Events/MouseEvent.h"
#include <sys/ioctl.h>
#include <unistd.h>

using namespace Utils::Terminal;
using namespace Events;

Terminal::Terminal()
{
    tcgetattr(STDIN_FILENO, &original);
    termios raw = original;

    // Don't wait for ENTER.
    raw.c_lflag &= ~(ICANON);

    // Don't echo typed characters.
    raw.c_lflag &= ~(ECHO);

    // Deliver ctrl-c and ctrl-\ as ordinary bytes instead of letting the line discipline
    // turn them into SIGINT/SIGQUIT -- otherwise Escape::CTRL_C never reaches readInput()
    // and the process is killed instead.
    raw.c_lflag &= ~(ISIG);

    // Read one character at a time.
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    term.showCursor();
    term.flush();

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    term.enableMouse();
    term.showCursor();
    term.flush();
}

std::pair<int, int> Terminal::getSize()
{
    struct winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0)
        return {0, 0};

    return {ws.ws_row, ws.ws_col};
}

std::optional<char> Terminal::readNext()
{
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
    {
        return std::nullopt;
    }
    return c;
};

std::optional<Terminal::Escape> Terminal::isEscapeCharacter(char in)
{
    char c = in;
    if (c == 3)
    {
        // std::cout << "\r\033[2K\n";
        return Escape::CTRL_C;
    }

    if (c == 4)
    {
        return Escape::CTRL_D;
    }

    if (c == '\n' || c == '\r')
    {
        return Escape::ENTER;
    }

    if (c == 127 || c == 8)
    {
        return Escape::BACKSPACE;
    }

    if (c == '\t')
    {
        return Escape::TAB;
    }

    if (c == '\033')
    {
        auto c1 = readNext();
        if (!c1.has_value() || c1.value() != '[')
        {
            return std::nullopt;
        }

        auto c2 = readNext();
        if (c2.has_value())
        {
            char val = c2.value();

            // SGR mouse report: ESC [ < Cb ; Cx ; Cy M|m
            if (val == '<')
            {
                pendingMouse = readSgrMouse();
                if (!pendingMouse)
                    return std::nullopt;
                return Escape::MOUSE;
            }

            switch (val)
            {
            case 'A':
            {
                return Escape::ARROW_UP;
            }
            case 'B':
            {
                return Escape::ARROW_DOWN;
            }
            case 'C':
            {
                return Escape::ARROW_RIGHT;
            }
            case 'D':
            {
                return Escape::ARROW_LEFT;
            }
            }

            // Ctrl + Arrow: ESC [ 1 ; 5 C/D
            if (c2.value() == '1')
            {
                auto semi = readNext();
                auto modifier = readNext();
                auto direction = readNext();

                if (!semi || !modifier || !direction)
                    return std::nullopt;

                if (*semi == ';' && *modifier == '5')
                {
                    if (*direction == 'C')
                        return Escape::CTRL_ARROW_RIGHT;

                    if (*direction == 'D')
                        return Escape::CTRL_ARROW_LEFT;
                }
            }
        }
    }

    return std::nullopt;
};

// Called with "ESC [ <" already consumed. Cb is a bitfield: low two bits pick the button
// (3 = none), bit 2/3/4 are shift/alt/ctrl, bit 5 marks motion and bit 6 the scroll wheel.
std::unique_ptr<Event> Terminal::readSgrMouse()
{
    int nums[3] = {0, 0, 0};
    int i = 0;
    char final = 0;

    while (true)
    {
        auto c = readNext();
        if (!c)
            return nullptr;

        if (*c >= '0' && *c <= '9')
            nums[i] = nums[i] * 10 + (*c - '0');
        else if (*c == ';' && i < 2)
            ++i;
        else if (*c == 'M' || *c == 'm')
        {
            final = *c;
            break;
        }
        else
            return nullptr; // Malformed -- drop the sequence rather than leak it as input.
    }

    const int cb = nums[0];
    const int x = nums[1];
    const int y = nums[2];
    const Mods mods{bool(cb & 4), bool(cb & 8), bool(cb & 16), false};

    if (cb & 64)
        return std::make_unique<EventMouseScrolled>(x, y, mods, (cb & 1) ? -1 : 1);

    const MouseButton button = static_cast<MouseButton>(cb & 3);

    if (cb & 32)
    {
        if (button == MouseButton::None)
            return std::make_unique<EventMouseMoved>(x, y, mods);
        return std::make_unique<EventMouseDragged>(button, x, y, mods);
    }

    if (final == 'm')
        return std::make_unique<EventMouseReleased>(button, x, y, mods);

    return std::make_unique<EventMousePressed>(button, x, y, mods);
}

std::unique_ptr<Event> Terminal::makeEvent(Escape esc)
{
    switch (esc)
    {
    case Escape::MOUSE:
        return std::move(pendingMouse);
    case Escape::CTRL_C:
        return std::make_unique<EventCtrlC>();
    case Escape::ENTER:
        return std::make_unique<EventEnter>();
    case Escape::BACKSPACE:
        return std::make_unique<EventBackspace>();
    case Escape::TAB:
        return std::make_unique<EventTab>();
    case Escape::ARROW_UP:
        return std::make_unique<EventArrowUp>();
    case Escape::ARROW_DOWN:
        return std::make_unique<EventArrowDown>();
    case Escape::ARROW_LEFT:
        return std::make_unique<EventArrowLeft>();
    case Escape::ARROW_RIGHT:
        return std::make_unique<EventArrowRight>();
    default:
        // No Event class yet for CTRL_C, CTRL_D, CTRL_ARROW_LEFT, CTRL_ARROW_RIGHT.
        return nullptr;
    }
}

void Terminal::readInput()
{
    std::string input = "";

    while (reading)
    {
        auto opt = readNext();
        if (!opt.has_value())
        {
            return;
        }

        char c = opt.value();

        auto escapeOpt = isEscapeCharacter(c);
        if (escapeOpt.has_value())
        {
            Escape esc = escapeOpt.value();
            if (auto event = makeEvent(esc))
            {
                onEvent(*event);
            }
        }
        else
        {
            auto e = std::make_unique<EventChar>(c);
            onEvent(*e);
        }

        term.flush();
    }
}

void Terminal::draw(const std::string &input)
{
    term.flush();
}