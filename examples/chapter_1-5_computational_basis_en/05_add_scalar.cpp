// Example from: playfull_programming_chapter_1-5_computational_basis_en.md
// Source line: 228
// Build (MSVC): cl /std:c++20 /EHsc /nologo 05_add_scalar.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 05_add_scalar 05_add_scalar.cpp

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <string_view>
#include <regex>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <deque>
#include <queue>
#include <future>
#include <list>
#include <memory>
#include <memory_resource>
#include <utility>
#include <algorithm>
#include <numeric>
#include <functional>
#include <optional>
#include <variant>
#include <tuple>
#include <type_traits>
#include <iterator>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <system_error>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <new>
#include <limits>
#include <concepts>
#include <ranges>
#include <span>
#include <bit>
#include <coroutine>
#if __has_include(<generator>)
#include <generator>
#endif
#include <immintrin.h>

void add_scalar(float *dst, const float *a, const float *b, std::size_t n) {
    for (std::size_t i = 0; i < n; i++)
        dst[i] = a[i] + b[i];
}

// n is a multiple of four
void add_sse(float *dst, const float *a, const float *b, std::size_t n) {
    for (std::size_t i = 0; i < n; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        _mm_storeu_ps(dst + i, _mm_add_ps(va, vb));
    }
}

int main() { return 0; }
