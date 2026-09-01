// Example from: playfull_programming_chapter_1-6_regularity_ru.md
// Source line: 5
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Enemy.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Enemy 00_Enemy.cpp

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

struct Enemy { int id; int dmg; };

// Отсортировать цели по урону, а при равном уроне... а при равном уроне
// кто-то однажды дописал «ну хоть как-нибудь, лишь бы стабильно».
long long resolve_targets(vector<Enemy*>& v) {
    sort(v.begin(), v.end(), [](const Enemy* a, const Enemy* b) {
        if (a->dmg != b->dmg) return a->dmg > b->dmg;
        return a < b;                    // <- а вот тут сравниваются адреса
    });
    long long acc = 0;
    for (const Enemy* e : v) acc = acc * 31 + e->id;   // порядок важен
    return acc;
}

int main() { return 0; }
