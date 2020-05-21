#pragma once

#include "utils.h"

std::vector<std::string> parseCodeBlock(const std::vector<std::string>& lines)
{
    std::vector<std::string> code;

    size_t space = minSpace(lines);

    size_t k;

    for(k=0; k<lines.size(); k++)
    {
        std::string line = sanitize(lines[k]);

        if(countSpaces(lines[k]) > space)
        {
            std::vector<std::string> block;

            for(; k<lines.size() && countSpaces(lines[k])>space; k++) {
                block.push_back(lines[k]);
            }

            k--;

            code.push_back(repeat(" ",space)+"{");

            for(const std::string& blockLine : parseCodeBlock(block)) {
                code.push_back(blockLine);
            }

            code.push_back(repeat(" ",space)+"}");
        }
        else
        if(line.find("block ")==0)
        {
            line = line.substr(0, findDoubleDotsIdx(line));

            code.push_back(repeat(" ",space)+"/*"+line+"*/");

            if(line.length()>6)
            {
                k++;
                std::vector<std::string> block;

                for(; k<lines.size() && countSpaces(lines[k])>space; k++) {
                    block.push_back(lines[k]);
                }

                k--;

                code.push_back(repeat(" ",space)+"{");

                for(const std::string& blockLine : parseCodeBlock(block)) {
                    code.push_back(blockLine);
                }

                code.push_back(repeat(" ",space)+"}");
                code.push_back(repeat(" ",space)+":end_block_"+line.substr(6));
            }
        }
        else
        if(line.find("break ")==0)
        {
            if(line.length()>6)
            {
                code.push_back(repeat(" ",space)+"goto end_block_"+line.substr(6)+";");
            }
            else
            {
                code.push_back(lines[k]+";");
            }
        }
        else
        {
            code.push_back(lines[k]+";");
        }
    }

    return code;
}