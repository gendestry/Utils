#pragma once
#include <expected>
#include <string>

namespace Utils::File {
    std::expected<std::string, std::string> read(const std::string& path);
}