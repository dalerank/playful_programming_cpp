// Example from: playfull_programming_chapter_1-2_values_ru.md
// Source line: 179
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_Quat.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_Quat 04_Quat.cpp

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

// Один поворот, два представления: q и -q
struct Quat { float x, y, z, w; };

static float dot(const Quat& a, const Quat& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Побитово разные, по смыслу один и тот же поворот
bool same_rotation(const Quat& a, const Quat& b, float eps = 1e-5f) {
    return std::abs(dot(a, b)) > 1.f - eps; // модуль, потому что знак не важен
}

int main() { return 0; }
