#pragma once

#include <iostream>
#include "type.h"
#include "encapsulation.h"
#include "composite.h"

struct Attribute : Content
{
    Encapsulation encapsulation;
    bool isStatic = false;
    Type type;
    std::string name;
    std::string defaultValue;

    virtual void writeDefinition(std::ostream& os)
    {
        os << encapsulation << ": ";
        os << std::endl;

        if(isStatic) {
            os << "static ";
        }

        os << type << " " << name;
        
        if(!defaultValue.empty()) {
            os << " = " << defaultValue;
        }

        os << ";" << std::endl;
    }

    virtual void writeTemplateImplementation(std::ostream& os) {}
    virtual void writeImplementation(std::ostream& os) {}
};

std::unique_ptr<Attribute> parseAttribute(const std::string& str, Composite* parent)
{
    std::unique_ptr<Attribute> attribute(new Attribute());
    attribute->encapsulation = getDefaultEncapsulation(parent->compositeType);

    std::vector<std::string> parts;

    std::string line = replace(str,"=", " = ");

    for(auto& part : smartSplit(line, ' ', true)) {
        if(!part.empty()) {
            parts.push_back(std::move(part));
        }
    }

    size_t k=0;

    std::string returnStr;

    if( parts[k] == "private"
    ||  parts[k] == "protected"
    ||  parts[k] == "public") {
        attribute->encapsulation = parseEncapsulation(parts[k]);
        k++;
    }

    if(parts[k] == "static") {
        attribute->isStatic = true;
        k++;
    }

    while(  parts[k] == "const"
    ||      parts[k] == "constexpr") {
        returnStr += parts[k];
        returnStr += ' ';
        k++;
    }

    returnStr += parts[k];

    attribute->type = parseType(returnStr);

    k++;

    attribute->name = parts[k];

    k++;

    if(k < parts.size()-1)
    {
        if(parts[k] == "=")
        {
            k++;
            attribute->defaultValue = join(parts.begin()+k, parts.end(), " ");
        }
        else
        {
            throw std::runtime_error(std::string("Error at parsing attribute, the remaining part cannot be parsed:")
            +join(parts.begin()+k, parts.end(), " "));
        }
    }

    return std::move(attribute);
}