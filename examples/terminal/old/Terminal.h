#pragma once
// #include "History.h"
#include "KeyEvent.h"
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

// Anything that occupies a rectangle of the screen. Bounds are in cells, with the
// origin at the top-left; Rectangle's half-open bounds mean a widget at x=0 w=10
// owns columns 0..9 and the neighbour starting at x=10 never overlaps it.
struct Renderable
{
    Utils::Maths::Rectangle bounds;

    Renderable(float x, float y, float width, float height) : bounds(x, y, width, height) {}
    explicit Renderable(const Utils::Maths::Rectangle &bounds) : bounds(bounds) {}
    virtual ~Renderable() = default;

    // The cursor is already parked at the widget's top-left corner when this is called.
    virtual void render(TerminalManipulation &term) = 0;
};

struct Button : public Renderable
{
    std::string text;

    Button(float x, float y, float width, float height, std::string text)
        : Renderable(x, y, width, height), text(std::move(text))
    {}

    void render(TerminalManipulation &term) override
    {
        if (m_hovered)
            term.reverse();
        std::cout << text;
        if (m_hovered)
            term.noReverse();
    }

    void setHovered(bool hovered) { m_hovered = hovered; }

  private:
    bool m_hovered = false;
};

class Terminal
{
    termios original{};
    TerminalManipulation term;

    // Terminal owns the widgets; the quadtree indexes them by position and only ever
    // holds non-owning observers, so it must be rebuilt whenever bounds change.
    std::vector<std::unique_ptr<Renderable>> m_renderables;
    Utils::Quadtree<Renderable *> m_index;

    // The buffer the handlers edit. It has to be a member: dispatch fixes every handler
    // signature to bool(T&), so there is no longer a parameter to thread it through.
    std::string m_input;
    bool m_running = true;

    std::optional<char> readNext();

    // Returning true marks the event handled, which is what Event::handled is for.
    bool onChar(CharEvent &e);
    bool onEnter(EnterEvent &e);
    bool onBackspace(BackspaceEvent &e);
    bool onTab(TabEvent &e);
    bool onArrowLeft(ArrowLeftEvent &e);
    bool onArrowRight(ArrowRightEvent &e);
    bool onArrowUp(ArrowUpEvent &e);
    bool onArrowDown(ArrowDownEvent &e);
    bool onQuit(Event &e);

  public:
    Terminal();
    ~Terminal() { tcsetattr(STDIN_FILENO, TCSANOW, &original); }

    // Doesn't touch any member, so it is callable before the object is fully built --
    // which is what lets the quadtree boundary be set up in the constructor.
    static std::pair<int, int> getSize();

    // Screen rectangle in cells. getSize() reports {rows, cols}, so width comes second.
    // It reports 0x0 when stdout is not a sized tty (piped, redirected, CI); falling back
    // to a conventional 80x24 keeps the index from collapsing to an empty boundary, which
    // would make every insert fail and every query come back empty.
    static Utils::Maths::Rectangle screenBounds()
    {
        const auto [rows, cols] = getSize();
        const float width = cols > 0 ? static_cast<float>(cols) : 80.0f;
        const float height = rows > 0 ? static_cast<float>(rows) : 24.0f;
        return {0.0f, 0.0f, width, height};
    }

    // Constructs a widget in place, takes ownership, and indexes it.
    template <typename T, typename... Args> T *add(Args &&...args)
    {
        auto owned = std::make_unique<T>(std::forward<Args>(args)...);
        T *raw = owned.get();
        m_renderables.push_back(std::move(owned));

        // A widget reaching outside the current boundary cannot be indexed; rebuilding
        // grows the boundary to cover it, so nothing is ever silently left unindexed.
        if (!m_index.insert(raw->bounds, static_cast<Renderable *>(raw)))
            reindex();

        return raw;
    }

    // Topmost widget covering `p`, or nullptr. Later-added widgets win ties.
    Renderable *hitTest(Utils::Maths::Point p) const;

    // Widgets overlapping `area` -- the set that a damaged region needs redrawn.
    std::vector<Renderable *> renderablesIn(const Utils::Maths::Rectangle &area) const;

    // Rebuild the index against the current screen size; call after a resize or after
    // mutating any widget's bounds.
    void reindex();

    void render();

    // Decodes one byte into an event, pulling any further bytes an escape sequence needs
    // straight from stdin. Null when the sequence is malformed or truncated.
    std::unique_ptr<Event> parseEvent(char in);

    // Routes one event to its handler. The Application::OnEvent analogue.
    void onEvent(Event &e);

    void readInput();
    void draw(const std::string &input);
}; // namespace Utils::Terminal