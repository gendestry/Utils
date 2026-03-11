//
// Created by bobi on 8. 03. 26.
//

#include "Font.h"
#include <sstream>

namespace Utils::Text::Font {
    std::string colorByRGB(uint8_t r, uint8_t g, uint8_t b, bool fg)
    {
        std::stringstream ss;
        ss << "\x1B[" << (fg ? 3 : 4) << "8;2;" << r << ";" << g << ";" << b << "m";
        return ss.str();
    }

    std::string colorByRGB(const Colors::RGB& color, bool fg)
    {
        return std::format("\x1B[{}8;2;{};{};{}m", (fg ? 3 : 4), color.r, color.g, color.b);
    }
}