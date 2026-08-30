// Example from: playfull_programming_chapter_1-3_objects_en.md
// Source line: 206
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Projectile.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Projectile 03_Projectile.cpp

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

struct Projectile { int id; float ttl; };

// One memory slot; at different times different objects live in it.
// A detailed talk about placing objects in ready-made memory comes later;
// right now what matters is only that the address matches and the objects differ.
alignas(Projectile) unsigned char slot[sizeof(Projectile)];

void pool_demo() {
    Projectile* first = new (slot) Projectile{1, 0.5f};
    first->~Projectile();                   // first object's lifetime ended

    Projectile* second = new (slot) Projectile{2, 1.0f};
    assert((void*)first == (void*)second);  // the same place
    second->~Projectile();                  // but the object here was already different
}

int main() { return 0; }
