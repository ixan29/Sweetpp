#pragma once

#include <string>
#include <vector>
#include <cmath>

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
    
    int max = str.length();

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