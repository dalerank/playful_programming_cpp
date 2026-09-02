// Example from: playfull_programming_chapter_2-1_overloads_en.md
// Source line: 37
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_pow_naive.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_pow_naive 01_pow_naive.cpp

#include "../common.h"

template <typename T>
T pow_naive(T x, unsigned n) {
    T result = T{1};              // 1. type has a multiplicative identity,
                                  //    written as T{1}
    for (unsigned i = 0; i < n; ++i)
        result = result * x;      // 2. has operator*, producing T from two T values
                                  // 3. result can be assigned back
    return result;                // 4. value is copied out
}

int main() { return 0; }
