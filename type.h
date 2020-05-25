#pragma once

#include <iostream>
#include "utils.h"

struct Type
{
    bool isConst = false;
    bool isConstexpr = false;
    std::string name;

    bool isReference() const {
        return name.find('&') < name.length();
    }

    bool isPrimitive() const {
        return  name == "bool"
        ||      name == "char"
        ||      name == "int"
        ||      name == "unsigned"
        ||      name == "float"
        ||      name == "double"
        ||      name == "long";
    }
};

std::ostream& operator<<(std::ostream& os, const Type& type)
{
    if(type.isConst) {
        os << "const ";
    }

    if(type.isConstexpr) {
        os << "constexpr ";
    }

    return os << type.name;
}

Type parseType(const std::string& str)
{
    Type type;

    auto split = smartSplit(str, ' ', true);

    for(size_t k=0; k<split.size(); k++)
    {
        std::string& s = split[k];
        s = sanitize(s);

        if(s=="") {
            //do nothing
        }
        else
        if(s=="const") {
            type.isConst = true;
        }
        else
        if(s=="constexpr") {
            type.isConstexpr = true;
        }
        else
        {
            type.name = join(split.begin()+k, split.end(), " ");
            return type;
        }
    }

    return type;
}