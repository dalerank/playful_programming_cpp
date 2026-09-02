// Example from: playfull_programming_chapter_1-4_procedures_en.md
// Source line: 103
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_walk.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_walk 01_walk.cpp

#include "../common.h"

#include <cstdio>

int walk(int depth, const char *parent)
{
    char scratch[64];
    scratch[0] = (char)depth;
    if (parent)                       // print, do not assert
        std::printf("frame %d: %td bytes\n", depth, parent - scratch);
    if (depth == 4)
        return scratch[0];
    return walk(depth + 1, scratch);
}

int main()
{
    return walk(0, nullptr);
}
