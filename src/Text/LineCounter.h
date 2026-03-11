#pragma once
#include <vector>
#include <string>

namespace Utils::Text
{
    class LineCounter
    {
        // positions of newlines in 1d
        std::vector<unsigned int> m_lines;
    public:
        // input = string with newlines so the whole file in a string lets say
        LineCounter();
        explicit LineCounter(const std::string &input);

        void count(const std::string &input);

        // returns the number of new lines (accumulated) at index
        [[nodiscard]] unsigned int accumulate(unsigned int index, unsigned int accOffset = 1) const;

        unsigned int operator[](unsigned int index);
        unsigned int getXOffset(unsigned int index);

        [[nodiscard]] unsigned int numLinesInBetween(unsigned int start, unsigned int end) const;
        [[nodiscard]] const std::vector<unsigned int>& lines() const;
        [[nodiscard]] const std::vector<unsigned int>& operator()() const;

    };
};