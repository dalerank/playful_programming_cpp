// Example from: playfull_programming_chapter_1-5_computational_basis_ru.md
// Source line: 95
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_Fixed 02_Fixed.cpp

#include "../common.h"

struct Fixed {                        // тот самый тип из начала главы
    int32_t raw;
};

bool less_raw(Fixed a, Fixed b) { return a.raw < b.raw; }

struct FixedOrdered {                 // он же плюс одна строчка
    int32_t raw;
    auto operator<=>(const FixedOrdered &) const = default;
};

static_assert(!std::totally_ordered<Fixed>);
static_assert(std::totally_ordered<FixedOrdered>);

void sort_bare(std::vector<Fixed> &v) {
    std::sort(v.begin(), v.end(),
              [](Fixed a, Fixed b) { return less_raw(a, b); });   // лямбда в каждом вызове
}

void sort_ordered(std::vector<FixedOrdered> &v) {
    std::sort(v.begin(), v.end());                            // лучше
}

int main() { return 0; }
