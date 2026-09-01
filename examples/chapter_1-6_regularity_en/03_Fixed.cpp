// Example from: playfull_programming_chapter_1-6_regularity_en.md
// Source line: 183
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Fixed 03_Fixed.cpp

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

/*
   Structural order instead of semantic order
   When a true order "by meaning" is unavailable or expensive,
   lexicographic / by fields is a practical alternative.
   std::sort, std::map, std::lower_bound work with any
   strict weak ordering and do not require the "right" meaning.
*/

struct Fixed {
    int32_t raw;

    // Structural order on the single field. It also turns out
    // semantic, because raw grows with the value, and this is
    // the rare case where one coincides with the other for free.
    //
    // A separate operator== is not needed: defaulted <=> declares
    // defaulted == itself, != is derived from ==, and <, >, <=, >=
    // are derived from <=>. Six operators from one line.
    auto operator<=>(const Fixed&) const = default;
};

struct ReplayHeader {
    std::string map;
    int         tick;
    std::string winner;

    // Here the order is purely structural: map → tick → winner.
    // Match records have no semantic order; nobody can say
    // which replay is "greater", but for std::set and std::map it is enough.
    auto operator<=>(const ReplayHeader&) const = default;
};

// Check that the six operators really appeared.
// In 16.16 format, one is 65536, a half is 32768, one and a half is 98304.
static_assert(Fixed{65536} == Fixed{65536});
static_assert(Fixed{65536} != Fixed{32768});
static_assert(Fixed{32768} <  Fixed{65536});
static_assert(Fixed{98304} >= Fixed{65536});

int main() { return 0; }
