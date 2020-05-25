#pragma once

#include "code_block.h"
#include "encapsulation.h"
#include "argument.h"
#include "template.h"
#include "composite.h"

struct Method
{
    Encapsulation encapsulation;
    std::vector<std::string> templates, partialTemplates;
    Composite* parent;
    bool isStatic = false;
    bool isVirtual = false;
    Type returnType;
    std::string name;
    std::vector<Argument> arguments;
    std::vector<std::string> constructorInitializers;
    std::vector<std::string> code;

    bool isConstructor() const {
        return name == parent->name;
    }

    bool isDestructor() const {
        return name == "~"+parent->name;
    }

    virtual void writeDefinition(std::ostream& os)
    {
        os << encapsulation << ":" << std::endl;
        
        if(templates.size()>0 || partialTemplates.size()>0) {
            os << "template<" << join(templates, ",") << ">" << std::endl;
        }

        if(isStatic) {
            os << "static ";
        }

        if(isVirtual) {
            os << "virtual ";
        }

        if(!isConstructor() && !isDestructor()) {
            os << returnType << " ";
        }
        
        os << name;

        if(partialTemplates.size()>0) {
            os << "<" << join(partialTemplates,",") << ">";
        }

        os << "(" << join(arguments, ",") << ");" << std::endl;
    }

    virtual void writeTemplateImplementation(std::ostream& os)
    {
        if(templates.size()==0 && partialTemplates.size()==0) {
            return;
        }

        parent->writeTemplateLineage(os);
        os << "template<" << join(templates, ",") << ">" << std::endl;

        if(!isConstructor() && !isDestructor()) {
            os << returnType << " ";
        }
        
        parent->writeLineageDestination(os);
        os << name;

        if(partialTemplates.size()>0) {
            os << "<" << join(partialTemplates,",") << ">";
        }

        os << "(" << join(arguments, ",") << ")" << std::endl;

        if(constructorInitializers.size()>0) {
            os << ":" << join(constructorInitializers, ",\n") << std::endl;
        }

        os << "{" << std::endl;
        os << join(code,"\n") << std::endl;
        os << "}" << std::endl;
    }

    virtual void writeImplementation(std::ostream& os)
    {
        if(templates.size()>0 || partialTemplates.size()>0) {
            return;
        }
        
        if(!isConstructor() && !isDestructor()) {
            os << returnType << " ";
        }

        parent->writeLineageDestination(os);
        os << name;

        os << "(" << join(arguments, ",") << ")" << std::endl;

        if(constructorInitializers.size()>0) {
            os << ":" << join(constructorInitializers, ",\n") << std::endl;
        }

        os << "{" << std::endl;
        os << join(code,"\n") << std::endl;
        os << "}" << std::endl;
    }
};

std::vector<std::shared_ptr<Method>> parseMethod(const std::vector<std::string>& lines, Composite* parent)
{
    std::unique_ptr<Method> method (new Method());

    method->parent = parent;
    method->encapsulation = getDefaultEncapsulation(parent->compositeType);

    std::string methodInfo = sanitize(lines[0]);
    size_t spaces = countSpaces(lines[0]);

    size_t k=1;

    while(countSpaces(lines[k])==spaces)
    {
        methodInfo += ' ';
        methodInfo += sanitize(lines[k]);
        k++;
    }

    std::vector<std::string> codeBlock (lines.begin()+k, lines.end());
    method->code = parseCodeBlock(codeBlock);

    methodInfo = replace(methodInfo, "<", " <");
    methodInfo = replace(methodInfo, ">", "> ");
    methodInfo = replace(methodInfo, "(", " (");
    methodInfo = replace(methodInfo, ")", ") ");

    std::vector<std::string> parts;
    
    for(std::string& part : smartSplit(methodInfo, ' ', true)) {
        if(!part.empty()) {
            parts.push_back(std::move(part));
        }
    }

    k=0;

    std::string returnStr;

    if( parts[k] == "private"
    ||  parts[k] == "protected"
    ||  parts[k] == "public") {
        method->encapsulation = parseEncapsulation(parts[k]);
        k++;
    }

    if(parts[k] == "static") {
        method->isStatic = true;
        k++;
    }

    if( parts[k] == "virtual"
    ||  parts[k] == "override") {
        method->isVirtual = true;
        k++;
    }

    while(  parts[k] == "const"
    ||      parts[k] == "constexpr") {
        returnStr += parts[k];
        returnStr += ' ';
        k++;
    }

    returnStr += parts[k];

    k++;

    //if someone likes to put a space between templates for types
    if(parts[k].find('<') < parts[k].length())
    {
        returnStr += ' ';
        returnStr += parts[k];
        k++;
    }

    method->returnType = parseType(returnStr);

    if(method->returnType.name == "this&")
    {
        method->returnType.name = parent->getTemplatedName()+"&";

        size_t spaces = minSpace(method->code);
        method->code.push_back(repeat(" ", spaces)+"return *this;");
    }
    else
    if(method->returnType.name == "this*")
    {
        method->returnType.name = parent->getTemplatedName()+"*";

        size_t spaces = minSpace(method->code);
        method->code.push_back(repeat(" ", spaces)+"return this;");
    }

    method->name = parts[k];

    if(method->name == "init") {
        method->name = parent->name;
    }
    else
    if(method->name == "delete") {
        method->name = "~" + parent->name;
    }
    
    k++;

    if(parts[k].find('<') < parts[k].find('<')) {

        std::string templates = parts[k].substr(1, parts[k].length()-2);
        method->templates = parseTemplates(templates);

        k++;

        if(parts[k] == "specializes")
        {
            k++;
            auto partial = parts[k].substr(1, parts[k].length()-2);
            method->partialTemplates = smartSplit(partial, ',', true);
            k++;
        }
    }

    if(parts[k][0] != '(')
    {
        throw std::runtime_error(std::string("error at parsing method. Expected arguments but got ")+parts[k]);
    };

    std::string args = parts[k].substr(1);
    args = args.substr(0, args.find_last_of(')'));
    method->arguments = parseArguments(args);
    

    return {
        std::move(method)
    };
}