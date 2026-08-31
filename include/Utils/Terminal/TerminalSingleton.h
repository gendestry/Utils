#pragma once
#include "Terminal.h"

#include <functional>
#include <stdexcept>

namespace Utils::Terminal
{
// The process has exactly one real stdin/stdout, so there's meaningfully
// only one Terminal — two live instances would fight over tcsetattr on the
// same fd. This gives every caller access to that one instance without
// threading a Terminal& through every constructor.
//
// Lifecycle is explicit on purpose (init() then instance()) rather than
// lazily constructing on first instance() call: whoever calls init() is the
// one who gets to decide the exit callback, and a stray instance() before
// that is a bug worth catching rather than silently getting a default.
class TerminalSingleton
{
    TerminalSingleton() = delete;

    static Terminal *&slot()
    {
        static Terminal *terminal = nullptr;
        return terminal;
    }

  public:
    // Constructs the one Terminal for this process (enters raw mode). Call
    // once, early — a second call is a bug and throws.
    static Terminal &init(std::function<void()> onExit = {})
    {
        auto &ref = slot();
        if (ref)
            throw std::logic_error("TerminalSingleton::init() called more than once");

        static Terminal terminal(std::move(onExit));
        ref = &terminal;
        return terminal;
    }

    // Reaches the instance set up by init(). Calling before init() is a bug
    // and throws, rather than silently constructing one with no exit
    // callback.
    static Terminal &instance()
    {
        auto &ref = slot();
        if (!ref)
            throw std::logic_error("TerminalSingleton::instance() called before init()");

        return *ref;
    }
};
} // namespace Utils::Terminal
