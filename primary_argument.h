#pragma once

#include "argument.h"
#include "attribute.h"
#include "method.h"

struct PrimaryArgument
{
    Encapsulation encapsulation;
    Type type;
    std::string name;
    std::string defaultValue;
    bool isAttribute = false;

    Attribute asAttribute() const
    {
        if(!isAttribute) {
            throw std::runtime_error("Error! Attempting to convert a non-attribute primary argument into an attribute");
        }

        Attribute attribute;

        attribute.type = type;
        attribute.encapsulation = encapsulation;
        attribute.name = name;
        //default value initialisation will be managed by the constructors

        return attribute;
    }

    Argument asArgument() const
    {
        Argument argument;

        argument.type = type;
        argument.name = name;
        argument.defaultValue = defaultValue;

        return argument;
    }
};

PrimaryArgument parsePrimaryArgument(const std::string& str, Encapsulation defaultEncapsulation)
{
    std::string line = replace(str, "this->", "this-> ");
    line = replace(line, "=", " = ");

    std::vector<std::string> parts;

    for(auto& part : smartSplit(line, ' ', true)) {
        if(!part.empty()) {
            parts.push_back(std::move(part));
        }
    }

    PrimaryArgument primaryArgument;
    primaryArgument.encapsulation = defaultEncapsulation;

    size_t k=0;

    if( parts[k] == "private"
    ||  parts[k] == "protected"
    ||  parts[k] == "public")
    {
        primaryArgument.encapsulation = parseEncapsulation(parts[k]);
        k++;
    }

    if(parts[k] == "static") {
        throw std::runtime_error(std::string("Error at parsing primary argument. Static primary arguments are not allowed:")+line);
    }

    std::string returnStr;

    while(  parts[k] == "const"
    ||      parts[k] == "constexpr") {
        returnStr += parts[k];
        returnStr += ' ';
        k++;
    }

    returnStr += parts[k];
    primaryArgument.type = parseType(returnStr);

    k++;

    if(parts[k] == "this->") {
        primaryArgument.isAttribute = true;
        k++;
    }

    primaryArgument.name = parts[k];
    k++;

    if(k<parts.size()-1 && parts[k] == " ") {
        k++;
        primaryArgument.defaultValue = parts[k];
    }

    return primaryArgument;
}

std::vector<PrimaryArgument> parsePrimaryArguments(const std::string& str, Encapsulation defaultEncapsulation)
{
    std::vector<PrimaryArgument> primaryArguments;

    std::vector<std::string> split = smartSplit(str, ',', true);

    primaryArguments.reserve(split.size());

    for(auto& s : split) {
        primaryArguments.push_back(parsePrimaryArgument(s, defaultEncapsulation));
    }

    return primaryArguments;
}

std::vector<Attribute> convertIntoAttributes(const std::vector<PrimaryArgument>& primaryArguments)
{
    std::vector<Attribute> attributes;

    for(auto& primaryArgument : primaryArguments) {
        if(primaryArgument.isAttribute) {
            attributes.push_back(primaryArgument.asAttribute());
        }
    }

    return attributes;
}

std::vector<Argument> convertIntoArguments(const std::vector<PrimaryArgument>& primaryArguments)
{
    std::vector<Argument> arguments;
    arguments.reserve(primaryArguments.size());

    for(auto& primaryArgument : primaryArguments) {
        arguments.push_back(primaryArgument.asArgument());
    }

    return arguments;
}

std::vector<std::shared_ptr<Method>> generateConstructors(
    const std::vector<PrimaryArgument>& primaryArguments,
    Composite* parent,
    const std::vector<std::string>& code    
)
{
    std::unique_ptr<Method> lvalueConstructor (new Method());

    lvalueConstructor->name = parent->name;
    lvalueConstructor->arguments.reserve(primaryArguments.size());

    bool needsRvalueConstructor = false;

    for(auto& primaryArgument : primaryArguments)
    {
        lvalueConstructor->arguments.push_back(primaryArgument.asArgument());
        lvalueConstructor->arguments.back().name += '_';

        if(!primaryArgument.type.isReference())
        {   
            needsRvalueConstructor = true;
            Type& type = lvalueConstructor->arguments.back().type;
            type.name += '&';
            type.isConst = true;
        }

        if(primaryArgument.isAttribute) {
            lvalueConstructor->constructorInitializers.push_back(
                primaryArgument.name+'('+lvalueConstructor->arguments.back().name+')'
            );
        }

        for(auto& inheritance : parent->inheritances) {
            lvalueConstructor->constructorInitializers.push_back(
                inheritance.base+'('+join(inheritance.primary_arguments,",")+')'
            );
        }
    }

    if(!needsRvalueConstructor) {
        return {
            std::move(lvalueConstructor)
        };
    }

    std::unique_ptr<Method> rvalueConstructor (new Method());

    rvalueConstructor->name = parent->name;
    rvalueConstructor->arguments.reserve(primaryArguments.size());

    for(auto& primaryArgument : primaryArguments)
    {
        rvalueConstructor->arguments.push_back(primaryArgument.asArgument());
        rvalueConstructor->arguments.back().name += '_';

        if(!primaryArgument.type.isReference())
        {   
            Type& type = lvalueConstructor->arguments.back().type;
            type.name += "&&";
        }

        if(primaryArgument.isAttribute) {
            rvalueConstructor->constructorInitializers.push_back(
                primaryArgument.name+'('+rvalueConstructor->arguments.back().name+')'
            );
        }

        for(auto& inheritance : parent->inheritances) {
            rvalueConstructor->constructorInitializers.push_back(
                inheritance.base+'('+join(inheritance.primary_arguments,",")+')'
            );
        }
    }

    return {
        std::move(lvalueConstructor),
        std::move(rvalueConstructor)
    };
}