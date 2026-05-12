//
// Created by bobi on 23. 02. 26.
//

// #include "Storage/FragmentedStorage.h"
// #include <print>
#include "Utils/Regex/Matcher.h"
#include "Utils/Logging/Logger.h"
#include <string>
#include <print>

// #include "File/File.h"
// #include "Network/Interfaces.h"
// #include "Network/SACN.h"
// #include "Text/Stream.h"
#include <array>
#include <chrono>
#include <iostream>
#include <thread>

// using namespace Utils::Network;
using namespace Utils::Regex;
int main() {
    Utils::Logger logger("Main");
    logger.setLoggerLevel(Utils::Logger::DEBUGGING);
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
    Utils::Regex::Matcher matcher(R"(('asd'{\d}))");
    // Utils::Regex::Matcher matcher ("!{'--'}{[a-z]+}");
    // Utils::Regex::Matcher matcher("!{'""'}'asd')+");
    // Utils::Regex::Matcher matcher("('asd'('qwe' | '1'))+");
    // Utils::Regex::Matcher matcher("{('a'|'b')+}'1'");
    // Utils::Regex::Matcher matcher(R"(!{'--'}{\T+})");

    matcher.printTokens();
    matcher.printAst();
    matcher.prettyPrint();

    // auto m = matcher.matchGroups("asd1asd1");
    auto m = matcher.matchGroups("asd1");
    // auto x = matcher.matchGroups("asdfooasdfoo");
    if (m.has_value()) {
        auto v = m.value();
        for (auto g : v.groups) {
            std::cout << g.match << std::endl;
        }
    }
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
