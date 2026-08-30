// Example from: playfull_programming_chapter_1-4_procedures_ru.md
// Source line: 299
// Build (MSVC): cl /std:c++20 /EHsc /nologo 07_add_damage.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 07_add_damage 07_add_damage.cpp

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

// счётчик урона за кадр: сначала чтение, потом запись
void add_damage(int *stat, int amount) {
    *stat += amount;
}

// хитпоинты: старое значение читается, новое пишется, порядок критичен
void clamp_hp(int *hp, int max_hp) {
    if (*hp > max_hp)
        *hp = max_hp;
}

int main() { return 0; }
