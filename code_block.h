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
                code.push_back(repeat(" ",space)+"end_block_"+line.substr(6)+":");
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
        if(line.find("select ") < line.length())
        {
            line = line.substr(0, findDoubleDotsIdx(line));

            code.push_back(line.substr(0, line.find("select ")));
            code.back() += "[&]()->";
            code.back() += line.substr(line.find("select ")+7);
            code.push_back(repeat(" ",space)+"{");

            k++;

            while(sanitize(lines[k]).find("case ")==0)
            {
                size_t case_spaces = countSpaces(lines[k]);

                std::string cond = lines[k].substr(lines[k].find("case ")+5);
                cond = cond.substr(0, findDoubleDotsIdx(cond));

                code.push_back(repeat(" ",case_spaces)+"if("+cond+") {");

                if(sanitize(lines[k]).length() > findDoubleDotsIdx(sanitize(lines[k]))+1)
                {
                    std::string codeBlock = repeat(" ",case_spaces) + lines[k].substr(findDoubleDotsIdx(lines[k])+1);

                    for(const std::string& lineBlock : parseCodeBlock({codeBlock})) {
                        code.push_back(lineBlock);
                    }
                    k++;
                }
                else
                {
                    k++;

                    std::vector<std::string> block;

                    for(; k<lines.size() && countSpaces(lines[k])>case_spaces; k++) {
                        block.push_back(lines[k]);
                    }

                    for(const std::string& blockLine : parseCodeBlock(block)) {
                        code.push_back(blockLine);
                    }
                }

                code.push_back(repeat(" ",case_spaces)+"}");
            }

            std::cout << "line: " << lines[k-1] << std::endl;

            if(sanitize(lines[k]).find("default")!=0) {
                throw std::runtime_error("Error! Excepted default condition at select statement");
            }

            if(lines[k].length() > findDoubleDotsIdx(lines[k]))
            {
                std::string codeBlock = repeat(" ", countSpaces(lines[k])) + lines[k].substr(findDoubleDotsIdx(lines[k])+1);

                for(const std::string& lineBlock : parseCodeBlock({codeBlock})) {
                    code.push_back(lineBlock);
                }
                k++;
            }
            else
            {
                k++;

                std::vector<std::string> block;

                for(; k<lines.size() && countSpaces(lines[k])>space; k++) {
                    block.push_back(lines[k]);
                }

                k--;

                for(const std::string& blockLine : parseCodeBlock(block)) {
                    code.push_back(blockLine);
                }
            }

            code.push_back(repeat(" ",space)+"}();");
        }
        else
        if(line.find("while") == 0)
        {
            std::string part = line.substr(5);
            part = sanitize(part);

            if(part[0] != '(')
            {
                std::string cond = part.substr(0, findDoubleDotsIdx(part));
                code.push_back(repeat(" ", space)+"while("+cond+")");
            }
            else {
                std::string cond = part.substr(0, findDoubleDotsIdx(part));
                code.push_back(repeat(" ", space)+"while"+cond);
            }

            if(part.length() > findDoubleDotsIdx(part) + 1)
            {
                std::string codeBlock = part.substr(findDoubleDotsIdx(part)+1);

                code.push_back(repeat(" ", space)+"{");

                for(const std::string& lineBlock : parseCodeBlock({codeBlock})) {
                    code.push_back(lineBlock);
                }

                code.push_back(repeat(" ", space)+"}");
            }
        }
        else
        {
            code.push_back(lines[k]+";");
        }
    }

    return code;
}