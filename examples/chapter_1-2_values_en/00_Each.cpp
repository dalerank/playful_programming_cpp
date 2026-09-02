// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 21
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Each.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Each 00_Each.cpp

#include "../common.h"

void example() {
    // Each of these bit patterns is meaningful as int
        uint32_t bits1 = 0x00000001u;
        uint32_t bits2 = 0xFFFFFFFFu;

        int32_t a, b;
        std::memcpy(&a, &bits1, 4); // 1
        std::memcpy(&b, &bits2, 4); // -1  (two's complement)
}

int main() {
    example();
    return 0;
}
