//
// Created by bobi on 3. 04. 26.
//

#pragma once
#include <cstdint>
#include <ctime>
#include <string>

#include "Text/Stream.h"

namespace Utils {


struct Time {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds = 0U;

    Time() {
        updateNow();

    }

    void updateNow() {
        const std::time_t t = std::time(0U); // current time
        const std::tm* now = std::localtime(&t);
        hours = now->tm_hour;
        minutes = now->tm_min;
        seconds = now->tm_sec;
    }

    std::string toString() const {
        Utils::Text::Stream s;
        s << (hours < 10 ? "0" : "") << hours << ":";
        s << (minutes < 10 ? "0" : "") << minutes << ":";
        s << (seconds < 10 ? "0" : "") << seconds;
        return s.end();
        // (now->tm_hour < 10) ? time.append("0" + std::to_string(now->tm_hour)) : time.append(std::to_string(now->tm_hour));
        // time.append(":");
        // (now->tm_min < 10) ? time.append("0" + std::to_string(now->tm_min)) : time.append(std::to_string(now->tm_min));
        // time.append(":");
        // (now->tm_sec < 10) ? time.append("0" + std::to_string(now->tm_sec)) : time.append(std::to_string(now->tm_sec));
        // time.append("] ");
        //
        // return time;
    }
};

}
