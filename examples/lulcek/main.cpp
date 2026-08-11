//
// Created by bobi on 23. 02. 26.
//

// #include "Storage/FragmentedStorage.h"
// #include <print>
#include "Utils/Colors/Colors.h"
#include "Utils/Colors/Font.h"
#include "Utils/Colors/Gradient.h"
#include "Utils/Colors/RGB.h"
#include "Utils/Logging/Logger.h"
#include "Utils/Regex/Matcher.h"
#include <cstddef>
#include <print>
#include <string>

// #include "File/File.h"
// #include "Network/Interfaces.h"
// #include "Network/SACN.h"
// #include "Text/Stream.h"
#include "Utils/Commands/Registry.h"
#include <array>
#include <chrono>
#include <iostream>
#include <thread>

// using namespace Utils::Network;
using namespace Utils::Regex;
int main()
{
    Utils::Logger logger("Main");
    logger.setLoggerLevel(Utils::Logger::DEBUGGING);

    for (const bool hardCut : {true, false})
    {
        Utils::Colors::Gradient gradient(10, hardCut);
        gradient.addSegments({{Utils::Colors::RGB(255, 0, 0), 20.f},
                              {Utils::Colors::RGB(0, 255, 0), 40.f},
                              {Utils::Colors::RGB(0, 0, 255), 40.f}});

        logger.println("Gradient red 20% / green 40% / blue 40%, size {}, hardCut = {}:",
                       gradient.get().size(), hardCut);
        for (std::size_t i = 0; i < gradient.get().size(); i++)
        {
            logger.println("  {:2}: {}", i, gradient.get()[i].toString());
        }
    }

    {
        Utils::Colors::Gradient g(50);
        g.addSegments({
            {{255, 0, 0}, 20.f},
            {{0, 255, 0}, 30.f},
            {{0, 0, 255}, 80.f},
        });

        std::print("\nGradient(50), red 20 / green 30 / blue 80, lerpInHSV:\n  ");
        for (const auto &c : g.get())
        {
            std::print("{} ", Utils::Font::colorByRGB(c, false));
        }
        std::print("{}\n", Utils::Font::reset);

        for (std::size_t i = 0; i < g.get().size(); i++)
        {
            const auto &c = g.get()[i];
            const auto hsv = c.toHSV();
            std::print("  {:2}: {}  {} rgb({:3},{:3},{:3})  hsv({:6.1f},{:4.2f},{:4.2f})\n", i,
                       Utils::Font::colorByRGB(c, false) + "  " + Utils::Font::reset,
                       Utils::Font::colorByRGB(c) + "██" + Utils::Font::reset, c.r, c.g,
                       c.b, hsv.h, hsv.s, hsv.v);
        }
    }

    {
        constexpr std::size_t N = 10;
        const Utils::Colors::RGB red(255, 0, 0);
        const Utils::Colors::RGB blue(0, 0, 255);

        logger.println("Lerp red -> blue, N = {}:", N);
        logger.println("   t     lerpInRGB          lerpInHSV");
        for (std::size_t i = 0; i < N; i++)
        {
            const float t = static_cast<float>(i) / static_cast<float>(N - 1);
            const auto inRgb = Utils::Colors::lerpInRGB(red, blue, t);
            const auto inHsv = Utils::Colors::lerpInHSV(red, blue, t);

            logger.println("  {:.2f}  ({:3}, {:3}, {:3})    ({:3}, {:3}, {:3})", t, inRgb.r,
                           inRgb.g, inRgb.b, inHsv.r, inHsv.g, inHsv.b);
        }
    }

    return 0;

    Matcher matcher("'abc'+[0-3]");
    auto info = matcher.matchInfo("abcabc1xxxxxx");
    // auto x = Utils::File::read("1");
    // if (x.has_value()) {
    //     logger.println(x.value());
    // }
    // else {
    //     logger.println(x.error());
    // }

    // SacnSender sender;
    // sender.begin(8, IP("192.168.0.6"));
    // uint8_t buff[512] = {255};
    // std::array<uint8_t, 512> packet;
    // for (auto i = 0; i < 512; i++) {
    // packet[i] = 255;
    // buff[i] = 0;
    // }
    // sender.setBuffer(buff.get());
    // sender.send();
    // sender.send(packet);

    // Utils::Regex::Matcher matcher("{'qwe'}*");
    // Utils::Regex::Matcher matcher(R"(!{'--'}{('"'\T+'"' | \T+)})");
    // Utils::Regex::Matcher matcher(R"('--'?'setuniverse '{\d+}' '?{\d+}?)");
    // Utils::Regex::Matcher matcher(R"('--'?'setuniverse '{\d+}' '?{\d+}?)");

    // Utils::Regex::Matcher matcher(R"(({\c}+'123')+)");
    // Utils::Regex::Matcher matcher ("{[a-z]+}'1'{[a-z]+}");
    // Utils::Regex::Matcher matcher("!{'""'}'asd')+");
    // Utils::Regex::Matcher matcher("('asd'('qwe' | '1'))+");
    // Utils::Regex::Matcher matcher(R"({'asd'('qwe' | '1')}+)");

    // Utils::Regex::Matcher matcher("{('a'|'b')+}'1'");
    // Utils::Regex::Matcher matcher(R"('a'({\c})?)");
    // Utils::Regex::Matcher matcher(R"(!{'--'}{\T+})");

    // matcher.printTokens();
    // matcher.printAst();
    // matcher.prettyPrint();

    // auto m = matcher.matchGroups("asd1asd1");
    // auto m = matcher.matchGroupsInfo("ax");
    // auto x = matcher.matchGroups("asdfooasdfoo");
    // if (m.has_value()) {
    // auto v = m.value();
    // for (auto g : v.groups) {
    //     std::cout << g.match << std::endl;
    // }
    // std::cout << m->toString();
    // }
    // std::cout << m.match << std::endl;
    // for (auto g : m.groups) {
    // for (auto g1 : g)
    // std::cout << g.match << ", ";
    // std::cout << std::endl;
    // }
    return 0;

    // std::string m = "aa";
    // logger.println("Input: '{}'", m);
    // auto f = matcher.findAllInfo(m);
    // if (f.has_value()) {
    // for (auto v : f.value()) {
    // logger.debug("[{:2}-{:2}]'{}'", v.start, v.start + v.match.length() - 1, v.match);
    // }
    // }

    // return 0;
    // FragmentedStorage<Fragment, 100> storage;
    //
    // storage.addMultiple(Fragment(2), 5, 10);
    // storage.add(Fragment(10));
    // storage.add(Fragment(7), 20);
    // storage.add(Fragment(35), 50);
    // storage.add(Fragment(3));
    // storage.add(Fragment(10), 3);
    // Fragment fragment(10);
    // storage.add(Fixture(), 5);
    // storage.add(Fixture(), 30);
    // storage.addFirstEmpty(Fixture());
    // storage.addFirstEmpty(Fixture());
    // storage.append(Fixture(8));

    // auto v = storage.findFirstEmpty(8);
    // if (v.has_value()) {
    //     std::cout << std::to_string(v.value()) << std::endl;
    // storage.addAt(std::make_shared<Fragment>(8), v.value());
    //
    // }

    // std::cout << storage.fragmentsToString() << std::endl;
    // storage.defragment();
    // std::cout << storage.fragmentsToString() << std::endl;
}
