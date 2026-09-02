// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 65
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_enum.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_enum 02_enum.cpp

#include "../common.h"

enum class Suit { Clubs, Diamonds, Hearts, Spades };

// the kind is finite, the type covers it entirely, so default is not needed,
// and -Wswitch will remind you if a fifth suit appears in the enum
const char* to_string(Suit s) {
    switch (s) {
        case Suit::Clubs:    return "clubs";
        case Suit::Diamonds: return "diamonds";
        case Suit::Hearts:   return "hearts";
        case Suit::Spades:   return "spades";
    }
    return "?"; // unreachable while s holds a well-formed Suit
}

int main() { return 0; }
