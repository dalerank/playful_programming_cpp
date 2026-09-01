// Example from: playfull_programming_chapter_1-6_regularity_ru.md
// Source line: 183
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Fixed 03_Fixed.cpp

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

/*
   Структурный порядок вместо смыслового
   Когда истинный порядок «по смыслу» недоступен или дорог,
   лексикографический / по полям это практичная альтернатива.
   std::sort, std::map, std::lower_bound работают с любым
   строгим слабым порядком, не требуя «правильного» смысла.
*/

struct Fixed {
    int32_t raw;

    // Структурный порядок по единственному полю. Он же оказывается
    // и смысловым, потому что raw растёт вместе со значением, и это
    // тот редкий случай, когда одно совпало с другим бесплатно.
    //
    // Отдельный operator== писать не нужно: дефолтный <=> объявляет
    // дефолтный == сам, из == выводится !=, а из <=> выводятся
    // <, >, <= и >=. Шесть операторов из одной строки.
    auto operator<=>(const Fixed&) const = default;
};

struct ReplayHeader {
    std::string map;
    int         tick;
    std::string winner;

    // А тут порядок чисто структурный: map → tick → winner.
    // Смыслового порядка у записей матча нет, никто не скажет,
    // какой реплей «больше», но для std::set и std::map хватает.
    auto operator<=>(const ReplayHeader&) const = default;
};

// Проверяем, что шесть операторов действительно появились.
// В формате 16.16 единица это 65536, половина 32768, полтора 98304.
static_assert(Fixed{65536} == Fixed{65536});
static_assert(Fixed{65536} != Fixed{32768});
static_assert(Fixed{32768} <  Fixed{65536});
static_assert(Fixed{98304} >= Fixed{65536});

int main() { return 0; }
