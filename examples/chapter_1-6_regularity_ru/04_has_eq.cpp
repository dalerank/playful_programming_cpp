// Example from: playfull_programming_chapter_1-6_regularity_ru.md
// Source line: 234
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_has_eq.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_has_eq 04_has_eq.cpp

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

// Концепт отвечает на вопрос «синтаксис на месте?»,
// а вовсе не на вопрос «смысл правильный?».
template <class T>
concept has_eq = requires(const T& a, const T& b) {
    { a == b } -> convertible_to<bool>;
};

struct Fixed {                               // герой главы
    int32_t raw;
    auto operator<=>(const Fixed&) const = default;
};

struct Loot { int gold; int ammo; };

class Enemy {                                // и злодей
    Loot* loot_;
    int   id_;
public:
    Enemy(int id, int gold) : loot_(new Loot{gold, 0}), id_(id) {}
    bool operator==(const Enemy& o) const { return loot_ == o.loot_; }
};

static_assert(regular<Fixed>);               // и по букве, и по смыслу
static_assert(regular<string>);              // и по букве, и по смыслу
static_assert(regular<string_view>);         // по букве да, по смыслу нет
static_assert(regular<optional<int>>);       // если регулярен сам T

static_assert(!regular<Enemy>);              // конструктора по умолчанию нет
static_assert(has_eq<Enemy>);                // а сравнение есть, только лживое

static_assert(!regular<span<int>>);          // сравнения нет вовсе
static_assert(!has_eq<span<int>>);           // a == b не соберётся
static_assert(copyable<span<int>>);          // копируется

static_assert(!regular<unique_ptr<int>>);    // не копируется
static_assert(has_eq<unique_ptr<int>>);      // зато сравнивается

int main() { return 0; }
