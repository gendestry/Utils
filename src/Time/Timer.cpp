//
// Created by bobi on 13. 03. 26.
//

#include "Time/Timer.h"
using namespace Utils::Time;

Timer::Timer() {
    start();
}

void Timer::start() {
    m_Start = std::chrono::high_resolution_clock::now();
}

unsigned int Timer::stop() const {
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - m_Start).count();
}