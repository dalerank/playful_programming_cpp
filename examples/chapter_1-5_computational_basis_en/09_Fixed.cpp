// Example from: playfull_programming_chapter_1-5_computational_basis_en.md
// Source line: 360
// Build (MSVC): cl /std:c++20 /EHsc /nologo 09_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 09_Fixed 09_Fixed.cpp

#include "../common.h"

struct Fixed {                        // that same type from the opening of the chapter
    int32_t raw;
};

// division on the instruction the 8086 brought back in 1978
Fixed divide(Fixed a, Fixed b) {
    return Fixed{(int32_t)(((int64_t)a.raw << 16) / b.raw)};
}

int main() { return 0; }
