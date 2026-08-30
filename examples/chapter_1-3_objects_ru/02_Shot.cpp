// Example from: playfull_programming_chapter_1-3_objects_ru.md
// Source line: 166
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_Shot.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_Shot 02_Shot.cpp

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

struct Shot {
    char team;    // 1 байт
    int  damage;  // 4 байта, а перед ними три байта дыры
};

static_assert(sizeof(Shot) == 8);
static_assert(!std::has_unique_object_representations_v<Shot>);  // мешает дыра
static_assert(std::has_unique_object_representations_v<int>);    // а тут дыр нет
static_assert(!std::has_unique_object_representations_v<float>); // дыр нет, а ответ «нет»

void padding_demo() {
    Shot a;
    Shot b;
    std::memset(&a, 0x00, sizeof a);  // весь объект нулями, вместе с дырой
    std::memset(&b, 0xFF, sizeof b);  // а этот единицами

    a.team = 'A';  a.damage = 30;
    b.team = 'A';  b.damage = 30;

    assert(a.team == b.team && a.damage == b.damage);  // поля совпали

    // memcmp сравнивает и дыру, содержимое которой стандарт вообще
    // никак не обещает, так что тут он почти наверняка скажет "не равны"
    std::cout << "memcmp: " << std::memcmp(&a, &b, sizeof a) << "\n";
}

int main() { return 0; }
