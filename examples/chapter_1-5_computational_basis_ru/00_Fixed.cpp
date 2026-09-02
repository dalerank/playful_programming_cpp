// Example from: playfull_programming_chapter_1-5_computational_basis_ru.md
// Source line: 5
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Fixed 00_Fixed.cpp

#include "../common.h"

// 16.16, детерминированная арифметика для реплеев
struct Fixed {
    int32_t raw;
};

// деление в лоб: вычитаем делитель, пока есть что вычитать
// (и это ещё только для положительных, с отрицательными всё веселее)
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
