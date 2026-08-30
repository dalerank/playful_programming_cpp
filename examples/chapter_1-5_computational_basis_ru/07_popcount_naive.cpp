// Example from: playfull_programming_chapter_1-5_computational_basis_ru.md
// Source line: 289
// Build (MSVC): cl /std:c++20 /EHsc /nologo 07_popcount_naive.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 07_popcount_naive 07_popcount_naive.cpp

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

// в лоб: тридцать два прохода по битам
int popcount_naive(uint32_t x) {
    int n = 0;
    for (int i = 0; i < 32; i++)
        n += (x >> i) & 1u;
    return n;
}

// трюк с масками, он же SWAR: складываем биты парами, потом тетрадами
int popcount_swar(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0F0F0F0Fu;
    return (int)((x * 0x01010101u) >> 24);
}

// и то, во что это превратилось
int popcount_today(uint32_t x) {
    return std::popcount(x);
}

int main() { return 0; }
