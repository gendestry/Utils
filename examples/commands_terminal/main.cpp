//
// Same todo list as examples/commands, but driven by a real terminal instead
// of a canned list of lines: CommandTerminal<Ctx> wires the Registry up to
// TerminalSingleton, so the verb autocompletes and the usage hints appear as
// dim ghost text while you type. Ctrl+C / Ctrl+D to quit.
//

#include "Utils/Commands/CommandTerminal.h"
#include "Utils/Terminal/TerminalSingleton.h"
#include <cstdlib>
#include <print>
#include <string>
#include <vector>

using namespace Utils::Commands;
using namespace Utils::Terminal;

struct Task
{
    std::string text;
    bool done = false;
};

struct Ctx
{
    std::vector<Task> tasks;
};

// readInput() only returns on Ctrl+C/Ctrl+D/EOF — there's no "stop" flag it
// polls — so 'quit' exits the same way: std::exit() rather than returning
// from run(). TerminalSingleton holds the Terminal as a function-local
// static, so its destructor (restores the tty) still runs on the way out.
static void quit()
{
    std::println("bye");
    std::exit(0);
}

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
             "no tasks yet — try 'add <text>'");

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

    reg.add("help", "list commands", "help", [](const Ctx &) { return true; },
             [&reg](const Args &, Ctx &ctx) {
                 for (const auto &group : reg.groups())
                 {
                     std::println("{}:", group.name);
                     for (const auto &cmd : group.commands)
                     {
                         const char *mark = cmd->available(ctx) ? " " : "*";
                         std::println("  {}{:<10} {}", mark, cmd->usage, cmd->desc);
                     }
                 }
                 return true;
             });

    reg.add("quit", "exit", "quit", [](const Ctx &) { return true; },
             [](const Args &, Ctx &) {
                 quit();
                 return true; // unreachable — quit() doesn't return
             });
    reg.finish();

    return reg;
}

int main()
{
    Ctx ctx;
    Registry<Ctx> reg = buildRegistry();

    Terminal &terminal = TerminalSingleton::init(quit); // Ctrl+C / Ctrl+D
    CommandTerminal<Ctx> commands(terminal, reg, ctx);

    std::println("todo — try 'add <text>', 'list', 'done <n>', 'help', 'quit'");
    commands.run();

    return 0;
}
