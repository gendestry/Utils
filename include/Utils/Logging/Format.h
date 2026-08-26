#pragma once
#include <cstdint>
#include <string>

namespace Utils::Logging
{
enum Level : uint8_t
{
    NOTSET = 0U,
    DEBUGGING = 1U,
    INFO = 2U,
    WARN = 3U,
    SUCCESS = 4U,
    ERROR = 5U,
};
struct Record
{
    Logging::Level level;
    std::string scope;
    std::string time;
    std::string location;
    std::string msg;
};
} // namespace Utils::Logging