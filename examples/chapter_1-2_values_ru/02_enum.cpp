// Example from: playfull_programming_chapter_1-2_values_ru.md
// Source line: 65
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_enum.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_enum 02_enum.cpp

#include "../common.h"

enum class Suit { Clubs, Diamonds, Hearts, Spades };

// вид конечен, тип покрывает его целиком, поэтому default не нужен,
// а -Wswitch напомнит, если в enum появится пятая масть
const char* to_string(Suit s) {
    switch (s) {
        case Suit::Clubs:    return "clubs";
        case Suit::Diamonds: return "diamonds";
        case Suit::Hearts:   return "hearts";
        case Suit::Spades:   return "spades";
    }
    return "?"; // недостижимо, пока в s лежит корректно сформированный Suit
}

int main() { return 0; }
