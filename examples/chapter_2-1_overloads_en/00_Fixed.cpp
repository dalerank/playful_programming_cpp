// Example from: playfull_programming_chapter_2-1_overloads_en.md
// Source line: 7
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Fixed 00_Fixed.cpp

#include "../common.h"

struct Fixed { int32_t raw; };            // 16.16, that exact type from the previous chapter

Fixed operator*(Fixed a, Fixed b) {       // multiplied and put the point back
    return Fixed{(int32_t)(((int64_t)a.raw * b.raw) >> 16)};
}

template <typename T>
T pow_naive(T x, unsigned n) {
    T result = T{1};                      // <- what happens here now?
    for (unsigned i = 0; i < n; ++i)
        result = result * x;
    return result;
}

void damage_bonus() {
    Fixed per_stack{(int32_t)(1.1 * 65536)};     // +10% per stack, raw == 72089
    Fixed mult = pow_naive(per_stack, 13);       // expecting a multiplier around 3.45
    printf("raw=%d\n", mult.raw);                // raw=1, meaning 0.0000153
}

int main() { return 0; }
