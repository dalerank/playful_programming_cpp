// Example from: playfull_programming_chapter_1-1_generalizations_en.md
// Source line: 77
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_successor.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_successor 01_successor.cpp

#include "../common.h"

// Kind: natural number
using Natural = unsigned int;

// Successor function: n → n + 1
// Rule that maps each Natural to the next Natural
Natural successor(Natural n) {
    return n + 1;
}

int main() { return 0; }
