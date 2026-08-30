// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 65
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_enum.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_enum 02_enum.cpp

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

enum class Suit { Clubs, Diamonds, Hearts, Spades };

// the kind is finite, the type covers it entirely, so default is not needed,
// and -Wswitch will remind you if a fifth suit appears in the enum
const char* to_string(Suit s) {
    switch (s) {
        case Suit::Clubs:    return "clubs";
        case Suit::Diamonds: return "diamonds";
        case Suit::Hearts:   return "hearts";
        case Suit::Spades:   return "spades";
    }
    return "?"; // unreachable while s holds a well-formed Suit
}

int main() { return 0; }
