#pragma once

#include "utils.h"
#include "argument.h"

std::vector<std::string> parseCodeBlock(const std::vector<std::string>& lines)
{
    std::vector<std::string> code;
    std::vector<char> stack;

    size_t space = minSpace(lines);

    size_t k;

    for(k=0; k<lines.size(); k++)
    {
        std::string line = sanitize(lines[k]);

        if(stack.empty() && countSpaces(lines[k]) > space)
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
            line = sanitize(line.substr(0, findDoubleDotsIdx(line)));

            if(line.length()>5)
            {
                code.push_back(repeat(" ",space)+"/*"+line+"*/");
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
        if(line.find("choose ") < line.length())
        {
            line = line.substr(0, findDoubleDotsIdx(line));

            code.push_back(line.substr(0, line.find("choose ")));
            code.back() += "[&]()->";
            code.back() += line.substr(line.find("choose ")+7);
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
                throw std::runtime_error("Error! Excepted default condition at choose statement");
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

            code.push_back(repeat(" ",space)+"}()");

            if(stack.empty()) {
                code.back() += ";";
            }
        }
        else
        if(line.find("if ") == 0 || line.find("if(") == 0)
        {
            std::string part = line.substr(2);
            part = sanitize(part);
            std::string cond = part.substr(0, findDoubleDotsIdx(part));

            if(part[0] != '(')
            {
                code.push_back(repeat(" ", space)+"if("+cond+")");
            }
            else {
                code.push_back(repeat(" ", space)+"if"+cond);
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
            else
            if(findDoubleDotsIdx(part) == part.length())
            {
                code.back() += ";";
            }
        }
        else
        if(line.find("else ") == 0)
        {
            std::string part = line.substr(4);
            part = sanitize(part);

            if(part.find("if") == 0)
            {
                part = part.substr(2);
                part = sanitize(part);
                std::string cond = part.substr(0, findDoubleDotsIdx(part));

                if(part[0] != '(')
                {
                    code.push_back(repeat(" ", space)+"else if("+cond+")");
                }
                else {
                    code.push_back(repeat(" ", space)+"else if"+cond);
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
                else
                if(findDoubleDotsIdx(part) == part.length())
                {
                    code.back() += ";";
                }
            }
            else
            if(part.length() > findDoubleDotsIdx(part) + 1)
            {
                std::string codeBlock = part.substr(findDoubleDotsIdx(part)+1);

                code.push_back(repeat(" ", space)+"else");
                code.push_back(repeat(" ", space)+"{");

                for(const std::string& lineBlock : parseCodeBlock({codeBlock})) {
                    code.push_back(lineBlock);
                }

                code.push_back(repeat(" ", space)+"}");
            }
            else
            {
                code.push_back(repeat(" ", space)+"else");
            }
        }
        else
        if(line.find("while ") == 0 || line.find("while(")==0)
        {
            std::string part = line.substr(5);
            part = sanitize(part);
            std::string cond = part.substr(0, findDoubleDotsIdx(part));

            if(part[0] != '(')
            {
                code.push_back(repeat(" ", space)+"while("+cond+")");
            }
            else {
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
            else
            if(findDoubleDotsIdx(part) == part.length())
            {
                code.back() += ";";
            }
        }
        else
        if(line.find("do") == 0)
        {
            std::string part = line.substr(2);
            part = sanitize(part);

            if(part[0]==':')
            {
                code.push_back(repeat(" ",space)+"do");

                if(part.size()>1)
                {
                    code.push_back(repeat(" ",space)+"{");

                    for(const std::string& lineBlock : parseCodeBlock({part.substr(1)})) {
                        code.push_back(lineBlock);
                    }

                    code.push_back(repeat(" ",space)+"};");
                }
            }
            else
            {
                code.push_back(lines[k]);
            }
        }
        else
        if(line.find("for ") == 0 || line.find("for(") == 0)
        {
            std::string part = line.substr(3);
            part = sanitize(part);
            std::string cond = part.substr(0, findDoubleDotsIdx(part));
            cond = replace(cond," in ", " : ");

            if(part[0] != '(')
            {
                code.push_back(repeat(" ", space)+"for("+cond+")");
            }
            else {
                code.push_back(repeat(" ", space)+"for"+cond);
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
            else
            if(findDoubleDotsIdx(part) == part.length())
            {
                code.back() += ";";
            }
        }
        else
        if( line.find(']') < line.find('(')
        &&  sanitize(
                line.substr(
                    line.find(']')+1,
                    line.find('(')-line.find(']')-1
                )
            ).empty()
        ) //detected a lambda
        {
            code.push_back(lines[k].substr(0, lines[k].find(']')+1));

            std::string strArgs = line.substr(line.find('(')+1);
            strArgs = strArgs.substr(0, findDoubleDotsIdx(strArgs));
            strArgs = strArgs.substr(0, strArgs.find_last_of(')'));

            auto args = parseArguments(strArgs);
            code.back() += '(';
            code.back() += join(args, ",");
            code.back() += ')';

            std::string rest = line.substr(line.find(')')+1);
            rest = rest.substr(0, findDoubleDotsIdx(rest));
            code.back() += rest;

            if(line.length() > findDoubleDotsIdx(line)+1)
            {
                std::string codeLine = repeat(" ",space) + line.substr(findDoubleDotsIdx(line)+1);

                code.push_back(repeat(" ",space)+"{");

                for(auto& line2 : parseCodeBlock({codeLine})) {
                    code.push_back(line2);
                }

                code.push_back(repeat(" ",space)+"}");

                if(stack.empty()) {
                    code.back() += ";";
                }
            }
            else
            {
                size_t lambda_space = countSpaces(lines[k]);

                std::vector<std::string> block;

                k++;

                for(; k<lines.size() && countSpaces(lines[k])>lambda_space; k++) {
                    block.push_back(lines[k]);
                }

                k--;

                code.push_back(repeat(" ",lambda_space)+"{");

                for(const std::string& blockLine : parseCodeBlock(block)) {
                    code.push_back(blockLine);
                }

                code.push_back(repeat(" ",lambda_space)+"}");

                if(stack.empty()) {
                    code.back() += ';';
                }
            }
        }
        else
        {
            code.push_back("");

            for(size_t i=0 ; i<lines[k].length() ; i++)
            {
                switch(lines[k][i])
                {
                    case '(':
                        stack.push_back('(');
                        code.back() += '(';
                        break;

                    case ')':
                        if(stack.back()=='(')
                        {
                            stack.pop_back();
                            code.back() += ')';
                        }
                        else
                        {
                            throw std::runtime_error(std::string("Error at smartSplit: expected ')' but got '")+stack.back()+"'");
                        }

                        break;

                    case '[':
                        stack.push_back('[');
                        code.back() += '[';
                        break;

                    case ']':
                        if(stack.back()=='[')
                        {
                            stack.pop_back();
                            code.back() += ']';
                        }
                        else
                        {
                            throw std::runtime_error(std::string("Error at smartSplit: expected ']' but got '")+stack.back()+"'");
                        }

                        break;

                    case '{':
                        stack.push_back('{');
                        code.back() += '{';
                        break;

                    case '}':
                        if(stack.back()=='{')
                        {
                            stack.pop_back();
                            code.back() += '}';
                        }
                        else
                        {
                            throw std::runtime_error(std::string("Error at smartSplit: expected '}' but got '")+stack.back()+"'");
                        }

                        break;

                    default:
                        code.back() += lines[k][i];
                }
            }
            
            if(stack.empty()) {
                code.back() += ";";
            }
        }
    }

    return code;
}