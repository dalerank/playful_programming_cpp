// Example from: playfull_programming_chapter_1-3_objects_en.md
// Source line: 298
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_Unit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_Unit 04_Unit.cpp

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

struct Unit { int id; int hp; };

void realloc_demo() {
    std::vector<Unit> units;
    units.reserve(2);
    units.push_back({1, 100});
    units.push_back({2, 80});

    // Remember the "target" by address, but as a number immediately,
    // so we do not keep a pointer that is about to become invalid.
    auto addr = reinterpret_cast<std::uintptr_t>(units.data());

    units.push_back({3, 60});  // reserve ran out, buffer moved

    assert(units.capacity() > 2);

    // The old buffer must stay alive while elements are moved out of it,
    // so the new address differs from the old one not by accident.
    assert(reinterpret_cast<std::uintptr_t>(units.data()) != addr);

    // addr is now just a number: objects live at a different address,
    // and dereferencing the old pointer would be undefined behavior
}

int main() { return 0; }
