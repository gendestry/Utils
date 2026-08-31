#pragma once
#include "Registry.h"
#include "Utils/Terminal/Terminal.h"

#include <functional>
#include <optional>
#include <print>
#include <string>

namespace Utils::Commands
{
// Wires a Registry<Ctx> up to a Terminal: as the user types, suggest()
// completes the verb (or hints its usage's args); on ENTER, submit()
// resolves the line and dispatches it. Takes the Terminal by reference
// rather than owning one — there's only one real stdin/stdout per process
// (see Terminal::TerminalSingleton), so the caller decides which one this
// binds to instead of this class constructing its own.
template <typename Ctx>
class CommandTerminal
{
    Terminal::Terminal &m_terminal;
    Registry<Ctx> &m_registry;
    Ctx &m_ctx;

  public:
    CommandTerminal(Terminal::Terminal &terminal, Registry<Ctx> &registry, Ctx &ctx)
        : m_terminal(terminal), m_registry(registry), m_ctx(ctx)
    {
        m_terminal.setSuggestionSource([this](const std::string &in) { return suggest(in); });
        m_terminal.setSubmitHandler([this](const std::string &in) { submit(in); });
    }

    void run() { m_terminal.readInput(); }

  private:
    // Ghost-text source: while the verb is still being typed, complete it
    // against the registry's keys; once it's followed by a space, hint the
    // usage's argument part (falls away once real args are typed past it).
    std::optional<std::string> suggest(const std::string &input) const
    {
        if (input.empty())
            return std::nullopt;

        Args args(input);

        if (input.find(' ') == std::string::npos)
        {
            auto matches = m_registry.complete(args.command());
            if (matches.empty())
                return std::nullopt;
            return matches.front()->key;
        }

        auto cmd = m_registry.get(args.command());
        if (cmd && cmd->usage.starts_with(input))
            return cmd->usage;

        return std::nullopt;
    }

    // Dispatch the finished line against the registry.
    void submit(const std::string &input)
    {
        Args args(input);
        if (args.empty())
            return;

        auto res = m_registry.resolve(args.command(), args, m_ctx);
        if (res)
        {
            if (!res.cmd->func(args, m_ctx))
                std::println("  usage: {}", res.cmd->usage);
        }
        else if (res.blocked())
            std::println("  {}", res.reason);
        else
            std::println("  unknown command: '{}'", args.command());
    }
};
} // namespace Utils::Commands
