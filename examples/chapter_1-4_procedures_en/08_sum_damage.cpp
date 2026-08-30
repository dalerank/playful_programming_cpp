// Example from: playfull_programming_chapter_1-4_procedures_en.md
// Source line: 328
// Build (MSVC): cl /std:c++20 /EHsc /nologo 08_sum_damage.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 08_sum_damage 08_sum_damage.cpp

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

#include <cstddef>

// input/output in pure form: *total is read and written every iteration
void sum_damage(const int *damage, std::size_t n, int *total) {
    for (std::size_t i = 0; i < n; i++)
        *total += damage[i];
}

// the same algorithm, but the accumulator is local and overlaps nothing
void sum_damage_local(const int *damage, std::size_t n, int *total) {
    int acc = *total;
    for (std::size_t i = 0; i < n; i++)
        acc += damage[i];
    *total = acc;
}

int main() { return 0; }
