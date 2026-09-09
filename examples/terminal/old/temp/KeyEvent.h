// //
// // Created by bobi on 9. 9. 26.
// //
//
// #pragma once
// #include <string>
//
// // A decoded keypress. The terminal hands us bytes -- a plain character is one
// // byte, but a special key arrives as an escape sequence (Left is ESC [ D,
// // Ctrl+Left is ESC [ 1 ; 5 D). Everything past the parser works with these
// // instead, so the byte-level mess stays in one place.
// //
// // Modifiers are flags rather than separate enum entries: terminals encode them
// // as a single numeric parameter in the sequence, so ctrl/alt/shift decode
// // straight from the wire and every combination is covered without naming it.
//
// // One entry per physical key. Modifiers live in KeyEvent, not here.
// enum class EventType
// {
//     Char, // a printable character; the character itself is KeyEvent::ch
//     Enter,
//     Tab,
//     Backspace,
//     Delete,
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
//
//     Eof,    // stdin closed
//     Unknown // parsed cleanly, but not a sequence we recognise
// };
//
// #define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
// 								virtual EventType GetEventType() const override { return GetStaticType(); }\
// 								virtual const char* GetName() const override { return #type; }
//
//
// class Event
// {
// public:
//     virtual ~Event() = default;
//
//     bool Handled = false;
//
//     virtual EventType GetEventType() const = 0;
//     virtual const char* GetName() const = 0;
//     // virtual int GetCategoryFlags() const = 0;
//     virtual std::string ToString() const { return GetName(); }
// };
//
// class ArrowUpEvent : public Event
// {
//     public:
//     EVENT_CLASS_TYPE(Up);
// };
//
// struct KeyEvent
// {
//     Key key = Key::Unknown;
//
//     char ch = 0;
//     bool ctrl = false;
//     bool alt = false;
//     bool shift = false;
//
//     bool isChar(char c) const { return key == Key::Char && ch == c; }
//
//     bool isCtrl(char c) const { return key == Key::Char && ch == c && ctrl; }
// };
