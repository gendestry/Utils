//
// Created by bobi on 16. 03. 26.
//

#include "File/File.h"
#include <fstream>

namespace Utils::File
{
    std::expected<std::string, std::string> read(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::unexpected("Could not open file " + path);
        }

        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() );
    }
}
