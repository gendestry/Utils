//
// Registry<Ctx> + Command<Ctx>: a tiny todo list driven entirely by string
// commands. No terminal loop yet — this just feeds a handful of lines
// through the registry to show registration, guards, help groups and the
// predicate fallback ("bare number" = "done <n>").
//

#include "Utils/Commands/Registry.h"
#include <print>
#include <string>
#include <vector>

using namespace Utils::Commands;

struct Task
{
    std::string text;
    bool done = false;
};

// The context every command operates on. Guards read it, callbacks mutate it.
struct Ctx
{
    std::vector<Task> tasks;
};

static Registry<Ctx> buildRegistry()
{
    Registry<Ctx> reg;

    reg.group("Tasks");
    reg.add("add", "add a task", "add <text...>", [](const Ctx &) { return true; },
             [](const Args &args, Ctx &ctx) {
                 if (!args.has(1))
                     return false;

                 std::string text;
                 for (size_t i = 1; i < args.size(); ++i)
                 {
                     if (i > 1)
                         text += ' ';
                     text += args[i];
                 }
                 ctx.tasks.push_back({std::move(text), false});
                 std::println("added #{}: {}", ctx.tasks.size(), ctx.tasks.back().text);
                 return true;
             });

    reg.add("list", "list all tasks", "list",
             [](const Ctx &ctx) { return !ctx.tasks.empty(); },
             [](const Args &, Ctx &ctx) {
                 for (size_t i = 0; i < ctx.tasks.size(); ++i)
                     std::println("  {} [{}] {}", i + 1, ctx.tasks[i].done ? 'x' : ' ',
                                  ctx.tasks[i].text);
                 return true;
             },
             "no tasks yet — try 'add <text>'"); // reason shown when guard fails

    // "done <n>" — keyed command, the normal path.
    reg.add("done", "mark a task done", "done <n>",
             [](const Ctx &ctx) { return !ctx.tasks.empty(); },
             [](const Args &args, Ctx &ctx) {
                 auto n = args.getInt(1);
                 if (!n || *n < 1 || static_cast<size_t>(*n) > ctx.tasks.size())
                     return false;
                 ctx.tasks[*n - 1].done = true;
                 std::println("done #{}: {}", *n, ctx.tasks[*n - 1].text);
                 return true;
             },
             "no tasks yet");
    reg.finish();

    reg.group("Bare shortcuts");
    // A nameless command: a line that's just a number is treated as "done <n>".
    // Tried only after keyed lookups (add/list/done/...) fail to match.
    reg.addPredicate("<n>", "shortcut for 'done <n>'", "<n>",
                     [](const Args &args) { return args.size() == 1 && args.isInt(0); },
                     [](const Ctx &ctx) { return !ctx.tasks.empty(); },
                     [](const Args &args, Ctx &ctx) {
                         auto n = *args.getInt(0);
                         if (n < 1 || static_cast<size_t>(n) > ctx.tasks.size())
                             return false;
                         ctx.tasks[n - 1].done = true;
                         std::println("done #{}: {}", n, ctx.tasks[n - 1].text);
                         return true;
                     },
                     "no tasks yet");
    reg.finish();

    return reg;
}

// Runs a line through the registry: resolve() then, if found, invoke it.
static void run(const Registry<Ctx> &reg, Ctx &ctx, const std::string &line)
{
    Args args(line);
    std::println("> {}", line);

    if (args.empty())
        return;

    // Single key here since this app's scheme is just "verb"; an app with
    // "verb/object" keys would pass the fallback chain instead, e.g.
    // resolve({"add/urgent", "add"}, args, ctx).
    auto res = reg.resolve(args.command(), args, ctx);
    if (res)
    {
        if (!res.cmd->func(args, ctx))
            std::println("  usage: {}", res.cmd->usage);
    }
    else if (res.blocked())
    {
        std::println("  {}", res.reason);
    }
    else
    {
        std::println("  unknown command: '{}'", args.command());
    }
}

static void printHelp(const Registry<Ctx> &reg, const Ctx &ctx)
{
    std::println("== help ==");
    for (const auto &group : reg.groups())
    {
        std::println("{}:", group.name);
        for (const auto &cmd : group.commands)
        {
            const char *mark = cmd->available(ctx) ? " " : "*";
            std::println("  {}{:<10} {}", mark, cmd->usage, cmd->desc);
        }
    }
}

int main()
{
    Registry<Ctx> reg = buildRegistry();
    Ctx ctx;

    printHelp(reg, ctx); // everything but 'add' is guarded off — no tasks yet
    std::println("");

    run(reg, ctx, "list");            // blocked: no tasks yet
    run(reg, ctx, "add buy milk");
    run(reg, ctx, "add write example");
    run(reg, ctx, "list");
    run(reg, ctx, "done 1");          // keyed command
    run(reg, ctx, "2");               // predicate fallback: bare "<n>"
    run(reg, ctx, "list");
    run(reg, ctx, "nope");            // unknown command

    std::println("");
    printHelp(reg, ctx); // now every command is available

    return 0;
}
