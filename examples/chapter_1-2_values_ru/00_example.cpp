// Example from: playfull_programming_chapter_1-2_values_ru.md
// Source line: 21
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_example.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_example 00_example.cpp

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
    // Каждый из этих битовых наборов осмыслен как int
        uint32_t bits1 = 0x00000001u;
        uint32_t bits2 = 0xFFFFFFFFu;

        int32_t a, b;
        std::memcpy(&a, &bits1, 4); // 1
        std::memcpy(&b, &bits2, 4); // -1  (дополнительный код)
}

int main() {
    example();
    return 0;
}
