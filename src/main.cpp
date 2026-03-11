//
// Created by bobi on 23. 02. 26.
//

#include "Storage/FragmentedStorage.h"
#include <print>
#include <iostream>
using namespace Utils;
#include <random>
#include "regex/tokenizer.h"

int getRandomInt(int min, int max) {
    std::random_device seed;
    std::mt19937 gen{seed()}; // seed the generator
    std::uniform_int_distribution<> dist{min, max}; // set min and max
    return dist(gen); // generate number
}


struct Fixture : public Fragment {
    Fixture() : Fragment(getRandomInt(1, 15)) {}
    Fixture(uint32_t size) : Fragment(size) {}
};

int main() {
    Regex::Tokenizer t("\T+");
    FragmentedStorage<Fragment, 100> storage;

    storage.addMultiple(Fragment(2), 5, 10);
    storage.add(Fragment(10));
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
    std::cout << storage.fragmentsToString() << std::endl;
}