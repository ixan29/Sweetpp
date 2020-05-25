#pragma once

#include <type_traits>
#include "utils.h"
#include "encapsulation.h"
#include "namespace.h"
#include "composite_type.h"
#include "inheritance.h"

struct Composite : public Content
{
    std::vector<Inheritance> inheritances;
    std::vector<std::string> templates, partialTemplates;
    CompositeType compositeType;
    std::string name;
    std::vector<std::shared_ptr<Content>> contents;
    bool isUnsafe;

    void writeTemplateLineage(std::ostream& os);
    void writeLineageDestination(std::ostream& os);
private:
    void* parent = nullptr;
    size_t parentTypeHashcode = typeid(void*).hash_code();

    void writeParentTemplateLineage(std::ostream& os)
    {
        if(parentTypeHashcode == typeid(Namespace).hash_code()) {
            //do nothing
        }
        else
        if(parentTypeHashcode == typeid(Composite).hash_code()) {
            static_cast<Composite*>(parent)->writeTemplateLineage(os);
        }
        else {
            throw std::runtime_error("Error at writing class lineage. Parent is neither a namespace or class");
        }
    }

    void writeParentLineageDestination(std::ostream& os)
    {
        if(parentTypeHashcode == typeid(Namespace).hash_code()) {
            static_cast<Namespace*>(parent)->writeLineageDestination(os);
        }
        else
        if(parentTypeHashcode == typeid(Composite).hash_code()) {
            static_cast<Composite*>(parent)->writeLineageDestination(os);
        }
        else {
            throw std::runtime_error("Error at writing class lineage. Parent is neither a namespace or class");
        }
    }

public:

    std::string getTemplatedName() const
    {
        std::ostringstream oss;
        oss << name;

        if(partialTemplates.size()>0) {
            oss << "<" << join(partialTemplates,",") << ">";
        }
        else
        if(templates.size()>0) {

            std::vector<std::string> simpler;
            simpler.reserve(templates.size());

            for(auto t : templates) {
                t = t.substr(t.find(' ')+1);
                t = t.substr(0, t.find('<'));
                simpler.push_back(t);
            }

            oss << "<" << join(simpler, ",") << ">";
        }

        return oss.str();
    }

    template<typename T>
    void setParent(T* parent) {
        this->parent = parent;
        this->parentTypeHashcode = typeid(T).hash_code();
    }

    virtual void writeDefinition(std::ostream& os)
    {
        if(templates.size()>0 || partialTemplates.size()>0) {
            os << "template<" << join(templates, ",") << ">" << std::endl;
        }

        os << compositeType << " " << name;
        
        if(partialTemplates.size()>0) {
            os << "<" << join(partialTemplates, ",") << ">";
        }

        if(inheritances.size()>0) {
            os << " : " << join(inheritances, ",") << std::endl;
        }

        os << std::endl;
        os << '{' << std::endl;

        for(auto& content : contents) {
            content->writeDefinition(os);
        }

        os << "};" << std::endl;
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
};

void Composite::writeTemplateLineage(std::ostream& os)
{
    writeParentTemplateLineage(os);
    
    if(templates.size()>0 || partialTemplates.size()>0) {
        os << "template<" << join(templates, ",") << ">" << std::endl;
    }
}

void Composite::writeLineageDestination(std::ostream& os)
{
    writeParentLineageDestination(os);
    os << name;

    os << getTemplatedName() << "::";
}