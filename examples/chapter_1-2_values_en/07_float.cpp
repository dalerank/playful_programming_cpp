// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 268
// Build (MSVC): cl /std:c++20 /EHsc /nologo 07_float.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 07_float 07_float.cpp

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

void example() {
    float a = 1e15, b = -1e15, c = 1.5;

    float left  = (a + b) + c; // (1e15 - 1e15) + 1.5 = 0 + 1.5 = 1.5
    float right = a + (b + c); // 1e15 + (-1e15 + 1.5), precision loss

    std::cout << "\n(a+b)+c = " << left  << "\n"; // 1.5
    std::cout << "a+(b+c) = " << right << "\n"; // may differ
    std::cout << "Equal:   " << (left == right) << "\n";
    // With -ffast-math the compiler may reorder and get a different result
}

int main() {
    example();
    return 0;
}
