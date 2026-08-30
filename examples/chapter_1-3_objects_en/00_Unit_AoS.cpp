// Example from: playfull_programming_chapter_1-3_objects_en.md
// Source line: 21
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Unit_AoS.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Unit_AoS 00_Unit_AoS.cpp

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

// array of structures: one unit's fields sit next to each other
struct Unit_AoS {
    float x, y, z;  // map position
    int hp;         // health
    int ai_state;   // AI state
};

void example_aos() {
    std::vector<Unit_AoS> units = {
        {10.0f, 0.0f, 5.0f, 100, 0},
        {12.0f, 0.0f, 7.0f,  80, 1},
        {14.0f, 0.0f, 9.0f,  60, 2}
    };

    // natural access, the whole unit is at hand
    std::cout << "AoS: " << units[1].x << ", " << units[1].z
              << ", hp " << units[1].hp << "\n";
}

int main() { return 0; }
