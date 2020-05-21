#pragma once

#include <fstream>
#include <vector>
#include <string>

std::vector<std::string> readFileLines(const std::string& fileDest)
{
    std::vector<std::string> lines {""};

    std::ifstream ifs(fileDest);

    if(ifs.is_open())
    {
        while(getline(ifs,lines.back())) {
            if(!lines.back().empty()) {
                lines.push_back("");
            }
        }

        ifs.close();
    }

    lines.pop_back();

    return lines;
}