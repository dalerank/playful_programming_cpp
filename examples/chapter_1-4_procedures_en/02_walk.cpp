// Example from: playfull_programming_chapter_1-4_procedures_en.md
// Source line: 127
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_walk.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_walk 02_walk.cpp

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

#include <cstdio>

// this program is supposed to crash, that is the point
int walk(int depth)
{
    char scratch[64];
    scratch[0] = (char)depth;
    if (depth % 16 == 0) {
        std::printf("%d\n", depth);
        std::fflush(stdout);
    }
    return walk(depth + 1) + scratch[0];
}

int main()
{
    return walk(1);
}
