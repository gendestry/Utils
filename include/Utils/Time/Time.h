//
// Created by bobi on 3. 04. 26.
//

#pragma once
#include <chrono>
#include <cstdint>
#include <ctime>
#include <string>

#include "Utils/Text/Stream.h"

namespace Utils
{

struct Time
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds = 0U;

    Time() { updateNow(); }

    void updateNow()
    {
        const auto tp = std::chrono::system_clock::now();
        const std::time_t t = std::chrono::system_clock::to_time_t(tp);
        const std::tm *now = std::localtime(&t);
        hours = now->tm_hour;
        minutes = now->tm_min;
        seconds = now->tm_sec;

        const auto sinceEpoch = tp.time_since_epoch();
        milliseconds = static_cast<uint16_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch).count() % 1000
        );
    }

    std::string toString() const
    {
        Utils::Text::Stream s;
        s << (hours < 10 ? "0" : "") << hours << ":";
        s << (minutes < 10 ? "0" : "") << minutes << ":";
        s << (seconds < 10 ? "0" : "") << seconds;
        return s.end();
    }
};

} // namespace Utils
