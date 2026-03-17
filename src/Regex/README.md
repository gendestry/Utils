# REGEX

The matches in the following method don't have to be full matches:
- Method find returns the first match found in the searched string
- Method findAll returns all matches in the searched string

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
