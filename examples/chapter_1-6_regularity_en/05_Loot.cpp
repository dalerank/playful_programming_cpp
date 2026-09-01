// Example from: playfull_programming_chapter_1-6_regularity_en.md
// Source line: 279
// Build (MSVC): cl /std:c++20 /EHsc /nologo 05_Loot.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 05_Loot 05_Loot.cpp

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

struct Loot {
    int gold;
    int ammo;
    auto operator<=>(const Loot&) const = default;
};

struct Enemy {
    int  id;
    int  dmg;
    Loot loot;                     // loot inside, not behind a pointer

    // Copying, assignment, and default construction the compiler
    // will write itself and write correctly, because every field already
    // behaves like a value. Comparison too, and also in one line.
    auto operator<=>(const Enemy&) const = default;
};

// That same check that failed on the previous Enemy.
static_assert(regular<Enemy>);
static_assert(totally_ordered<Enemy>);

// Equality is now about contents, not addresses.
static_assert(Enemy{7, 40, {100, 30}} == Enemy{7, 40, {100, 30}});
static_assert(Enemy{7, 40, {100, 30}} != Enemy{7, 40, {100, 29}});

// And that same target sorter from the opening of the chapter. We sort values,
// not pointers, so there is nowhere left to pad the comparison with addresses.
long long resolve_targets(vector<Enemy>& v) {
    sort(v.begin(), v.end(), [](const Enemy& a, const Enemy& b) {
        if (a.dmg != b.dmg) return a.dmg > b.dmg;
        return a.id < b.id;        // <- value, not address
    });
    long long acc = 0;
    for (const Enemy& e : v) acc = acc * 31 + e.id;   // the same fold
    return acc;
}

int main() { return 0; }
