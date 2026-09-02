// Example from: playfull_programming_chapter_1-5_computational_basis_en.md
// Source line: 128
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_the.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_the 03_the.cpp

#include "../common.h"

// on the Baby the only arithmetic was subtraction
unsigned int add_via_sub(unsigned int a, unsigned int b) {
    return a - (0u - b);   // the same as a + b
}

int main() { return 0; }
