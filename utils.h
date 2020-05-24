#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <sstream>

size_t countSpaces(const std::string& str)
{
    size_t spaces;

    for(spaces=0;spaces<str.length() && str[spaces] == ' ' ; spaces++);

    if(spaces>0) {
        spaces--;
    }

    return spaces;
}

size_t minSpace(const std::vector<std::string>& lines)
{
    size_t spaces = 10000;

    for(const std::string& str : lines) {
        spaces = std::min(spaces, countSpaces(str));
    }

    return spaces;
}

std::string sanitize(const std::string& str)
{
    size_t min = str[0]==' ' ? countSpaces(str)+1 : 0;
    
    int max = str.length()-1;

    while(max>=0 && str[max] == ' ') {
        max--;
    }

    max++;

    return str.substr(min, max-min);
}

std::string repeat(const std::string& str, size_t times)
{
    std::string repeat;
    repeat.reserve(str.length()*times);

    for(size_t k=0; k<times; k++) {
        repeat += str;
    }

    return repeat;
}

size_t findDoubleDotsIdx(const std::string& str)
{
    for(size_t idx=0; idx<str.length();idx++) {
        if(str[idx]==':') {
            if(str[idx+1]==':') {
                idx++;
            }
            else {
                return idx;
            }
        }
    }

    return str.length();
}

std::string replace(const std::string& str, const std::string& oldSeq, const std::string& newSeq)
{
    std::string replace;

    size_t idx=0;
    size_t new_idx;

    while((new_idx = str.find(oldSeq, idx)) < str.length())
    {
        replace += str.substr(idx, new_idx - idx);
        replace += newSeq;
        idx = new_idx + oldSeq.length();
    }

    replace += str.substr(idx);

    return replace;
}

std::vector<std::string> smartSplit(const std::string& str, const char delim, const bool checkAngles)
{
    std::vector<std::string> split {""};

    std::vector<char> stack;

    for(size_t k=0 ; k<str.length() ; k++)
    {
        switch(str[k])
        {
            case '(':
                stack.push_back('(');
                split.back() += '(';
                break;

            case ')':
                if(stack.back()=='(')
                {
                    stack.pop_back();
                    split.back() += ')';
                }
                else
                {
                    throw std::runtime_error(std::string("Error at smartSplit: expected ')' but got '")+stack.back()+"'");
                }

                break;

            case '[':
                stack.push_back('[');
                split.back() += '[';
                break;

            case ']':
                if(stack.back()=='[')
                {
                    stack.pop_back();
                    split.back() += ']';
                }
                else
                {
                    throw std::runtime_error(std::string("Error at smartSplit: expected ']' but got '")+stack.back()+"'");
                }

                break;

            case '{':
                stack.push_back('{');
                split.back() += '{';
                break;

            case '}':
                if(stack.back()=='{')
                {
                    stack.pop_back();
                    split.back() += '}';
                }
                else
                {
                    throw std::runtime_error(std::string("Error at smartSplit: expected '}' but got '")+stack.back()+"'");
                }

                break;

            case '<':
                if(checkAngles)
                {
                    /*
                    if( k+1<str.length()
                    &&  str[k+1] == '<')
                    {
                        k++;
                        split.back() += "<<";
                    }
                    else*/
                    {
                        stack.push_back('<');
                        split.back() += '<';
                    }
                }
                else
                {
                    split.back() += '<';
                }

                break;

            case '>':
                if(checkAngles)
                {
                    /*
                    if( k+1<str.length()
                    &&  str[k+1] == '>')
                    {
                        k++;
                        split.back() += ">>";
                    }
                    else
                    {
                    */
                        if(stack.back()== '<')
                        {
                            stack.pop_back();
                            split.back() += '>';
                        }
                        else
                        {
                            throw std::runtime_error(std::string("Error at smartSplit: expected '>' but got '")+stack.back()+"'");
                        }
                    //}
                }
                else
                {
                    split.back() += '<';
                }

                break;

            default:
                if(str[k] == delim)
                {
                    if(stack.empty())
                    {
                        split.push_back("");
                    }
                    else
                    {
                        split.back() += delim;
                    }
                }
                else
                {
                    split.back() += str[k];
                }

        }
    }

    if(!stack.empty())
    {
        throw std::runtime_error("Error at smartSplit: The text is not well formated");
    }

    return split;
}

template<typename T>
std::string join(const std::vector<T>& vec, const std::string& del)
{
    if(vec.empty()) {
        return "";
    }

    std::ostringstream oss;
    oss << vec[0];

    for(size_t k=1; k<vec.size(); k++) {
        oss << del << vec[k];
    }

    return oss.str();
}

template<typename Iterator>
std::string join(Iterator begin, Iterator end, const std::string& delim)
{
    if(begin==end) {
        return "";
    }

    std::ostringstream oss;

    oss << *begin;

    for(Iterator it = begin+1; it!=end; ++it) {
        oss << delim << *it;
    }

    return oss.str();
}