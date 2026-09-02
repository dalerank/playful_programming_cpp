// Example from: playfull_programming_chapter_1-5_computational_basis_en.md
// Source line: 173
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_copy_by_hand.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_copy_by_hand 04_copy_by_hand.cpp

#include "../common.h"

void copy_by_hand(char *dst, const char *src, std::size_t n) {
    for (std::size_t i = 0; i < n; i++)
        dst[i] = src[i];
}

void copy_today(char *dst, const char *src, std::size_t n) {
    std::memcpy(dst, src, n);
}

int main() { return 0; }
