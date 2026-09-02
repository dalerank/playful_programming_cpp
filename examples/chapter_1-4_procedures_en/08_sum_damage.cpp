// Example from: playfull_programming_chapter_1-4_procedures_en.md
// Source line: 328
// Build (MSVC): cl /std:c++20 /EHsc /nologo 08_sum_damage.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 08_sum_damage 08_sum_damage.cpp

#include "../common.h"

#include <cstddef>

// input/output in pure form: *total is read and written every iteration
void sum_damage(const int *damage, std::size_t n, int *total) {
    for (std::size_t i = 0; i < n; i++)
        *total += damage[i];
}

// the same algorithm, but the accumulator is local and overlaps nothing
void sum_damage_local(const int *damage, std::size_t n, int *total) {
    int acc = *total;
    for (std::size_t i = 0; i < n; i++)
        acc += damage[i];
    *total = acc;
}

int main() { return 0; }
