// Example from: playfull_programming_chapter_2-1_overloads_en.md
// Source line: 139
// Build (MSVC): cl /std:c++20 /EHsc /nologo 05_pow_fast.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 05_pow_fast 05_pow_fast.cpp

#include "../common.h"

template <typename T>
T pow_fast(T x, unsigned n) {
   T result = T{1};

   while (n > 0) {
       if (n & 1)
           result = result * x;

       x = x * x;
       n >>= 1;
   }

   return result;
}

int main() { return 0; }
