#pragma once
#include "Node.h"
#include "Utils/Terminal/Events/KeyEvent.h"
#include "Utils/Terminal/Events/MouseEvent.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace Utils::Terminal::Claude
{
// One line of text. Not focusable and not hittable, so clicks pass through it.
struct Text : Node
{
    explicit Text(std::string text) : m_text(std::move(text)) {}

    const std::string& get() const { return m_text; }

    // Through a setter, because the text is what measure() reads.
    void set(std::string text)
    {
        if (text.size() != m_text.size())
            markLayoutDirty();
        m_text = std::move(text);
    }

    Size measure() const override { return {float(m_text.size()), 1.0f}; }

    void render(Helper::TerminalManipulation& term) override
    {
        if (width < 1.0f || height < 1.0f)
            return;
        moveTo(term, left(), top());
        std::cout << m_text.substr(0, size_t(width));
    }

  private:
    std::string m_text;
};

// "[label]", with the label centred when flex makes the button wider than it needs.
struct Button : Node
{
    std::string label;
    std::function<void()> onPress;

    Button(std::string label, std::function<void()> onPress)
        : label(std::move(label)), onPress(std::move(onPress))
    {
    }

    bool focusable() const override { return true; }

    Size measure() const override { return {float(label.size() + 2), 1.0f}; }

    Size minimum() const override
    {
        return {std::max(layout.minSize.x, 3.0f), std::max(layout.minSize.y, 1.0f)};
    }

    void onEvent(Events::Event& e) override
    {
        using namespace Events;
        EventDispatcher d(e);
        d.dispatch<EventEnter>([&](EventEnter&) { return press(); });
        d.dispatch<EventChar>([&](EventChar& event) { return event.get() == ' ' && press(); });
        // The App already hit-tested it, so there are no bounds to re-check.
        d.dispatch<EventMousePressed>([&](EventMousePressed& event) { return event.isLeft() && press(); });

        // Unhandled events still reach a callback set on the button itself.
        if (!e.handled)
            OnEvent::onEvent(e);
    }

    void render(Helper::TerminalManipulation& term) override
    {
        const int w = int(width);
        if (w < 2 || height < 1.0f)
            return;

        const int inner = w - 2;
        std::string shown = label.substr(0, size_t(inner));
        const int pad = inner - int(shown.size());
        shown = std::string(size_t(pad / 2), ' ') + shown + std::string(size_t(pad - pad / 2), ' ');

        moveTo(term, left(), top());
        if (hasFocus)
            term.reverse();
        else if (isHovered)
            term.underline();
        std::cout << "[" << shown << "]";
        if (hasFocus)
            term.noReverse();
        else if (isHovered)
            term.noUnderline();
    }

  private:
    bool press()
    {
        if (onPress)
            onPress();
        return true;
    }
};

// Takes up space and nothing else: filler() between two nodes pushes them apart.
struct Filler : Node
{
    Filler() { layout.grow = 1.0f; }
};
} // namespace Utils::Terminal::Claude
