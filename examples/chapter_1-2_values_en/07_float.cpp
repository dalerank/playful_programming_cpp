// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 268
// Build (MSVC): cl /std:c++20 /EHsc /nologo 07_float.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 07_float 07_float.cpp

#include "../common.h"

void example() {
    float a = 1e15, b = -1e15, c = 1.5;

    float left  = (a + b) + c; // (1e15 - 1e15) + 1.5 = 0 + 1.5 = 1.5
    float right = a + (b + c); // 1e15 + (-1e15 + 1.5), precision loss

    std::cout << "\n(a+b)+c = " << left  << "\n"; // 1.5
    std::cout << "a+(b+c) = " << right << "\n"; // may differ
    std::cout << "Equal:   " << (left == right) << "\n";
    // With -ffast-math the compiler may reorder and get a different result
}

int main() {
    example();
    return 0;
}
