#pragma once
#include <cstdint>
#include <format>
#include <iostream>
#include <string>

namespace Utils::Terminal
{
struct TerminalManipulation
{
    // --------------------------------------------------------
    // Cursor movement
    // --------------------------------------------------------

    void moveCursorStart() { std::cout << "\r"; }

    void moveCursorUp(uint16_t lines = 1) { std::cout << std::format("\033[{}A", lines); }

    void moveCursorDown(uint16_t lines = 1) { std::cout << std::format("\033[{}B", lines); }

    void moveCursorLeft(uint16_t columns = 1) { std::cout << std::format("\033[{}D", columns); }

    void moveCursorRight(uint16_t columns = 1) { std::cout << std::format("\033[{}C", columns); }

    void moveCursorToColumn(uint16_t column) { std::cout << std::format("\033[{}G", column); }

    void moveCursorToPosition(uint16_t row, uint16_t column)
    {
        std::cout << std::format("\033[{};{}H", row, column);
    }

    // --------------------------------------------------------
    // Line manipulation
    // --------------------------------------------------------

    void clearLine() { std::cout << "\033[2K"; }

    void clearLineFromCursor() { std::cout << "\033[0K"; }

    void clearLineToCursor() { std::cout << "\033[1K"; }

    // --------------------------------------------------------
    // Screen manipulation
    // --------------------------------------------------------

    void clearScreen() { std::cout << "\033[2J"; }

    void clearScreenFromCursor() { std::cout << "\033[0J"; }

    void clearScreenToCursor() { std::cout << "\033[1J"; }

    void clearScreenAndMoveHome() { std::cout << "\033[2J\033[H"; }

    // --------------------------------------------------------
    // Cursor visibility
    // --------------------------------------------------------

    void hideCursor() { std::cout << "\033[?25l"; }

    void showCursor() { std::cout << "\033[?25h"; }

    // --------------------------------------------------------
    // Text formatting
    // --------------------------------------------------------

    void resetFormatting() { std::cout << "\033[0m"; }

    void bold() { std::cout << "\033[1m"; }

    void dim() { std::cout << "\033[2m"; }

    void underline() { std::cout << "\033[4m"; }

    void reverse() { std::cout << "\033[7m"; }

    void noBold() { std::cout << "\033[22m"; }

    void noUnderline() { std::cout << "\033[24m"; }

    void noReverse() { std::cout << "\033[27m"; }

    // --------------------------------------------------------
    // New lines
    // --------------------------------------------------------

    void newLine() { std::cout << '\n'; }

    void carriageReturn() { std::cout << '\r'; }

    // --------------------------------------------------------
    // Output
    // --------------------------------------------------------

    void flush() { std::cout.flush(); }
};
} // namespace Utils::Terminal