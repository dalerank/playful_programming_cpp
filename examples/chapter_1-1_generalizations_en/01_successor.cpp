// Example from: playfull_programming_chapter_1-1_generalizations_en.md
// Source line: 77
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_successor.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_successor 01_successor.cpp

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

// Kind: natural number
using Natural = unsigned int;

// Successor function: n → n + 1
// Rule that maps each Natural to the next Natural
Natural successor(Natural n) {
    return n + 1;
}

int main() { return 0; }
