// Example from: playfull_programming_chapter_1-5_computational_basis_ru.md
// Source line: 128
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Baby.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Baby 03_Baby.cpp

#include "../common.h"

// у Baby из арифметики было только вычитание
unsigned int add_via_sub(unsigned int a, unsigned int b) {
    return a - (0u - b);   // те же a + b
}

int main() { return 0; }
