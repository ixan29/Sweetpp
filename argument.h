#pragma once

#include "type.h"

struct Argument
{
    Type type;
    std::string name;
    std::string defaultValue;
};

std::ostream& operator<<(std::ostream& os, const Argument& argument)
{
    return os << argument.type << " " << argument.name << " = " << argument.defaultValue; 
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