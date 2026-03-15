//
// Created by bobi on 16. 03. 26.
//

#include "File/File.h"
#include <fstream>

using namespace Utils::File;

std::string read(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}