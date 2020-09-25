#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger
{
public:
    void writeInfo(std::string message);
protected:
private:
    static Logger mainLogInstance;
    Logger();
};

#endif // LOGGER_H
