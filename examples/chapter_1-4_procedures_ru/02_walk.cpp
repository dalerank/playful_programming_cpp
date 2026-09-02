// Example from: playfull_programming_chapter_1-4_procedures_ru.md
// Source line: 127
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_walk.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_walk 02_walk.cpp

#include "../common.h"

#include <cstdio>

// эта программа обязана упасть, в этом и смысл
int walk(int depth)
{
    char scratch[64];
    scratch[0] = (char)depth;
    if (depth % 16 == 0) {
        std::printf("%d\n", depth);
        std::fflush(stdout);
    }
    return walk(depth + 1) + scratch[0];
}

int main()
{
    return walk(1);
}
