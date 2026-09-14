# REGEX

The matches in the following method don't have to be full matches:
- Method find returns the first match found in the searched string
- Method findAll returns all matches in the searched string

## Groups

`{<search>}` captures what it matches. `findGroupsInfo` returns the first match together
with its captures, `findAllGroupsInfo` every match in the text, and `matchGroupsInfo` only
matches when the pattern covers the whole text. Each `MatchInfo` carries `match`, `start`,
`len`/`end()` and a `groups` vector with the same fields.
See `examples/regex_groups/main.cpp`.

Three rules decide what ends up in `groups`:

- Only `{}` produces an entry. `(a|b)` groups for alternation but never becomes a capture;
  the `{}` inside it are reported as if the parens weren't there.
- A `{}` inside a `{}` is reported under it, to any depth. The outer group's `match` is
  still its whole span, so `{{'-'}?{\d+}}` gives `"-3"` with `"-"` and `"3"` beneath it.
- A repeated group keeps every iteration: `({\d+}',')+` over `1,2,3,` reports three
  entries, not just the last.

Consequently indices are positional only if the pattern says so — an optional group that
didn't participate leaves no entry at all, so check `groups.size()` instead of assuming a
fixed slot.

```cpp
Utils::Regex::Matcher flagRegex(R"('%'{'-'?\d+}?\T)");

// Hold the optional in a variable: iterating over *call() dangles on GCC < 15.
auto matches = flagRegex.findAllGroupsInfo("%-3S %H, %1M");
if (!matches) return;

for (auto& m : *matches)
    for (auto& g : m.groups)
        std::println("{} -> width '{}'", m.match, g.match);
```

## Search possibilities
- `'text'`: match text
- `[<c1> - <c2>]`: match digits or characters in range from c1 to c2 (c2 needs to be bigger)
- `(<search> | <search2>)`: match search1 or search2

Every search possibility can have an operator describing repititions

## Escape characters

There are 5 escape characters in this regex implementation:

Character | Explanation
--- | ---
\c | Matches any lower case character
\C | Matches any upper case character
\T | Matches any character
\d | Matches any digit
\n | Matches newline

## Operators
There are 4 operators
Operator | Explanation
--- | ---
\* | Matches 0 or more times
\+ | Matches 1 or more times
\? | Matches 0 or 1 time
{x,y} | Matches between x and y times

## Examples
- `'apple'` matches the string "apple"
- `('apple' | \c)` matches the string "apple" or small character
- `([1-8] | [a-d])+` matches either a digit from 1 to 8 or characters between a and d one or multiple times
- `'c'{1,7}('a' | 'b')?` matches from 1 to 7 characters c and then possibly ending with characters a or b
