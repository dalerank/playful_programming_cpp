// Example from: playfull_programming_chapter_2-1_overloads_en.md
// Source line: 200
// Build (MSVC): cl /std:c++20 /EHsc /nologo 06_pow_fast_careful.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 06_pow_fast_careful 06_pow_fast_careful.cpp

#include "../common.h"

template <typename T>
T pow_fast_careful(T x, unsigned n) {
   T result = T{1};

   while (n > 0) {
       if (n & 1)
           result = result * x;

       n >>= 1;

       if (n)
           x = x * x;
   }

   return result;
}

int main() { return 0; }
