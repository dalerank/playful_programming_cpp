// Example from: playfull_programming_chapter_1-2_values_ru.md
// Source line: 33
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_double.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_double 01_double.cpp

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
    double y = std::nan("");   // «не число»

        std::cout << "y = "        << y                 << "\n";
        std::cout << "y + 1  = "   << y + 1             << "\n";
        std::cout << "y * 0  = "   << y * 0             << "\n";
        std::cout << "y == y = "   << (y == y)          << "\n";
        std::cout << "sqrt(-1) = " << std::sqrt(-1.0)   << "\n";
}

int main() {
    example();
    return 0;
}
