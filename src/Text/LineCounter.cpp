#include "LineCounter.h"

namespace Utils::Text
{
    LineCounter::LineCounter()
    {}

    LineCounter::LineCounter(const std::string &input)
    {
        count(input);
    }

    void LineCounter::count(const std::string &input)
    {
        m_lines.clear();
        m_lines.push_back(0);
        for (unsigned int i = 0; i < input.size(); i++)
        {
            if (input[i] == '\n')
            {
                m_lines.push_back(i);
            }
        }
    }


    unsigned int LineCounter::operator[](unsigned int index)
    {
        return accumulate(index);
    }

    unsigned int LineCounter::getXOffset(unsigned int index)
    {
        unsigned int limit = accumulate(index, 0);
        return m_lines[limit];
    }

    unsigned int LineCounter::numLinesInBetween(unsigned int start, unsigned int end) const
    {
        return accumulate(end) - accumulate(start);
    }

    unsigned int LineCounter::accumulate(unsigned int index, unsigned int accOffset) const
    {
        unsigned int acc = 0;

        for (unsigned int i = 1; i < m_lines.size(); i++)
        {
            unsigned int line = m_lines[i];
            if (line < index)
            {
                acc++;
            }
            else if (line == index)
            {
                // acc++;
                break;
            }
        }

        return acc;
    }

    const std::vector<unsigned int>& LineCounter::lines() const
    {
        return m_lines;
    }

    const std::vector<unsigned int> &LineCounter::operator()() const
    {
        return m_lines;
    }
};