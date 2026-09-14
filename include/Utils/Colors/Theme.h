#pragma once
#include "Utils/Colors/ColorFormatter.h"

namespace Theme
{
inline const auto err = Utils::Font::themed({210, 100, 100});
inline const auto ok = Utils::Font::themed({90, 200, 110});
inline const auto txt = Utils::Font::themed({100, 150, 220});
inline const auto num = Utils::Font::themed({220, 190, 100});   //
inline const auto warn = Utils::Font::themed({201, 199, 68});   // yellow
inline const auto owarn = Utils::Font::themed({230, 145, 70});  // orange
inline const auto accent = Utils::Font::themed({240, 170, 90}); // softer orange
inline const auto lbl = Utils::Font::themed({80, 190, 190});    // teal
inline const auto name = Utils::Font::themed({190, 160, 230});  // violet
inline const auto path = Utils::Font::themed({140, 170, 210});  // muted blue
inline const auto dim = Utils::Font::themed({130, 130, 130});   // grey
inline const auto pink = Utils::Font::themed({225, 130, 170});  // magenta
inline const auto lime = Utils::Font::themed({170, 200, 100});  // olive/lime
} // namespace Theme