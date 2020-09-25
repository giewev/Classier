#include <string>
#include "Logger.h"

Logger Logger::mainLogInstance;

Logger::Logger()
{
    const int logSizeMb = 5;
    const int mbSize = 1048576;
    const int fileCount = 3;
    //this->log = spdlog::rotating_logger_mt("MainLogger", "Classy.log", logSizeMb * mbSize, fileCount);
}
