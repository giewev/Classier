#include "ArgumentParser.h"
#include <algorithm>
#include <string>

ArgumentParser::ArgumentParser(int argumentCount, char **arguments)
{
    this->argumentCount = argumentCount;
    this->arguments = arguments;
}

bool ArgumentParser::flagExists(const std::string& flag)
{
    char** argumentEnd = this->arguments + this->argumentCount;
    return std::find(this->arguments, argumentEnd, flag) != argumentEnd;
}

char* ArgumentParser::argumentContent(const std::string& flag)
{
    char** argumentEnd = this->arguments + this->argumentCount;
    char** flagAddress = std::find(this->arguments, argumentEnd, flag) + 1;
    if (flagAddress >= this->arguments + this->argumentCount)
    {
        return NULL;
    }
    else
    {
        return *flagAddress;
    }
}
