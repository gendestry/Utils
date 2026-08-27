//
// Created by bobi on 23. 02. 26.
//
#include "Utils/Logging/Logger.h"
#include <string>
#include "Utils/Colors/ColorFormatter.h"

using namespace Utils;
int main()
{
    Utils::Logger logger("Main");
    logger.setLoggerLevel(Utils::Logger::DEBUGGING);

    using namespace Font;
    std::string str = std::format("Test {} x: {}, y: {}", "neki", 2, 4.3f);
    logger.debug(str);
    logger.warn(str);
    logger.error(str);
    return 0;
}
