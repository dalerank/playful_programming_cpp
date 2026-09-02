// Example from: playfull_programming_chapter_1-5_computational_basis_ru.md
// Source line: 228
// Build (MSVC): cl /std:c++20 /EHsc /nologo 05_add_scalar.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 05_add_scalar 05_add_scalar.cpp

#include "../common.h"

void add_scalar(float *dst, const float *a, const float *b, std::size_t n) {
    for (std::size_t i = 0; i < n; i++)
        dst[i] = a[i] + b[i];
}

// n кратно четырём
void add_sse(float *dst, const float *a, const float *b, std::size_t n) {
    for (std::size_t i = 0; i < n; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        _mm_storeu_ps(dst + i, _mm_add_ps(va, vb));
    }
}

int main() { return 0; }
