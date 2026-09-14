//
// Created by bobi on 23. 02. 26.
//

#include "Utils/Logging/Logger.h"
#include "Utils/Math/Curve.h"

#include <cmath>
#include <string>

struct Function
{
    std::vector<float> values;
    Function(uint32_t length) : values(length)
    {
        for (int i = 0; i < values.size(); ++i)
        {
            float norm = (float)i / (float)(values.size()-1);
            values[i] = std::fabs(std::sin(norm * 2*M_PI));
        }
    }

    void print() {
        for (const float& v : values) {
            std::print("{:.2} ", v);
        }
    }
};

using namespace Utils;
int main()
{
    Utils::Logger logger("Main");
    logger.setLoggerLevel(Utils::Logger::DEBUGGING);
    Function f(50);
    f.print();
    std::println();


    Maths::Sinusoid curve(10);
    // Maths::SinusoidHalf curve2(19);
    for (auto i = 0; i < 10; ++i)
    {
        std::print("{:.2} ", curve[i]);
    }
    //
    // std::println();
    // for (auto i = 0; i < 20; ++i)
    // {
    //     std::print("{:.2} ", curve2[i]);
    // }
    return 0;
}