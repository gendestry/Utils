#pragma once
#include "Args.h"
#include <functional>

namespace Utils::Commands
{
template <typename Ctx> struct Command
{
    using Callback = std::function<bool(const Args &, Ctx &)>;
    using Guard = std::function<bool(const Ctx &)>;

    std::string key; // verb, or "verb/object" — app decides the key scheme
    std::string desc, usage, reason;
    Guard guard = [](const Ctx &) { return true; };
    Callback func;

    bool available(const Ctx &c) const { return guard(c); }
};
} // namespace Utils::Commands