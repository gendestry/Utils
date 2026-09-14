// //
// // Created by bobi on 8. 9. 26.
// //
//
// #include "Utils/Storage/Quadtree.h"
// #include "Utils/Terminal/Terminal.h"
// #include "Utils/Terminal/TerminalApplication.h"
//
// #include <algorithm>
// #include <memory>
// #include <optional>
// #include <string>
// #include <vector>
//
// using namespace Utils::Terminal;
// using namespace Utils::Terminal::Events;
//
// namespace Temp
// {
//
// struct Renderable : Utils::Maths::Rectangle, Iface::OnEvent
// {
//     Renderable* parent = nullptr;
//
//     virtual Utils::Maths::Size measure() const { return {0.0f, 0.0f}; }
//     virtual void render(Helper::TerminalManipulation&) {}
// };
//
// /*class Element
// {
//     mutable std::unique_ptr<Renderable> m_node;
//
// public:
//     Element() = default;
//
//     template<std::derived_from<Renderable> T>
//     Element(std::unique_ptr<T> node) : m_node(std::move(node))
//     {
//     }
//
//     Element(const Element& other) : m_node(std::move(other.m_node)) {}
//     Element(Element&& other) noexcept : m_node(std::move(other.m_node)) {}
//     Element& operator=(const Element& other)
//     {
//         m_node = std::move(other.m_node);
//         return *this;
//     }
//     Element& operator=(Element&& other) noexcept
//     {
//         m_node = std::move(other.m_node);
//         return *this;
//     }
//
//     Renderable* get() const { return m_node.get(); }
//     Renderable* operator->() const { return m_node.get(); }
//     explicit operator bool() const { return bool(m_node); }
//
//     std::unique_ptr<Renderable> release() const { return std::move(m_node); }
// };*/
//
//
// struct Text : Renderable
// {
//     std::string text;
//
//     Utils::Maths::Size measure() const override { return {static_cast<float>(text.size()), 1.0f}; }
//     virtual void render(Helper::TerminalManipulation&)
//     {
//         std::cout << text;
//     }
// };
//
// struct Container : Renderable
// {
//     std::vector<std::unique_ptr<Renderable>> widgets;
// };
//
// class Application : public Iface::OnEvent
// {
//     Terminal& m_terminal;
//     std::vector<std::unique_ptr<Iface::Renderable>> widgets;
//     // Quadtree<Iface::Renderable> quadtree;
//
// public:
//     Application(Terminal& terminal)
//     // getSize() is {rows, cols}: columns are the width, rows the height.
//     : m_terminal(terminal)
//     // , quadtree({0,0, (float)terminal.getSize().second, (float)terminal.getSize().first})
//     {
//
//     }
//
//     // Utils::Quadtree<Iface::Renderable>& quad()
//     // {
//     //     return quadtree;
//     // }
//     template<typename T, typename... Args>
//     T& add(Args&&... args)
//     {
//         auto widget = std::make_unique<T>(std::forward<Args>(args)...);
//         T& ref = *widget;
//         widgets.push_back(std::move(widget));
//
//         // if (ref.focusable())
//         // {
//         //     quadtree.insert(&ref);
//         //     if (!focused)
//         //         focus(&ref);
//         // }
//
//         return ref;
//     }
//
//     void onEvent(Events::Event & e) override
//     {
//
//     }
//
//     void render(Helper::TerminalManipulation& term)
//     {
//         term.hideCursor();
//         term.clearScreenAndMoveHome();
//
//         for (auto& widget : widgets)
//         {
//             widget->render(term);
//         }
//     }
// };
// }
//
// int main()
// {
//     Terminal terminal;
//     Temp::Application app(terminal);
//     auto [rows, cols] = Terminal::getSize();
//     // auto& c = app.add<Temp::Container>(2,2,cols-2, rows-2);
//     // c.border = Helper::Border{};
//     // auto& label = app.add<Label>(0,2,20,0);
//     // label.text = "Label: 0";
//     // // // auto& counterRow = app.add<NavContainer>(2, 5, 22, 1);
//     // // // counterRow.border = Helper::Border{};
//     // auto& decrement = app.add<Button>(0, 0, "-");
//     // auto& counter = app.add<Label>(6, 0, 10, 1);
//     // auto& increment = app.add<Button>(17, 0, "+");
//     // // // //
//     // int count = 0;
//     // counter.text = "Count: 0";
//     // decrement.onPress = [&] { counter.text = "Count: " + std::to_string(--count); };
//     // increment.onPress = [&] { counter.text = "Count: " + std::to_string(++count); };
//     // auto& todo = app.add<Checklist>(2, 8, std::vector<std::string>{"Milk", "Eggs", "Bread"});
//
//
//     terminal.setCallback([&](Events::Event & e)
//     {
//         EventDispatcher d(e);
//         d.dispatch<EventCtrlC>([&](EventCtrlC&)
//         {
//            terminal.exit();
//            return true;
//         });
//
//         app.onEvent(e);
//         // label.text = e.toString();
//         app.render(terminal.manipulate());
//     });
//
//     // app.setCallback([&](Event& e)
//     // {
//     //     EventDispatcher d(e);
//     //     d.dispatch<EventCtrlC>([&](EventCtrlC&)
//     //     {
//     //        terminal.exit();
//     //        return true;
//     //     });
//     // });
//
//     app.render(terminal.manipulate());
//     terminal.manipulate().flush();
//     terminal.readInput();
//
//     terminal.manipulate().clearScreenAndMoveHome();
//     terminal.manipulate().showCursor();
//     terminal.manipulate().flush();
// }