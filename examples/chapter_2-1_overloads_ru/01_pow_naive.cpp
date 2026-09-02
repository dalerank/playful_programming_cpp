// Example from: playfull_programming_chapter_2-1_overloads_ru.md
// Source line: 37
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_pow_naive.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_pow_naive 01_pow_naive.cpp

#include "../common.h"

template <typename T>
T pow_naive(T x, unsigned n) {
    T result = T{1};              // 1. у типа есть нейтральный элемент
                                  //    умножения, записываемый как T{1}
    for (unsigned i = 0; i < n; ++i)
        result = result * x;      // 2. есть operator*, из двух T даёт T
                                  // 3. результат можно присвоить обратно
    return result;                // 4. значение копируется наружу
}

int main() { return 0; }
