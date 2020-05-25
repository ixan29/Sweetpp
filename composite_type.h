#pragma once

enum CompositeType {
    STRUCT,
    CLASS,
    ABSTRACT,
    INTERFACE
};

CompositeType parseCompositeType(const std::string& str)
{
    std::string line = sanitize(str);

    if(line == "struct") {
        return STRUCT;
    }
    else
    if(line == "class") {
        return CLASS;
    }
    else
    if(line == "abstract class") {
        return ABSTRACT;
    }
    else
    if(line == "interface") {
        return INTERFACE;
    }
    else {
        throw std::runtime_error(std::string("Error at parsing composite type. The following cannot be parsed:")+line);
    }
}

std::ostream& operator<<(std::ostream& os, CompositeType compositeType)
{
    switch(compositeType)
    {
        case STRUCT:
            return os << "struct";
        
        case CLASS:     
        case ABSTRACT:
        case INTERFACE:
            return os << "class";

        default:
            throw std::runtime_error("Error at printing composite type. It is not defined");
    }
}

Encapsulation getDefaultEncapsulation(CompositeType compositeType)
{
    switch(compositeType)
    {
        case STRUCT:
        case INTERFACE:
            return PUBLIC;
        
        case CLASS:     
        case ABSTRACT:
            return PRIVATE;

        default:
            throw std::runtime_error("Error at getting composite type default encapsulation. It is not defined");
    }
}