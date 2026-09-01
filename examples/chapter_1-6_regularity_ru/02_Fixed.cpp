// Example from: playfull_programming_chapter_1-6_regularity_ru.md
// Source line: 100
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_Fixed 02_Fixed.cpp

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

// Тот самый Fixed из прошлой главы, уже с дописанным <=>,
// значение в формате 16.16, всё состояние это одно 32-битное число.
struct Fixed {
    int32_t raw;
    auto operator<=>(const Fixed&) const = default;
};

template <typename T>
T midpoint_naive(T a, T b) {
    // Регулярность гарантирует, что равные входы дают равный результат,
    // но саму арифметику из воздуха не достаёт.
    return (a + b) / T(2);   // <- и вот здесь притаилось переполнение
}

template <class T>
concept has_plus = requires(const T& a, const T& b) { { a + b } -> same_as<T>; };

// Fixed регулярен по всем требованиям концепта...
static_assert(regular<Fixed>);
// ...но сложения у него нет, поэтому midpoint_naive<Fixed> не соберётся.
static_assert(!has_plus<Fixed>);
static_assert(has_plus<int>);

int main() { return 0; }
