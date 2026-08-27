# LOGGING

A scoped logger whose output layout is a pattern string, the way spdlog and Log4j do it.

```cpp
#include "Utils/Logging/Logger.h"

Utils::Logger log("Main");
log.setLoggerLevel(Utils::Logger::DEBUGGING);

log.info("connected to {} in {}ms", host, elapsed);
```

```
02:30:43 INFO    [main.cpp:12] [Main] connected to db.local in 14ms
```

The message text is a `std::format` string, so `{}` placeholders and format specs
(`{:.2f}`, `{:>8}`) work as usual. The call site is captured automatically — there is no
macro anywhere.

## Setting the pattern

```cpp
Utils::Logger::setFormat("%H:%M:%S %7l [%@] [%n] %v");  // every logger
log.setLoggerFormat("[%n] %v");                         // this logger only
```

A logger without its own pattern follows the shared one. `setFormat` may be called at any
time, including while other threads are logging; `setLoggerFormat` touches only that
logger, so treat it like the rest of that object's state (see [Threads](#threads)).

## Flags

A pattern is literal text with `%` flags in it.

| Flag | Meaning |
| --- | --- |
| `%v` | the message |
| `%n` | logger name (scope) |
| `%l` | level word: `DEBUG`, `INFO`, `WARN`, `ERROR`, `SUCC` |
| `%@` | call site, `file.cpp:12` |
| `%T` | `HH:MM:SS` |
| `%H` `%M` `%S` | hours / minutes / seconds |
| `%e` | milliseconds |
| `%%` | a literal `%` |

Anything else from the table in `Format.h` is still to be written and prints itself
literally (`%Y` renders as `%Y`), so a pattern never silently loses text.

## Width

A flag may carry a width: `%[min][.max]flag`

| Spec | `DB` | `Main` | `NetworkInterfaceManager` |
| --- | --- | --- | --- |
| `%n` | `DB` | `Main` | `NetworkInterfaceManager` |
| `%8n` | `DB      ` | `Main    ` | `NetworkInterfaceManager` |
| `%.8n` | `DB` | `Main` | `NetworkI` |
| `%8.8n` | `DB      ` | `Main    ` | `NetworkI` |

`min` pads a short field, `.max` cuts a long one, and both together give a column that is
always the same width — which is the point of `%8.8n`: the `]` after it lands in the same
place on every line.

`-` flips which side gets padded (`%8n` vs `%-8n`). Note this is the opposite of
spdlog and printf, where `-` means left-align.

Widths count what the terminal shows: the ANSI colors in `%l`, `%n` and `%@` do not
count toward the width, and cutting never splits an escape sequence or a UTF-8
character. Double-width CJK and emoji are counted as one column each, so those will
still run wide.

## Levels

`DEBUGGING < INFO < WARN < ERROR`. A message is dropped when its level is below the
active one.

```cpp
Utils::Logger::setLevel(Utils::Logger::WARN);  // every logger
log.setLoggerLevel(Utils::Logger::INFO);       // this logger only, overrides the above
```

Methods: `debug()`, `info()`, `warn()`, `error()`, `success()`. `print()`/`println()`
write without a level, `printColor()`/`printlnColor()` wrap the message in a color.

## Threads

The shared pattern and level are safe to read and change from any thread, and a whole
line is written under one lock, so lines never interleave.

A `Logger` object itself belongs to one thread: its own level, its own pattern override
and its indentation (`incPadOffset`) are plain fields. Give each thread its own `Logger`
and share through `setFormat`/`setLevel`.

## How it works

`Formatter::init()` parses the pattern with `Utils::Regex` (see `../Regex/README.md`),
capturing the width and flag of each `%…` in one pass:

```
'%'{'-'?\d+}?{'.'\d+}?{(\c|\C|'@'|'#'|'!'|'^'|'$'|'%')}
```

What comes out is a list of literals and a list of `Flag{op, padding, maxWidth, lalign}`,
so rendering a message is a walk over two vectors with no pattern parsing at all.
