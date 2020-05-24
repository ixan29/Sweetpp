#pragma once

#include <iostream>
#include "utils.h"

enum Encapsulation {
    PRIVATE,
    PROTECTED,
    PUBLIC
};

std::ostream& operator<<(std::ostream& os, Encapsulation encapsulation)
{
    switch(encapsulation)
    {
        case PRIVATE:   return os << "private";
        case PROTECTED: return os << "protected";
        case PUBLIC:    return os << "public";
        default: throw std::runtime_error("Error at printing encapsulation!");
    }
}

Encapsulation parseEncapsulation(const std::string& str)
{
    std::string line = sanitize(str);

    if(line == "private") {
        return PRIVATE;
    }
    else
    if(line == "protected") {
        return PROTECTED;
    }
    else
    if(line == "public") {
        return PUBLIC;
    }
    else {
        throw std::runtime_error(std::string("Error at encapsulation parsing. The string cannot be parsed:")+str);
    }
}