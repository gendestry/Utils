#pragma once
#include "Command.h"

#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Utils::Commands
{

// Stores every bound command keyed by Command::key and resolves a set of
// candidate keys (in priority order) to the command that should run in the
// current context. Header-only because Command<Ctx> is a template.
//
// The registry is intentionally dumb about the app's key scheme: the caller
// decides what a key means ("verb", "verb/object", …) and passes the fallback
// chain to resolve() (e.g. {"store/group", "store"}). The registry only does
// lookup + guard evaluation + reason reporting.
template <typename Ctx> class Registry
{
  public:
    using Cmd = Command<Ctx>;
    using CmdPtr = std::shared_ptr<Cmd>;

    // For `help`: commands grouped under a heading, in registration order.
    struct Group
    {
        std::string name;
        std::vector<CmdPtr> commands;
    };

    // Outcome of resolve(): a command was found, or found-but-blocked (guard
    // failed, `reason` set), or nothing matched (both null/empty).
    struct Resolution
    {
        CmdPtr cmd;         // the matched, available command (null if none/blocked)
        std::string reason; // why a matched command was unavailable ("" otherwise)

        explicit operator bool() const { return static_cast<bool>(cmd); }
        bool blocked() const { return !cmd && !reason.empty(); }
    };

    // Predicate for nameless commands (bare selector, <name>, …): decides
    // whether this command should handle the given line. Empty = keyed only.
    using Condition = std::function<bool(const Args &)>;

  private:
    struct Predicate
    {
        Condition matches;
        CmdPtr cmd;
    };

    std::map<std::string, CmdPtr> m_byKey; // matched by exact key
    std::vector<Predicate> m_predicate;    // matched by matches(args), in order
    std::vector<Group> m_groups;
    Group m_current;

  public:
    // ── registration ────────────────────────────────────────────
    CmdPtr add(Cmd command)
    {
        auto ptr = std::make_shared<Cmd>(std::move(command));
        m_byKey[ptr->key] = ptr;
        m_current.commands.push_back(ptr);
        return ptr;
    }

    CmdPtr add(std::string key, std::string desc, std::string usage, typename Cmd::Guard guard,
               typename Cmd::Callback func, std::string reason = {})
    {
        return add(Cmd{std::move(key), std::move(desc), std::move(usage), std::move(reason),
                       std::move(guard), std::move(func)});
    }

    // Register a nameless command matched by a predicate over the line (e.g. a
    // bare selector "1 thru 10", or a <name> fallback). Tried after keyed
    // lookups fail, in registration order.
    CmdPtr addPredicate(Cmd command, Condition matches)
    {
        auto ptr = std::make_shared<Cmd>(std::move(command));
        m_predicate.push_back({std::move(matches), ptr});
        m_current.commands.push_back(ptr);
        return ptr;
    }

    CmdPtr addPredicate(std::string label, std::string desc, std::string usage, Condition matches,
                        typename Cmd::Guard guard, typename Cmd::Callback func,
                        std::string reason = {})
    {
        return addPredicate(Cmd{std::move(label), std::move(desc), std::move(usage),
                                std::move(reason), std::move(guard), std::move(func)},
                            std::move(matches));
    }

    // Start a new help group; flush the previous one. Call finish() at the end.
    void group(std::string name)
    {
        finish();
        m_current.name = std::move(name);
    }

    void finish()
    {
        if (!m_current.commands.empty() || !m_current.name.empty())
            m_groups.push_back(std::move(m_current));
        m_current = {};
    }

    const std::vector<Group> &groups() const { return m_groups; }

    // ── lookup ───────────────────────────────────────────────────
    // Raw lookup by exact key, ignoring the guard (for `help <cmd>`).
    CmdPtr get(const std::string &key) const
    {
        auto it = m_byKey.find(key);
        return it == m_byKey.end() ? nullptr : it->second;
    }

    // Resolve `args` against the registry. Keyed candidates (in priority order)
    // are tried first; if none exist, predicate commands are tried in
    // registration order. First match wins:
    //   - guard passes → return it (reason empty)
    //   - guard fails  → stop and report its reason (blocked)
    // Nothing matched → empty Resolution (unknown command).
    Resolution resolve(std::initializer_list<std::string> keys, const Args &args,
                       const Ctx &ctx) const
    {
        for (const auto &key : keys)
        {
            auto it = m_byKey.find(key);
            if (it == m_byKey.end())
                continue;
            return check(it->second, ctx); // matched a key — don't fall through to predicates
        }

        for (const auto &p : m_predicate)
            if (p.matches(args))
                return check(p.cmd, ctx);

        return {nullptr, {}};
    }

    // Single-key convenience.
    Resolution resolve(const std::string &key, const Args &args, const Ctx &ctx) const
    {
        return resolve({key}, args, ctx);
    }

    std::vector<CmdPtr> complete(const std::string &prefix) const
    {
        std::vector<CmdPtr> out;
        for (auto it = m_byKey.lower_bound(prefix);
             it != m_byKey.end() && it->first.compare(0, prefix.size(), prefix) == 0; ++it)
            out.push_back(it->second);
        return out;
    }

  private:
    static Resolution check(const CmdPtr &cmd, const Ctx &ctx)
    {
        if (cmd->available(ctx))
            return {cmd, {}};
        return {nullptr, cmd->reason}; // matched but blocked
    }
};

} // namespace Utils::Commands
