//
// Group capture: '{' ... '}' marks a capture group, findAllGroupsInfo walks every
// match in the text and reports each match together with the groups inside it.
//

#include "Utils/Regex/Matcher.h"
#include <iostream>
#include <print>
#include <string>

using namespace Utils;

static void dump(const Regex::Matcher &matcher, const std::string &text)
{
    std::println("pattern: {}", matcher.getPattern());
    std::println("text:    '{}'", text);

    auto matches = matcher.findAllGroupsInfo(text);
    if (!matches)
    {
        std::println("  no matches\n");
        return;
    }

    for (auto &match : *matches)
    {
        std::println("  match '{}' [{}-{}]", match.match, match.start, match.end());
        for (std::size_t i = 0; i < match.groups.size(); i++)
        {
            const auto &group = match.groups[i];
            std::println("    group {}: '{}' [{}-{}]", i + 1, group.match, group.start, group.end());

            // A {..} inside a {..} is reported under it, however deep it goes.
            for (const auto &sub : group.groups)
            {
                std::println("      sub: '{}' [{}-{}]", sub.match, sub.start, sub.end());
            }
        }
    }
    std::println("");
}

int main()
{
    Regex::Matcher flagRegex(R"('AB'{\A}+'X')");
    flagRegex.printAst();
    flagRegex.prettyPrint();

    dump(flagRegex, "ABCCCX");
    // '%', then an optional captured width like "-3" or "1", then any flag character.
    // Regex::Matcher flagRegex(R"('%'{'-'?\d+}?\T)");
    // flagRegex.prettyPrint();
    //
    // dump(flagRegex, "%-3S %H, %1M");
    //
    // // A single, fully consumed match can be read with matchGroupsInfo instead: it
    // // returns a match only when the pattern covers the whole text.
    // if (auto whole = flagRegex.matchGroupsInfo("%-3S"))
    // {
    //     std::println("matchGroupsInfo: {}", whole->toString());
    //     for (const auto &group : whole->groups)
    //     {
    //         std::println("  width: '{}'", group.match);
    //     }
    // }
    // std::println("");
    //
    // // Two groups per match.
    // Regex::Matcher pair(R"({\c+}'='{\d+})");
    // dump(pair, "width=12, height=7;");
    //
    // // Groups nest, and a repeated group keeps every iteration. Parens are only there
    // // for grouping and alternation, so they never show up as a capture themselves.
    // dump(Regex::Matcher(R"('%'{{'-'}?{\d+}}?{\T})"), "%-3S");
    // dump(Regex::Matcher(R"({({\d+}',')+})"), "1,2,3,");
    //
    // // Rebuild a std::format string out of the flags, the way a formatter would.
    // const std::string pattern = "%H:%M:%S asd %i";
    // if (auto matches = flagRegex.findAllGroupsInfo(pattern))
    // {
    //     std::string format;
    //     std::string flags;
    //     std::size_t pos = 0;
    //
    //     for (const auto &match : *matches)
    //     {
    //         format.append(pattern, pos, match.start - pos); // text before the flag
    //         format += "{}";
    //         flags += match.match.back(); // the flag character itself
    //         pos = match.start + match.len;
    //     }
    //     format.append(pattern, pos); // trailing text
    //
    //     std::println("'{}' -> '{}' flags: '{}'", pattern, format, flags);
    // }

    return 0;
}
