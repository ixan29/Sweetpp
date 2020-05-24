#pragma once

#include <iostream>
#include "type.h"
#include "encapsulation.h"

struct Attribute
{
    Encapsulation encapsulation;
    bool isStatic = false;
    Type type;
    std::string name;
    std::string defaultValue;
};

std::ostream& operator<<(std::ostream& os, const Attribute& attribute)
{
    os << attribute.encapsulation << ": ";

    if(attribute.isStatic) {
        os << "static ";
    }

    os << attribute.type << " " << attribute.name << " = " << attribute.defaultValue;
}