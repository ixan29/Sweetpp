#include <iostream>
#include "utils.h"
#include "code_block.h"
#include "fileRead.h"

int main()
{
    auto lines = readFileLines("../test.spp");

    for(auto& line : parseCodeBlock(lines)) {
        std::cout << line << std::endl;
    }

    return 0;
}