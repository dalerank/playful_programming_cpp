// Example from: playfull_programming_chapter_1-1_generalizations_en.md
// Source line: 45
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Medkit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Medkit 00_Medkit.cpp

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

// "Health" as an abstract entity: a value kind with no birth frame
using Health = int;

// A concrete medkit: appeared on a frame and vanishes after pickup
struct Medkit {
    Health heal;
    unsigned spawn_frame;
};

void pickup_demo() {
    Health hp = 80;
    Medkit kit{20, 148392};  // stash in this match
    hp += kit.heal;          // +20 hit points
    // kit removed from the scene, only the trace in hp remains
}

int main() { return 0; }
