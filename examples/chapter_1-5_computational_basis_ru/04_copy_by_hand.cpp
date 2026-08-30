// Example from: playfull_programming_chapter_1-5_computational_basis_ru.md
// Source line: 173
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_copy_by_hand.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_copy_by_hand 04_copy_by_hand.cpp

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

void copy_by_hand(char *dst, const char *src, std::size_t n) {
    for (std::size_t i = 0; i < n; i++)
        dst[i] = src[i];
}

void copy_today(char *dst, const char *src, std::size_t n) {
    std::memcpy(dst, src, n);
}

int main() { return 0; }
