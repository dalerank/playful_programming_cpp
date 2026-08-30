// Example from: playfull_programming_chapter_1-1_generalizations_en.md
// Source line: 94
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_Medkit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_Medkit 02_Medkit.cpp

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

// Kinds: each item has its own set of attributes
struct Medkit  { float weight; int heal; };
struct Ammo    { float weight; int caliber; int count; };
struct Grenade { float weight; float radius; float fuse; };

// Genera cut this set differently and match no single kind:
//   fits in backpack       Medkit, Ammo, Grenade
//   consumed in hand       Medkit, Grenade
//   explodes               Grenade

int main() { return 0; }
