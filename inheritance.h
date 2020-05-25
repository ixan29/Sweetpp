#pragma once

#include "utils.h"
#include "encapsulation.h"

struct Inheritance
{
    Encapsulation encapsulation = PUBLIC;
    std::string base;
    std::vector<std::string> primary_arguments;
};

std::ostream& operator<<(std::ostream& os, const Inheritance& inheritance)
{
    return os << inheritance.encapsulation << " " << inheritance.base;
}

Inheritance parseInheritance(const std::string& str)
{
    std::string line = sanitize(str);

    Inheritance inheritance;

    if(line.find("private ") == 0) {
        inheritance.encapsulation = PRIVATE;
        line = sanitize(line.substr(8));
    }
    else
    if(line.find("protected ") == 0) {
        inheritance.encapsulation = PROTECTED;
        line = sanitize(line.substr(10));
    }
    else
    if(line.find("public ") == 0) {
        inheritance.encapsulation = PUBLIC;
        line = sanitize(line.substr(7));
    }

    if(line.find('(') < line.length())
    {
        inheritance.base = line.substr(0, line.find('('));

        auto part = line.substr(line.find('(')+1);
        part = part.substr(0, part.find_last_of(')'));

        inheritance.primary_arguments = smartSplit(part, ',', true);
    }
    else
    {
        inheritance.base = std::move(line);
    }

    return inheritance;
}

std::vector<Inheritance> parseInheritances(const std::string& str)
{
    std::vector<Inheritance> inheritances;

    std::vector<std::string> split = smartSplit(str, ',', true);

    inheritances.reserve(split.size());

    for(auto& s : split) {
        inheritances.push_back(parseInheritance(s));
    }

    return inheritances;
}