// Example from: playfull_programming_chapter_1-6_regularity_en.md
// Source line: 100
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_Fixed.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_Fixed 02_Fixed.cpp

#include "../common.h"

// That same Fixed from the previous chapter, already with <=> added,
// a 16.16 value, the whole state one 32-bit number.
struct Fixed {
    int32_t raw;
    auto operator<=>(const Fixed&) const = default;
};

template <typename T>
T midpoint_naive(T a, T b) {
    // Regularity guarantees that equal inputs give an equal result,
    // but it does not conjure arithmetic out of thin air.
    return (a + b) / T(2);   // <- and here lurks overflow
}

template <class T>
concept has_plus = requires(const T& a, const T& b) { { a + b } -> same_as<T>; };

// Fixed is regular by every concept requirement...
static_assert(regular<Fixed>);
// ...but it has no addition, so midpoint_naive<Fixed> will not compile.
static_assert(!has_plus<Fixed>);
static_assert(has_plus<int>);

int main() { return 0; }
