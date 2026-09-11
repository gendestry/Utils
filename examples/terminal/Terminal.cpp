#include "Terminal.h"
#include "Utils/Colors/Font.h"
#include <sys/ioctl.h>
#include <unistd.h>

// using namespace Utils::Terminal;

// Terminal::Terminal(std::function<void()> exitCallback) : m_exitCallack(std::move(exitCallback))
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

    term.showCursor();
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

std::unique_ptr<Event> Terminal::makeEvent(Escape esc)
{
    switch (esc)
    {
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

void Terminal::handleEnter(std::string &input)
{
    // history.push(input);

    // std::cout << '\n';
    // std::cout.flush();
    //
    // if (m_onSubmitCallback)
    //     m_onSubmitCallback(input);
    //
    // cursor.x = 0;
    // input.clear();
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