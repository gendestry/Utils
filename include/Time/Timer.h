//
// Created by bobi on 13. 03. 26.
//

#pragma once
#include <chrono>

namespace Utils::Time {
    class Timer {
        std::chrono::time_point<std::chrono::system_clock> m_Start;
    public:
        Timer();

        void start();
        unsigned int stop() const;
    };
}