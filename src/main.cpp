//
// Created by bobi on 23. 02. 26.
//

// #include "Storage/FragmentedStorage.h"
// #include <print>
#include "Regex/Matcher.h"
#include "Logging/Logger.h"
#include <string>

using namespace Utils::Regex;
int main() {
    Utils::Logger logger("Main");
    logger.setLoggerLevel(Utils::Logger::DEBUGGING);

    // Engine::Tokenizer tok("123+");
    // tok.tokenize();
    // tok.print_tokens();
    // Utils::Regex::Matcher t("('a''b'? | ('1'| '22'))+");
    Utils::Regex::Matcher matcher("[a-z]{2,5}");

    matcher.printTokens();
    matcher.printAst();

    std::string m = "aa";
    logger.println("Input: '{}'", m);
    auto f = matcher.findAllInfo(m);
    if (f.has_value()) {
        for (auto v : f.value()) {
            logger.debug("[{:2}-{:2}]'{}'", v.start, v.start + v.match.length() - 1, v.match);
        }
    }
    return 0;
    // Utils::Regex::Matcher t("'qwe'('a' | 'b+')?");
    // t.printTokens();
    // Utils::Regex::Matcher t("('+386' | '0')");

    // std::string m = "qwea111111qwe23qweb+";
    // logger.println("Input: '{}'", m);
    // auto f = t.findAllInfo(m);
    // if (f.has_value()) {
    //     for (auto v : f.value()) {
    //         logger.debug("[{:2}-{:2}]'{}'", v.start, v.start + v.match.length() - 1, v.match);
    //     }
    // }

    return 0;
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