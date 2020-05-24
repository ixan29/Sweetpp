#pragma once

#include <string>
#include <vector>
#include <memory>
#include "content.h"

struct Namespace : public Content
{
    Namespace* parent = nullptr;
    std::string name;
    std::vector<std::unique_ptr<Content>> contents;

    virtual void writeDefinition(std::ostream& os)
    {
        if(!name.empty()) {
            os
            << "namespace " << name << std::endl
            << "{" << std::endl;
        }

        for(auto& content : contents) {
            content->writeDefinition(os);
        }

        if(!name.empty()) {
            os << "}";
        }
    }

    virtual void writeTemplateImplementation(std::ostream& os)
    {
        for(auto& content : contents) {
            content->writeTemplateImplementation(os);
        }
    }

    virtual void writeImplementation(std::ostream& os)
    {
        for(auto& content : contents) {
            content->writeImplementation(os);
        }
    }

    void writeLineageDestination(std::ostream& os)
    {
        if(!name.empty())
        {
            if(parent) {
                parent->writeLineageDestination(os);
            }

            os << name << "::";
        }
    }
};