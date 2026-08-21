//
// Created by bobi on 23. 02. 26.
//

// #include "Storage/FragmentedStorage.h"
// #include <print>
#include "Utils/Colors/Gradient.h"
#include "Utils/Logging/Logger.h"
#include "Utils/Regex/Matcher.h"
#include <iostream>
#include <print>
#include <string>

// #include "File/File.h"
// #include "Network/Interfaces.h"
// #include "Network/SACN.h"
// #include "Text/Stream.h"
#include "Utils/Math/Interval.h"
#include <array>
#include <chrono>
#include <thread>

// using namespace Utils::Network;
using namespace Utils;
int main()
{
    Utils::Logger logger("Main");
    logger.setLoggerLevel(Utils::Logger::DEBUGGING);

    // Maths::IntervalBase A(0, 3);
    // Maths::IntervalBase B(7, 12);
    // Maths::IntervalBase C(18, 20);
    // Maths::IntervalBase D(1, 7);
    // Maths::IntervalBase E(16, 17);

    // auto c = A | B;
    // if (A.merge(B))
    // {
    //     logger.println("{}", A.toString());
    // }

    // if (B.overlaps(A))
    // {
    //     logger.println("123{}", A.toString());
    // }
    // Maths::Interval interval;
    // interval.add(A);
    // interval.add(B);
    // interval.add(C);
    // logger.println("{}", interval.toString());

    // Maths::Interval interval2;
    // interval2.add(D);
    // interval2.add(E);
    // logger.println("{}", interval2.toString());

    // auto uni = interval | interval2;
    // auto inter = interval & interval2;

    // logger.println("U: {}", uni.toString());
    // logger.println("I: {}", inter.toString());

    // interval.remove(9);
    // logger.println("{}", interval.toString());

    // interval.remove({2, 5});
    // logger.println("{}", interval.toString());

    // interval.add(5);
    // logger.println("{}", interval.toString());

    // // interval.add({8, 13});

    // logger.println("{}", interval.toString());
    // logger.println("{}", interval.contains(10));
    // logger.println("{}", interval.contains(7));
    // logger.println("{}", A > B);
    // logger.println("{}", B < A);
    // logger.println("{}", B > A);
    return 0;

    // Utils::Colors::Gradient grad(20);
    // grad.addSegment({{255, 0, 0}, 100, 0});
    // grad.addSegment({{0, 0, 255}, 100, 0});
    // for (auto a : grad.get())
    // {
    //     std::cout << a.toString() << std::endl;
    // }
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

    Utils::Regex::Matcher matcher("[a-z]{2,5}");

    matcher.printTokens();
    matcher.printAst();

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
