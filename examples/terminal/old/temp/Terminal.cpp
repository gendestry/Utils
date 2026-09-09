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

    // Read one character at a time.
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    term.showCursor();
    term.flush();

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    term.showCursor();
}

std::pair<int, int> Terminal::getSize() {
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
void Terminal::handleBackspace(std::string &input)
{
}

void Terminal::handleArrowLeft()
{
    term.moveCursorLeft();
    term.flush();
}
void Terminal::handleArrowRight(std::string &input)
{
    term.moveCursorRight();
    term.flush();
}

// void Terminal::handleTab(std::string &input) { acceptSuggestion(input); }

void Terminal::handleArrowUp(std::string &input)
{
    term.moveCursorUp();
    draw(input);
}
void Terminal::handleArrowDown(std::string &input)
{
    term.moveCursorDown();
    draw(input);
}

void Terminal::readInput()
{
    std::string input = "";


    while (true)
    {

        auto opt = readNext();
        if (!opt.has_value())
        {
            // m_exitCallack();
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
                // m_exitCallack();
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

            if (esc == Escape::TAB)
            {
                // handleTab(input);
            }

            continue;
        }

        // input.insert(input.begin() + cursor.x, c);
        // cursor.x++;
        draw(input);
    }
}

void Terminal::draw(const std::string &input)
{
    // term.carriageReturn();
    // term.clearLine();

    // std::string suggestion;
    //
    // if (auto match = currentSuggestion(input))
    //     suggestion = match->substr(input.size());
    //
    // std::cout << input << Utils::Font::colorDim << suggestion << Utils::Font::colorReset;
    //
    // const size_t charsAfterCursor = suggestion.size() + (input.size() - cursor.x);
    //
    // if (charsAfterCursor > 0)
    //     term.moveCursorLeft(charsAfterCursor);

    term.flush();
}