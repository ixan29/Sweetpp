#pragma once

#include <string>
#include "content.h"

struct Text : public Content
{
    std::string str;

    Text(const std::string& str_)
    :str(str_)
    {}

    virtual void writeDefinition(std::ostream& os)
    {
        os << str << std::endl;
    }
    
    virtual void writeTemplateImplementation(std::ostream& os) {}
    virtual void writeImplementation(std::ostream& os) {}
};