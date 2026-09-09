#include "Terminal.h"
#include "Utils/Colors/Font.h"
#include <sys/ioctl.h>
#include <unistd.h>

// using namespace Utils::Terminal;

// Terminal::Terminal(std::function<void()> exitCallback) : m_exitCallack(std::move(exitCallback))
Terminal::Terminal() : m_index(screenBounds())
{
    tcgetattr(STDIN_FILENO, &original);

    termios raw = original;

    // Don't wait for ENTER.
    raw.c_lflag &= ~(ICANON);

    // Don't echo typed characters.
    raw.c_lflag &= ~(ECHO);

    // Deliver ctrl-c and ctrl-\ as ordinary bytes instead of letting the line discipline
    // turn them into SIGINT/SIGQUIT -- otherwise CtrlCEvent can never reach onEvent().
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

void Terminal::reindex()
{
    // The boundary has to contain every widget, not just the screen -- a widget hanging
    // off the edge would otherwise be rejected and vanish from all queries.
    Utils::Maths::Rectangle boundary = screenBounds();
    for (const std::unique_ptr<Renderable> &r : m_renderables)
        boundary = boundary.united(r->bounds);

    m_index = Utils::Quadtree<Renderable *>(boundary);
    for (const std::unique_ptr<Renderable> &r : m_renderables)
        m_index.insert(r->bounds, r.get());
}

Renderable *Terminal::hitTest(Utils::Maths::Point p) const
{
    // The quadtree yields overlapping widgets in no particular order, so resolve the
    // tie by insertion order: the last one added is on top.
    Renderable *top = nullptr;
    size_t topIndex = 0U;

    m_index.query(p, [&](Renderable *candidate) {
        for (size_t i = 0U; i < m_renderables.size(); i++)
        {
            if (m_renderables[i].get() != candidate)
                continue;

            if (top == nullptr || i >= topIndex)
            {
                top = candidate;
                topIndex = i;
            }
            break;
        }
    });

    return top;
}

std::vector<Renderable *> Terminal::renderablesIn(const Utils::Maths::Rectangle &area) const
{
    return m_index.query(area);
}

void Terminal::render()
{
    term.clearScreenAndMoveHome();

    for (const std::unique_ptr<Renderable> &r : m_renderables)
    {
        // Escape codes are 1-based, the Rectangle is 0-based.
        term.moveCursorToPosition(static_cast<uint16_t>(r->bounds.top()) + 1U,
                                  static_cast<uint16_t>(r->bounds.left()) + 1U);
        r->render(term);
    }

    term.flush();
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

std::unique_ptr<Event> Terminal::parseEvent(char in)
{
    switch (in)
    {
    case 3:
        return std::make_unique<CtrlCEvent>();
    case 4:
        return std::make_unique<CtrlDEvent>();
    case '\n':
    case '\r':
        return std::make_unique<EnterEvent>();
    case 127:
    case 8:
        return std::make_unique<BackspaceEvent>();
    case '\t':
        return std::make_unique<TabEvent>();
    default:
        break;
    }

    if (in != '\033')
        return std::make_unique<CharEvent>(in);

    // CSI sequences: ESC [ ...
    auto c1 = readNext();
    if (!c1.has_value() || c1.value() != '[')
        return nullptr;

    auto c2 = readNext();
    if (!c2.has_value())
        return nullptr;

    switch (c2.value())
    {
    case 'A':
        return std::make_unique<ArrowUpEvent>();
    case 'B':
        return std::make_unique<ArrowDownEvent>();
    case 'C':
        return std::make_unique<ArrowRightEvent>();
    case 'D':
        return std::make_unique<ArrowLeftEvent>();
    default:
        break;
    }

    // Ctrl + Arrow: ESC [ 1 ; 5 C/D
    if (c2.value() == '1')
    {
        auto semi = readNext();
        auto modifier = readNext();
        auto direction = readNext();

        if (!semi || !modifier || !direction)
            return nullptr;

        if (*semi == ';' && *modifier == '5')
        {
            if (*direction == 'C')
                return std::make_unique<CtrlArrowRightEvent>();

            if (*direction == 'D')
                return std::make_unique<CtrlArrowLeftEvent>();
        }
    }

    return nullptr;
}

void Terminal::onEvent(Event &e)
{
    EventDispatcher dispatcher(e);

    dispatcher.dispatch<CharEvent>([this](CharEvent &ev) { return onChar(ev); });
    dispatcher.dispatch<EnterEvent>([this](EnterEvent &ev) { return onEnter(ev); });
    dispatcher.dispatch<BackspaceEvent>([this](BackspaceEvent &ev) { return onBackspace(ev); });
    dispatcher.dispatch<TabEvent>([this](TabEvent &ev) { return onTab(ev); });
    dispatcher.dispatch<ArrowLeftEvent>([this](ArrowLeftEvent &ev) { return onArrowLeft(ev); });
    dispatcher.dispatch<ArrowRightEvent>([this](ArrowRightEvent &ev) { return onArrowRight(ev); });
    dispatcher.dispatch<ArrowUpEvent>([this](ArrowUpEvent &ev) { return onArrowUp(ev); });
    dispatcher.dispatch<ArrowDownEvent>([this](ArrowDownEvent &ev) { return onArrowDown(ev); });
    dispatcher.dispatch<CtrlCEvent>([this](CtrlCEvent &ev) { return onQuit(ev); });
    dispatcher.dispatch<CtrlDEvent>([this](CtrlDEvent &ev) { return onQuit(ev); });
}

bool Terminal::onChar(CharEvent &e)
{
    m_input.push_back(e.getChar());
    draw(m_input);
    return true;
}

bool Terminal::onEnter(EnterEvent &)
{
    // history.push(m_input);
    m_input.clear();
    draw(m_input);
    return true;
}

bool Terminal::onBackspace(BackspaceEvent &)
{
    if (m_input.empty())
        return true;

    m_input.pop_back();
    draw(m_input);
    return true;
}

bool Terminal::onTab(TabEvent &)
{
    // acceptSuggestion(m_input);
    return false;
}

bool Terminal::onArrowLeft(ArrowLeftEvent &)
{
    term.moveCursorLeft();
    term.flush();
    return true;
}

bool Terminal::onArrowRight(ArrowRightEvent &)
{
    term.moveCursorRight();
    term.flush();
    return true;
}

bool Terminal::onArrowUp(ArrowUpEvent &)
{
    term.moveCursorUp();
    draw(m_input);
    return true;
}

bool Terminal::onArrowDown(ArrowDownEvent &)
{
    term.moveCursorDown();
    draw(m_input);
    return true;
}

bool Terminal::onQuit(Event &)
{
    m_running = false;
    return true;
}

void Terminal::readInput()
{
    m_input.clear();
    m_running = true;

    while (m_running)
    {
        auto opt = readNext();
        if (!opt.has_value())
            return;

        // A malformed or truncated escape sequence decodes to nothing; drop it.
        std::unique_ptr<Event> event = parseEvent(opt.value());
        if (event)
            onEvent(*event);
    }
}

void Terminal::draw(const std::string &input)
{
    term.carriageReturn();
    term.clearLine();

    std::cout << input;
    term.flush();
}
