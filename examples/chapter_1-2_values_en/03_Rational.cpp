// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 135
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Rational.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Rational 03_Rational.cpp

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

#include <cassert>

// Non-unique representation
struct Rational {
    int64_t num; // numerator
    int64_t den; // denominator, strictly positive

    // Invariant lives here, not in a comment next to the field,
    // or well-formedness stays on the programmer's word alone
    static Rational make(int64_t n, int64_t d) {
        assert(d != 0);
        return d > 0 ? Rational{n, d} : Rational{-n, -d};
    }

    // comparing representations, fast and not about meaning
    bool naive_eq(const Rational& o) const {
        return num == o.num && den == o.den;
    }
    // slow check with normalization
    bool normalized_eq(const Rational& o) const {
        auto g1 = std::gcd(num, den);
        auto g2 = std::gcd(o.num, o.den);
        return (num / g1) == (o.num / g2) && (den / g1) == (o.den / g2);
    }
    // slow check with cross multiplication
    // careful: num * o.den can overflow int64_t
    bool cross_eq(const Rational& o) const {
        return num * o.den == o.num * den;
    }
};

void demo_rational() {
    auto a = Rational::make(1, 2); // 1/2
    auto b = Rational::make(2, 4); // 2/4, same abstract entity

    std::cout<<"1/2 naive_eq 2/4:"<<a.naive_eq(b)<<"\n"; // 0, representations differ, entity is one
    std::cout<<"1/2 normalized_eq 2/4:"<<a.normalized_eq(b)<<"\n"; // 1
    std::cout<<"1/2 cross_eq 2/4:"<<a.cross_eq(b)<< "\n"; // 1
}

int main() { return 0; }
