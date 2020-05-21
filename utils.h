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
    size_t min = countSpaces(str)+1;
    size_t max = str.find(' ', min);

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