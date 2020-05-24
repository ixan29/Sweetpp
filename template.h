#pragma once

#include "utils.h"

bool isTemplateArgument(const std::string& str)
{
    std::string line = sanitize(str);

    return  line == "bool"
    ||      line == "int"
    ||      line == "unsigned"
    ||      line == "long"
    ||      line == "size_t";
}

std::vector<std::string> parseTemplates(const std::string& str)
{
    std::vector<std::string> templates;

    auto split = smartSplit(sanitize(str), ',', true);

    for(auto& s : split) {
        s = sanitize(s);
    }

    if(split.size()>=2) {
        for(auto& s : split) {
            auto temps = parseTemplates(s);
            templates.insert(templates.end(), temps.begin(), temps.end());
        }
    }
    else
    if(split[0].find('<')<split[0].length())
    {
        std::vector<std::string> templatesTemplates;

        auto part = split[0].substr(split[0].find('<')+1);
        part = sanitize(part.substr(0, part.find_last_of('>')));

        for(auto& s : smartSplit(part, ',', true))
        {
            s = sanitize(s);

            if(s != "class" && s != "typename" && !isTemplateArgument(split[0]))
            {
                auto temps = parseTemplates(s);
                templates.insert(templates.end(), temps.begin(), temps.end());
            }

            if(s.find(' ') < s.find('<')) {
                templatesTemplates.push_back(s.substr(0, s.find(' ')));
            }
            else {
                templatesTemplates.push_back("class");
            }
        }

        templates.push_back(split[0].substr(0, split[0].find('<'))+"<"+join(templatesTemplates,",")+">");
    }
    else
    if(split[0].find_last_of(' ') < split[0].length())
    {
        templates.push_back(split[0]);
    }
    else
    if(split[0].find("...") < split.size()-3)
    {
        templates.push_back("class... "+split[0].substr(0, split[0].find("...")));
    }
    else
    {
        templates.push_back("class "+split[0]);
    }

    return templates;
}