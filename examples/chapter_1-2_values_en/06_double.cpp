// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 250
// Build (MSVC): cl /std:c++20 /EHsc /nologo 06_double.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 06_double 06_double.cpp

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

void example() {
    double pos_zero =  0.0;
    double neg_zero = -0.0;

    std::cout<<"\n+0.0 == -0.0:"<<(pos_zero == neg_zero)<<"\n";
    // 1, equal per ==

    std::cout<<"signbit(+0.0):"<<std::signbit(pos_zero)<<"\n";
    // 0

    std::cout<<"signbit(-0.0):"<<std::signbit(neg_zero)<<"\n";
    // 1, different result!
    // signbit breaks regularity:
    // replaced equal with equal, but the result changed
}

int main() {
    example();
    return 0;
}
