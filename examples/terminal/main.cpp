//
// Created by bobi on 8. 9. 26.
//

#include "Utils/Terminal/Components/InputText.h"
#include "Utils/Terminal/Components/Label.h"
#include "Utils/Terminal/Terminal.h"
#include <memory>
#include <string>
#include <vector>

using namespace Utils::Terminal;
using namespace Utils::Terminal::Components;
using namespace Utils::Terminal::Events;

// Owns the widgets. Events go to the focused widget first; Tab moves focus;
// anything still unhandled goes to the Screen's own callback.
class Screen : public Iface::OnEvent
{
    std::vector<std::unique_ptr<Iface::Renderable>> widgets;
    Iface::Renderable* focused = nullptr;

    void focusNext()
    {
        if (widgets.empty())
            return;

        size_t start = 0;
        for (size_t i = 0; i < widgets.size(); ++i)
            if (widgets[i].get() == focused)
                start = i + 1;

        for (size_t n = 0; n < widgets.size(); ++n)
        {
            Iface::Renderable* candidate = widgets[(start + n) % widgets.size()].get();
            if (candidate->focusable())
            {
                focused = candidate;
                return;
            }
        }
    }

  public:
    template<typename T, typename... Args>
    T& add(Args&&... args)
    {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *widget;
        widgets.push_back(std::move(widget));

        if (!focused && ref.focusable())
            focused = &ref;

        return ref;
    }

    void onEvent(Event& e) override
    {
        if (focused)
            focused->onEvent(e);

        if (!e.handled)
        {
            EventDispatcher d(e);
            d.dispatch<EventTab>([&](EventTab&)
            {
                focusNext();
                return true;
            });
        }

        if (!e.handled)
            OnEvent::onEvent(e);
    }

    void render(Helper::TerminalManipulation& term)
    {
        term.hideCursor();
        term.clearScreenAndMoveHome();

        for (auto& widget : widgets)
            widget->render(term);

        if (focused)
        {
            focused->placeCursor(term);
            term.showCursor();
        }
    }
};

int main()
{
    Terminal terminal;
    Screen screen;

    screen.add<Label>(1, 1, 6, 1).text = "Name: ";
    auto& nameInput = screen.add<InputText>(7, 1, 30, 1);
    auto& status = screen.add<Label>(1, 3, 40, 1);

    nameInput.onSubmit = [&](const std::string& value) { status.text = "Submitted: " + value; };

    // Events no widget handled.
    screen.setCallback([&](Event& e)
    {
        EventDispatcher d(e);
        d.dispatch<EventCtrlC>([&](EventCtrlC&)
        {
            terminal.exit();
            return true;
        });
    });

    terminal.setCallback([&](Event& e)
    {
        screen.onEvent(e);
        screen.render(terminal.manipulate());
    });

    screen.render(terminal.manipulate());
    terminal.manipulate().flush();
    terminal.readInput();

    terminal.manipulate().clearScreenAndMoveHome();
    terminal.manipulate().flush();
    return 0;
}
