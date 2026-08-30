// Example from: playfull_programming_chapter_1-5_computational_basis_ru.md
// Source line: 360
// Build (MSVC): cl /std:c++20 /EHsc /nologo 09_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 09_Fixed 09_Fixed.cpp

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

struct Fixed {                        // тот самый тип из начала главы
    int32_t raw;
};

// деление на инструкции, которую 8086 принёс ещё в 1978 году
Fixed divide(Fixed a, Fixed b) {
    return Fixed{(int32_t)(((int64_t)a.raw << 16) / b.raw)};
}

int main() { return 0; }
