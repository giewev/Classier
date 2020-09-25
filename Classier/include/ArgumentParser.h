#include <string>


class ArgumentParser
{
public:
    ArgumentParser(int argumentCount, char *arguments[]);
    bool flagExists(const std::string& flag);
    char* argumentContent(const std::string& flag);
protected:
private:
    int argumentCount;
    char** arguments;
};
