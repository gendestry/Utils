//
// Created by bobi on 23. 02. 26.
//
#include "Utils/Colors/ColorFormatter.h"
// #include "Utils/Grid/Grid.h"
#include "Utils/Logging/Logger.h"
#include "Utils/Regex/Matcher.h"
#include "Utils/Terminal/Terminal.h"

#include <string>

// #pragma once
#include "Utils/Traits/ID.h"

#include <array>
#include <cstdint>
#include <sstream>


// template <typename T = uint32_t>
#define T uint32_t
// template<typename T = uint32_t>
struct Subgrid : public Utils::Traits::IDGenerator<Subgrid>
{
     std::vector<T*> data;
     uint32_t width;
     uint32_t height;

     Subgrid() = default;
     Subgrid(uint32_t width, uint32_t height) : width(width), height(height)
     {
         static uint32_t _id = 0;
         data.reserve(width * height);
     }

     void initID()
     {
         for (auto d : data)
         {
             *d = getUID();
         }
     }

};

struct Rectangle
{
    uint32_t x, y;
    uint32_t width, height;
};


template <uint32_t WIDTH, uint32_t HEIGHT>
class Grid
{
    std::array<T, WIDTH * HEIGHT> data;
};
// template <uint32_t WIDTH, uint32_t HEIGHT>
// class Grid
// {
//     std::array<T, WIDTH * HEIGHT> data;
//     // std::vector<Subgrid> grids;
//     T& getValue(uint32_t x, uint32_t y)
//     {
//         return data[y * WIDTH + x];
//     }
// public:
//     Grid()
//     {
//         data.fill(0);
//     }
//
//     void addSubgrid(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
//     {
//         static T t = 1;
//         Subgrid subgrid(width, height);
//         for (auto i = y; i < y + height; i++)
//             for (auto j = x; j < x + width; j++)
//                 subgrid.data.push_back(&getValue(i, j));
//                 // getValue(j, i) = t;
//
//         // subgrid.initID();
//         // grids.push_back(std::move(subgrid));
//         t++;
//     }
//
//     uint32_t getWidth() const { return WIDTH; }
//     uint32_t getHeight() const { return HEIGHT; }
//     [[nodiscard]] std::string toString() const
//     {
//         std::stringstream ss;
//         for (auto i = 0; i < HEIGHT; i++)
//         {
//             for (auto j = 0; j < WIDTH; j++)
//             {
//                 ss << data[i * WIDTH + j] << " ";
//             }
//             ss << std::endl;
//         }
//
//         return ss.str();
//     }
// };


using namespace Utils;
int main()
{
    Utils::Logger logger("Main");
    logger.setLoggerLevel(Utils::Logger::DEBUGGING);
    Grid<5,6> grid;
    grid.addSubgrid(0,0,2,3);
    grid.addSubgrid(2,0,3,2);
    grid.addSubgrid(2,3,3,2);
    grid.addSubgrid(2,2,3,1);
    grid.addSubgrid(0,3,2,3);
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
