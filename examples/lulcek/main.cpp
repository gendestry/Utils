//
// Created by bobi on 23. 02. 26.
//
#include "Utils/Colors/ColorFormatter.h"
#include "Utils/Colors/HSV.h"
#include "Utils/Colors/RGB.h"
// #include "Utils/Grid/Grid.h"
#include "Utils/Logging/Logger.h"
#include "Utils/Regex/Matcher.h"
#include "Utils/Terminal/Terminal.h"

#include <string>

// #pragma once
// #include "Utils/Grid/Grid.h"
#include "Utils/Traits/ID.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <random>
#include <sstream>

inline Utils::Colors::RGB randomPastelColor()
{
    constexpr float goldenAngle = 137.5077f;
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<float> satDist(0.45f, 0.7f);
    static std::uniform_real_distribution<float> valDist(0.85f, 1.f);
    static float hue = std::uniform_real_distribution<float>(0.f, 360.f)(rng);

    hue = std::fmod(hue + goldenAngle, 360.f);
    return Utils::Colors::HSV(hue, satDist(rng), valDist(rng)).toRGB();
}


#define T uint32_t
// template<typename T = uint32_t>
struct Rectangle
{
    uint32_t x, y;
    uint32_t width, height;

    Rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        : x(x), y(y), width(width), height(height) {}

    bool contains(const Rectangle &rect) const
    {
        return rect.x >= x && rect.y >= y &&
               rect.x + rect.width  <= x + width &&
               rect.y + rect.height <= y + height;
    }

    bool adjacent(const Rectangle &rect) const
    {
        bool touchesVertically = (rect.x == x + width || x == rect.x + rect.width) &&
                                  rect.y < y + height && y < rect.y + rect.height;

        bool touchesHorizontally = (rect.y == y + height || y == rect.y + rect.height) &&
                                    rect.x < x + width && x < rect.x + rect.width;

        return touchesVertically || touchesHorizontally;
    }
};



struct Subgrid : Utils::Traits::IDGenerator<Subgrid>, Rectangle
{
    std::vector<uint32_t> indexes;
    Utils::Colors::RGB color = randomPastelColor();

    Subgrid(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        : Rectangle(x, y, width, height)
    {
    }

    [[nodiscard]] std::string toString() const
    {
        std::stringstream ss;
        ss << "Subgrid[id=" << getUID() << ", x=" << x << ", y=" << y
           << ", width=" << width << ", height=" << height << ", indexes=(";
        for (std::size_t i = 0; i < indexes.size(); i++)
        {
            if (i > 0) ss << ", ";
            ss << indexes[i];
        }
        ss << ")]";

        return ss.str();
    }
};

template <uint32_t WIDTH, uint32_t HEIGHT>
class Grid
{
    std::array<T, WIDTH * HEIGHT> data;
    std::vector<Subgrid> subgrids;

    bool contains(const Rectangle &rect) const
    {
        for (auto& s : subgrids)
            if (s.contains(rect)) return true;
        return false;
    }
public:
    Grid()
    {
        data.fill(0);
    }

    void addSubgrid(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {

        static uint32_t sval = 1;
        if (contains(Rectangle{x,y,width,height}))
        {
            return;
        }

        Subgrid subgrid(x,y, width, height);
        for (auto i = y; i < y + height; i++)
            for (auto j = x; j < x + width; j++)
            {
                uint32_t index = i * WIDTH + j;
                subgrid.indexes.push_back(index);
                data[index] = sval;
            }

        subgrids.push_back(std::move(subgrid));
        sval++;
    }

    void setValue(uint32_t i, T val)
    {
        if (i >= subgrids.size())
        {
            return;
        }

        auto &subgrid = subgrids[i];
        for (auto& index : subgrid.indexes)
        {
            data[index] = val;
        }
    }

    void makeSubgrids(uint32_t w, uint32_t h)
    {
        subgrids.clear();
        const auto cols = (WIDTH + w - 1) / w;
        const auto rows = (HEIGHT + h - 1) / h;

        for (auto y = 0u; y < rows; y++)
            for (auto x = 0u; x < cols; x++)
            {
                uint32_t sx = x * w;
                uint32_t sy = y * h;
                uint32_t sw = std::min(w, WIDTH - sx);
                uint32_t sh = std::min(h, HEIGHT - sy);
                addSubgrid(sx, sy, sw, sh);
            }
    }

    uint32_t getWidth() const { return WIDTH; }
    uint32_t getHeight() const { return HEIGHT; }

    [[nodiscard]] const Subgrid *subgridAt(uint32_t index) const
    {
        for (const auto &subgrid : subgrids)
            for (auto i : subgrid.indexes)
                if (i == index)
                    return &subgrid;

        return nullptr;
    }

    [[nodiscard]] std::string toString() const
    {
        std::stringstream ss;
        for (auto i = 0; i < HEIGHT; i++)
        {
            for (auto j = 0; j < WIDTH; j++)
            {
                uint32_t index = i * WIDTH + j;
                std::stringstream cell;
                cell << data[index] << " ";

                if (const auto *subgrid = subgridAt(index))
                    ss << Utils::Font::FG(subgrid->color, cell.str()).str();
                else
                    ss << cell.str();
            }
            ss << std::endl;
        }

        return ss.str();
    }
};

using namespace Utils;
int main()
{
    Utils::Logger logger("Main");
    logger.setLoggerLevel(Utils::Logger::DEBUGGING);
    Grid<6,7> grid;
    // grid.makeSubgrids(2,3);
    grid.addSubgrid(0,0,2,3);
    grid.addSubgrid(0,0,1,1);
    grid.addSubgrid(2,0,3,2);
    // grid.addSubgrid(2,3,3,2);
    grid.addSubgrid(2,2,3,1);
    grid.addSubgrid(0,3,2,3);
    std::cout << grid.toString() << std::endl;
    grid.setValue(0,7);
    std::cout << grid.toString() << std::endl;


    // Terminal::Terminal terminala([](){exit(0);});
    // terminala.setSuggestionSource([](const std::string& s){return s + "asd";});
    // terminala.readInput();
    return 0;
}
//     std::string str =
//         R"(test = "asd"
// neki=1.0
// x = 1
// y = false)";

//
//     Utils::Regex::Matcher matcher(R"({\T}+' '*'='' '*{\A}+)");
//     auto split = Utils::String::split(str, "\n");
//     for (auto s : split)
//     {
//         auto _match = matcher.findAllGroupsInfo(s);
//         if (!_match) continue;
//         for (auto &match : *_match)
//         {
//             std::println("  match '{}' [{}-{}]", match.match, match.start, match.end());
//             for (std::size_t i = 0; i < match.groups.size(); i++)
//             {
//                 const auto &group = match.groups[i];
//                 std::println("    group {}: '{}' [{}-{}]", i + 1, group.match, group.start, group.end());
//
//                 // A {..} inside a {..} is reported under it, however deep it goes.
//                 for (const auto &sub : group.groups)
//                 {
//                     std::println("      sub: '{}' [{}-{}]", sub.match, sub.start, sub.end());
//                 }
//             }
//         }
//         std::println("");
        // auto groups = matcher.findAllGroupsInfo(s);
        // if (!groups) continue;
        // for (auto g : *groups)
        // {
        //     std::cout << g.toString() << std::endl;
        // }
        // logger.warn(s);
    // }
    // using namespace Font;
    // std::string str = std::format("Test {} x: {}, y: {}", "neki", 2, 4.3f);
    // logger.debug(str);
    // logger.warn(str);
    // logger.error(str);
//     return 0;
// }
