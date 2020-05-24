#pragma once

#include <iostream>

struct Content
{
    virtual ~Content() {};
    virtual void writeDefinition(std::ostream& os) = 0;
    virtual void writeTemplateImplementation(std::ostream& os) = 0;
    virtual void writeImplementation(std::ostream& os) = 0;
};