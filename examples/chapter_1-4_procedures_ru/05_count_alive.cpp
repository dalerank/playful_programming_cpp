// Example from: playfull_programming_chapter_1-4_procedures_ru.md
// Source line: 247
// Build (MSVC): cl /std:c++20 /EHsc /nologo 05_count_alive.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 05_count_alive 05_count_alive.cpp

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

#include <cstddef>
#include <cstdio>

// чистый вход: массив хитпоинтов count_alive только читает
std::size_t count_alive(const int *hp, std::size_t n) {
    std::size_t alive = 0;
    for (std::size_t i = 0; i < n; i++)
        if (hp[i] > 0) alive++;
    return alive;
}

void pure_in() {
    int hp[] = {100, 0, 35, 0, 12};
    std::printf("%zu %zu\n", count_alive(hp, 5), count_alive(hp, 5));
}

int main() {
    pure_in();
}
