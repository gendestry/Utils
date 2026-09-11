// #pragma once
// #include <cstdint>
// #include <string>
//
// // A key as it arrives from a terminal.
// //
// // A tty hands us bytes, never modifier key-down events, so the modifiers below are
// // whatever survived the encoding -- see Mods for what that means per modifier.
//
// enum class Key : uint16_t
// {
//     None = 0,
//     Char, // a printable character; the codepoint is in KeyEvent::ch
//
//     Enter,
//     Tab,
//     Backspace,
//     Escape,
//
//     Up,
//     Down,
//     Left,
//     Right,
//
//     Home,
//     End,
//     PageUp,
//     PageDown,
//     Insert,
//     Delete,
//
//     F1,
//     F2,
//     F3,
//     F4,
//     F5,
//     F6,
//     F7,
//     F8,
//     F9,
//     F10,
//     F11,
//     F12,
//
//     PasteBegin, // bracketed paste, ESC [ 200 ~
//     PasteEnd,   // bracketed paste, ESC [ 201 ~
// };
//
// struct Mods
// {
//     bool shift = false;
//     bool alt = false;
//     bool ctrl = false;
//     bool super = false;
//
//     // The CSI modifier parameter is 1 + a bitmask: 1=shift, 2=alt, 4=ctrl, 8=super.
//     // So ESC [ 1 ; 5 C is param 5 == 1 + 4 == Ctrl+Right. A missing or zero param
//     // means no modifiers.
//     static constexpr Mods fromCsi(int param)
//     {
//         const int m = param > 0 ? param - 1 : 0;
//         return Mods{(m & 1) != 0, (m & 2) != 0, (m & 4) != 0, (m & 8) != 0};
//     }
//
//     // The inverse, for emitting sequences.
//     constexpr int toCsi() const
//     {
//         return 1 + (shift ? 1 : 0) + (alt ? 2 : 0) + (ctrl ? 4 : 0) + (super ? 8 : 0);
//     }
//
//     constexpr bool none() const { return !shift && !alt && !ctrl && !super; }
//
//     constexpr bool operator==(const Mods &) const = default;
// };
//
// struct KeyEvent
// {
//     Key key = Key::None;
//     char32_t ch = 0; // valid when key == Key::Char
//     Mods mods{};
//
//     // Unmodified key: e.is(Key::Left) is false for Ctrl+Left.
//     constexpr bool is(Key k) const { return key == k && mods.none(); }
//
//     // Exact match including modifiers: e.is(Key::Left, Mods{.ctrl = true}).
//     constexpr bool is(Key k, Mods m) const { return key == k && mods == m; }
//
//     // Ignores modifiers; use when only the key itself matters.
//     constexpr bool isKey(Key k) const { return key == k; }
//
//     constexpr bool isChar(char32_t c) const { return key == Key::Char && ch == c; }
//
//     // Unmodified printable character, i.e. something to insert into a buffer.
//     // False for Ctrl+a, Alt+a and for every named key.
//     constexpr bool isText() const { return key == Key::Char && mods.none() && ch >= 0x20 && ch != 0x7F; }
//
//     // "Ctrl+Left", "Alt+a", "F5", "Enter". For debugging and keybinding display.
//     std::string toString() const;
// };
//
// // Bare name of a key, without modifiers: "Left", "F5", "Enter", "Char".
// constexpr const char *keyName(Key k)
// {
//     switch (k)
//     {
//     case Key::None: return "None";
//     case Key::Char: return "Char";
//     case Key::Enter: return "Enter";
//     case Key::Tab: return "Tab";
//     case Key::Backspace: return "Backspace";
//     case Key::Escape: return "Escape";
//     case Key::Up: return "Up";
//     case Key::Down: return "Down";
//     case Key::Left: return "Left";
//     case Key::Right: return "Right";
//     case Key::Home: return "Home";
//     case Key::End: return "End";
//     case Key::PageUp: return "PageUp";
//     case Key::PageDown: return "PageDown";
//     case Key::Insert: return "Insert";
//     case Key::Delete: return "Delete";
//     case Key::F1: return "F1";
//     case Key::F2: return "F2";
//     case Key::F3: return "F3";
//     case Key::F4: return "F4";
//     case Key::F5: return "F5";
//     case Key::F6: return "F6";
//     case Key::F7: return "F7";
//     case Key::F8: return "F8";
//     case Key::F9: return "F9";
//     case Key::F10: return "F10";
//     case Key::F11: return "F11";
//     case Key::F12: return "F12";
//     case Key::PasteBegin: return "PasteBegin";
//     case Key::PasteEnd: return "PasteEnd";
//     }
//     return "Unknown";
// }
//
// // Appends a codepoint to a UTF-8 string. Input arrives byte at a time, so the parser
// // decodes; anything holding a buffer needs to encode back.
// inline void appendUtf8(std::string &out, char32_t cp)
// {
//     if (cp < 0x80)
//     {
//         out += char(cp);
//     }
//     else if (cp < 0x800)
//     {
//         out += char(0xC0 | (cp >> 6));
//         out += char(0x80 | (cp & 0x3F));
//     }
//     else if (cp < 0x10000)
//     {
//         out += char(0xE0 | (cp >> 12));
//         out += char(0x80 | ((cp >> 6) & 0x3F));
//         out += char(0x80 | (cp & 0x3F));
//     }
//     else
//     {
//         out += char(0xF0 | (cp >> 18));
//         out += char(0x80 | ((cp >> 12) & 0x3F));
//         out += char(0x80 | ((cp >> 6) & 0x3F));
//         out += char(0x80 | (cp & 0x3F));
//     }
// }
//
// inline std::string KeyEvent::toString() const
// {
//     std::string s;
//
//     if (mods.ctrl)
//         s += "Ctrl+";
//     if (mods.alt)
//         s += "Alt+";
//     if (mods.shift)
//         s += "Shift+";
//     if (mods.super)
//         s += "Super+";
//
//     if (key != Key::Char)
//     {
//         s += keyName(key);
//         return s;
//     }
//
//     if (ch == U' ')
//         s += "Space";
//     else if (ch < 0x20 || ch == 0x7F)
//         s += "0x" + std::string{"0123456789ABCDEF"[(ch >> 4) & 0xF], "0123456789ABCDEF"[ch & 0xF]};
//     else
//         appendUtf8(s, ch);
//
//     return s;
// }
