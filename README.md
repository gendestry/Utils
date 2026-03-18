# Simple C++ utils
My collection of Utils functions and classes in C++

## Colors
- RGB and HSV implementation
- Font class: colorise linux terminal
- conversion between RGB and HSV
- gradient generation of 2 or more colors

## File
For now a function that returns a string of the file contents

## Logging
Contains a simple scoped logger class, supports multiple log levels (DEBUG, INFO, ERROR)

## Maths
- linear interpolation
- random int generator
- fill vector with a range of integers
- curve generator (sinusoid, triangle, square)

## Regex
Simplified regex implementation, more in Regex/README.md

## Storage
- Fragmented Storage implementation:
    - fixed sized array of N bytes
    - Insert fragments into the array at specific address with specific size

## Text
 - LineCounter is a class that counts the number of newlines ('\n')
 - Stream is a std::stringstream wrapper with formatting
 - String
   - concat methods
   - split a string into a vector of strings by some delimiter
   - pad string with some other string

## Time
Timer class, that measures time