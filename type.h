#pragma once

#include <iostream>
#include "utils.h"

struct Type
{
    bool isConst = false;
    std::string name;
};

std::ostream& operator<<(std::ostream& os, const Type& type)
{
    if(type.isConst) {
        os << "const ";
    }

    return os << type.name;
}

Type parseType(const std::string& str)
{
    std::string line = sanitize(str);

    Type type;

    if(line.find("const ")==0)
    {
        type.isConst = true;
        line = sanitize(line.substr(6));
    }

    type.name = std::move(line);

    return type;
}