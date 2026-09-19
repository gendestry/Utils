#include "../../include/Utils/Terminal/Terminal.h"
#include "Utils/Colors/Font.h"
#include "Utils/Terminal/Events/MouseEvent.h"
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

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
    // turn them into SIGINT/SIGQUIT -- otherwise EventCtrlC never reaches readInput()
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

bool Terminal::inputPending(int timeoutMs)
{
    pollfd pfd{STDIN_FILENO, POLLIN, 0};
    return poll(&pfd, 1, timeoutMs) > 0;
}

namespace
{
constexpr uint8_t SHIFT = static_cast<uint8_t>(Mod::Shift);
constexpr uint8_t ALT = static_cast<uint8_t>(Mod::Alt);
constexpr uint8_t CTRL = static_cast<uint8_t>(Mod::Ctrl);

// Builds a navigation/function key event. Arrows keep their dedicated classes so existing
// handlers still see them; everything else becomes an EventKey.
std::unique_ptr<Event> makeNavKey(Key key, Mods mods)
{
    return std::make_unique<EventKey>(key, 0, mods);
}
} // namespace

std::unique_ptr<Event> Terminal::readKey(char c, uint8_t extra)
{
    if (c == 3 && extra == 0)
        return std::make_unique<EventCtrlC>();

    if (c == '\n' || c == '\r')
        return std::make_unique<EventEnter>(Mods{extra});

    if (c == 127 || c == 8)
        return std::make_unique<EventBackspace>(Mods{extra});

    if (c == '\t')
        return std::make_unique<EventTab>(Mods{extra});

    if (c == '\033')
    {
        // A lone ESC and the start of a sequence both begin with this byte; sequences arrive in
        // one burst, so if nothing follows quickly it was the Escape key itself.
        if (!inputPending(25))
            return std::make_unique<EventKey>(Key::Escape, 0, Mods{extra});

        auto next = readNext();
        if (!next)
            return nullptr;

        if (*next == '[')
            return readCsi(extra);
        if (*next == 'O')
            return readSs3(extra);

        // ESC followed by a key is how terminals send Alt+key.
        return readKey(*next, extra | ALT);
    }

    // Ctrl+letter arrives as 1..26 (Tab, Enter and Backspace were handled above).
    if (c >= 1 && c <= 26)
        return std::make_unique<EventKey>(Key::Char, char32_t('a' + c - 1), Mods{uint8_t(extra | CTRL)});

    uint8_t mods = extra;
    if (c >= 'A' && c <= 'Z')
        mods |= SHIFT;

    // With Ctrl/Alt held it's a shortcut, not text -- keep it away from EventChar consumers.
    if (mods & (ALT | CTRL))
        return std::make_unique<EventKey>(Key::Char, char32_t(static_cast<unsigned char>(c)), Mods{mods});

    return std::make_unique<EventChar>(c, Mods{mods});
}

// Called with "ESC [" already consumed. Reads "params final" where params are ';'-separated
// numbers (':' sub-params are skipped) and the final byte is in 0x40..0x7E.
std::unique_ptr<Event> Terminal::readCsi(uint8_t extra)
{
    std::vector<int> params{0};
    bool skippingSub = false;
    char final = 0;

    while (true)
    {
        auto c = readNext();
        if (!c)
            return nullptr;

        if (*c == '<' && params.size() == 1 && params[0] == 0)
            return readSgrMouse();

        if (*c >= '0' && *c <= '9')
        {
            if (!skippingSub)
                params.back() = params.back() * 10 + (*c - '0');
        }
        else if (*c == ';')
        {
            params.push_back(0);
            skippingSub = false;
        }
        else if (*c == ':')
            skippingSub = true;
        else if (*c >= 0x40 && *c <= 0x7E)
        {
            final = *c;
            break;
        }
        // Private markers ('?', '>', '=') and intermediates are ignored.
    }

    const Mods mods{uint8_t(Mods::decodeMods(params.size() > 1 ? params[1] : 1).bits | extra)};

    switch (final)
    {
    case 'A':
        return std::make_unique<EventArrowUp>(mods);
    case 'B':
        return std::make_unique<EventArrowDown>(mods);
    case 'C':
        return std::make_unique<EventArrowRight>(mods);
    case 'D':
        return std::make_unique<EventArrowLeft>(mods);
    case 'H':
        return makeNavKey(Key::Home, mods);
    case 'F':
        return makeNavKey(Key::End, mods);
    case 'P':
        return makeNavKey(Key::F1, mods);
    case 'Q':
        return makeNavKey(Key::F2, mods);
    case 'R':
        return makeNavKey(Key::F3, mods);
    case 'S':
        return makeNavKey(Key::F4, mods);
    case 'Z': // Shift+Tab
        return std::make_unique<EventTab>(Mods{uint8_t(mods.bits | SHIFT)});

    case '~':
        switch (params[0])
        {
        case 1: case 7: return makeNavKey(Key::Home, mods);
        case 2: return makeNavKey(Key::Insert, mods);
        case 3: return makeNavKey(Key::Delete, mods);
        case 4: case 8: return makeNavKey(Key::End, mods);
        case 5: return makeNavKey(Key::PageUp, mods);
        case 6: return makeNavKey(Key::PageDown, mods);
        case 11: return makeNavKey(Key::F1, mods);
        case 12: return makeNavKey(Key::F2, mods);
        case 13: return makeNavKey(Key::F3, mods);
        case 14: return makeNavKey(Key::F4, mods);
        case 15: return makeNavKey(Key::F5, mods);
        case 17: return makeNavKey(Key::F6, mods);
        case 18: return makeNavKey(Key::F7, mods);
        case 19: return makeNavKey(Key::F8, mods);
        case 20: return makeNavKey(Key::F9, mods);
        case 21: return makeNavKey(Key::F10, mods);
        case 23: return makeNavKey(Key::F11, mods);
        case 24: return makeNavKey(Key::F12, mods);
        }
        return nullptr;

    case 'u': // kitty keyboard protocol / xterm modifyOtherKeys: ESC [ code ; mods u
    {
        const char32_t code = params[0];
        switch (code)
        {
        case 13: return std::make_unique<EventEnter>(mods);
        case 9: return std::make_unique<EventTab>(mods);
        case 127: return std::make_unique<EventBackspace>(mods);
        case 27: return makeNavKey(Key::Escape, mods);
        }

        if (code == 'c' && mods == Mod::Ctrl)
            return std::make_unique<EventCtrlC>();

        if (!(mods.bits & (ALT | CTRL | static_cast<uint8_t>(Mod::Super))) && code >= 32 && code < 127)
            return std::make_unique<EventChar>(char(code), mods);

        return std::make_unique<EventKey>(Key::Char, code, mods);
    }
    }

    return nullptr; // Unknown sequence (focus reports etc.) -- drop it rather than leak it as text.
}

// Called with "ESC O" consumed: application-mode arrows/Home/End and F1-F4.
std::unique_ptr<Event> Terminal::readSs3(uint8_t extra)
{
    auto c = readNext();
    if (!c)
        return nullptr;

    const Mods mods{extra};
    switch (*c)
    {
    case 'A': return std::make_unique<EventArrowUp>(mods);
    case 'B': return std::make_unique<EventArrowDown>(mods);
    case 'C': return std::make_unique<EventArrowRight>(mods);
    case 'D': return std::make_unique<EventArrowLeft>(mods);
    case 'H': return makeNavKey(Key::Home, mods);
    case 'F': return makeNavKey(Key::End, mods);
    case 'P': return makeNavKey(Key::F1, mods);
    case 'Q': return makeNavKey(Key::F2, mods);
    case 'R': return makeNavKey(Key::F3, mods);
    case 'S': return makeNavKey(Key::F4, mods);
    }
    return nullptr;
}

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
    // SGR reports columns and rows 1-based; widget rectangles are 0-based, so normalise here
    // and every consumer can hit-test against a rect directly.
    const int x = nums[1] - 1;
    const int y = nums[2] - 1;
    const Mods mods{static_cast<uint8_t>((cb >> 2) & 7)}; // shift/alt/ctrl, same order as Mod

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

void Terminal::readInput()
{
    while (reading)
    {
        auto opt = readNext();
        if (!opt.has_value())
        {
            return;
        }

        if (auto event = readKey(*opt))
        {
            onEvent(*event);
        }

        term.flush();
    }
}