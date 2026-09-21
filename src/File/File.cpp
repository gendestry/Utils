//
// Created by bobi on 16. 03. 26.
//

#include "Utils/File/File.h"
#include <fstream>

namespace Utils::File
{
    namespace {
        std::expected<std::string, std::string> readWith(const std::string& path,
                                                         std::ios::openmode mode) {
            std::ifstream file(path, mode);
            if (!file.is_open()) {
                return std::unexpected("Could not open file " + path);
            }

            return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() );
        }
    }

    std::expected<std::string, std::string> read(const std::string& path) {
        return readWith(path, std::ios::in);
    }

    std::expected<std::string, std::string> readBinary(const std::string& path) {
        return readWith(path, std::ios::in | std::ios::binary);
    }
}
