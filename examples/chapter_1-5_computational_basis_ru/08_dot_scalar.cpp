// Example from: playfull_programming_chapter_1-5_computational_basis_ru.md
// Source line: 322
// Build (MSVC): cl /std:c++20 /EHsc /nologo 08_dot_scalar.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 08_dot_scalar 08_dot_scalar.cpp

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

// свёртка на int8 с накоплением в int32, как в инференсе сети
int32_t dot_scalar(const uint8_t *a, const int8_t *b, std::size_t n) {
    int32_t acc = 0;
    for (std::size_t i = 0; i < n; i++)
        acc += (int32_t)a[i] * (int32_t)b[i];
    return acc;
}

static int32_t hsum(__m256i v) {          // сложить восемь дорожек
    alignas(32) int32_t t[8];
    _mm256_store_si256((__m256i *)t, v);
    int32_t s = 0;
    for (int k = 0; k < 8; k++) s += t[k];
    return s;
}

// то же самое, но по 32 пары байт за одну инструкцию
int32_t dot_vnni(const uint8_t *a, const int8_t *b, std::size_t n) {
    __m256i acc = _mm256_setzero_si256();
    for (std::size_t i = 0; i < n; i += 32)
        acc = _mm256_dpbusd_avx_epi32(acc,
                  _mm256_loadu_si256((const __m256i *)(a + i)),
                  _mm256_loadu_si256((const __m256i *)(b + i)));
    return hsum(acc);
}

int main() { return 0; }
