#pragma once

#include "code_block.h"
#include "argument.h"
#include "template.h"
#include "namespace.h"

struct Function
{
    std::vector<std::string> templates, partialTemplates;
    Namespace* parent;
    Type returnType;
    std::string name;
    std::vector<Argument> arguments;
    std::vector<std::string> code;

    virtual void writeDefinition(std::ostream& os)
    {
        if(templates.size()>0 || partialTemplates.size()>0) {
            os << "template<" << join(templates, ",") << ">" << std::endl;
        }

        os << returnType << " " << name;

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

        os << "template<" << join(templates, ",") << ">" << std::endl;

        os << returnType << " ";
        
        parent->writeLineageDestination(os);
        os << name;

        if(partialTemplates.size()>0) {
            os << "<" << join(partialTemplates,",") << ">";
        }

        os << "(" << join(arguments, ",") << ")" << std::endl;

        os << "{" << std::endl;
        os << join(code,"\n") << std::endl;
        os << "}" << std::endl;
    }

    virtual void writeImplementation(std::ostream& os)
    {
        if(templates.size()>0 || partialTemplates.size()>0) {
            return;
        }

        os << returnType << " ";
        
        parent->writeLineageDestination(os);
        os << name;

        os << "(" << join(arguments, ",") << ")" << std::endl;

        os << "{" << std::endl;
        os << join(code,"\n") << std::endl;
        os << "}" << std::endl;
    }
};

std::vector<std::shared_ptr<Function>> parseFunction(const std::vector<std::string>& lines, Namespace* parent)
{
    std::unique_ptr<Function> function (new Function());

    function->parent = parent;

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
    function->code = parseCodeBlock(codeBlock);

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

    function->returnType = parseType(returnStr);

    function->name = parts[k];
    k++;

    if(parts[k].find('<') < parts[k].find('(')) {

        std::string templates = parts[k].substr(1, parts[k].length()-2);
        function->templates = parseTemplates(templates);

        k++;

        if(parts[k] == "specializes")
        {
            k++;
            auto partial = parts[k].substr(1, parts[k].length()-2);
            function->partialTemplates = smartSplit(partial, ',', true);
            k++;
        }
    }

    if(parts[k][0] != '(')
    {
        throw std::runtime_error(std::string("error at parsing function. Expected arguments but got ")+parts[k]);
    };

    std::string args = parts[k].substr(1);
    args = args.substr(0, args.find_last_of(')'));
    function->arguments = parseArguments(args);
    

    return {
        std::move(function)
    };
}