// Example from: playfull_programming_chapter_1-5_computational_basis_en.md
// Source line: 5
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Fixed 00_Fixed.cpp

#include "../common.h"

// 16.16, deterministic arithmetic for replays
struct Fixed {
    int32_t raw;
};

// division the blunt way: subtract the divisor while there is something to subtract
// (and this is still only for positives; with negatives it gets more fun)
Fixed divide(Fixed a, Fixed b) {
    int64_t rem = (int64_t)a.raw << 16;
    int64_t d = b.raw;
    int32_t q = 0;
    while (rem >= d) {
        rem -= d;
        ++q;
    }
    return Fixed{q};
}

int main() { return 0; }
