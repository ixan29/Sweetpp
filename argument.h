#pragma once

#include <sstream>
#include "type.h"

struct Argument
{
    Type type;
    std::string name;
    std::string defaultValue;
};

std::ostream& operator<<(std::ostream& os, const Argument& argument)
{
    os << argument.type << " " << argument.name;

    if(!argument.defaultValue.empty()) {
        os << " = " << argument.defaultValue;
    }

    return os;
}

Argument parseArgument(const std::string& str)
{
    std::string line = sanitize(str);

    Argument argument;

    if(line.find('=') < line.length())
    {
        argument.defaultValue = sanitize(line.substr(line.find('=')+1));
        line = sanitize(line.substr(0, line.find('=')));
    }

    argument.name = sanitize(line.substr(line.find_last_of(' ')+1));
    argument.type = parseType(line.substr(0, line.find_last_of(' ')));

    return argument;
}

std::vector<Argument> parseArguments(const std::string& str)
{
    std::string line = sanitize(str);

    if(line.empty()) {
        return {};
    }

    std::vector<Argument> arguments;
    auto split = smartSplit(line, ',', true);
    arguments.reserve(split.size());

    for(std::string& argStr : split) {

        argStr = sanitize(argStr);

        if(argStr.find(' ') < argStr.length())
        {
            arguments.push_back(parseArgument(argStr));
        }
        else
        {
            if(arguments.empty()) {
                throw std::runtime_error(
                    std::string("Error at parsing arguments. The first argument has no type: ")
                +   argStr
                );
            }
            else
            {
                std::ostringstream oss;
                oss << arguments.back().type << " " << argStr;
                arguments.push_back(parseArgument(oss.str()));
            }
        }
    }

    return arguments;
}