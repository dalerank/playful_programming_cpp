// Example from: playfull_programming_chapter_1-4_procedures_en.md
// Source line: 196
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_armor_of.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_armor_of 03_armor_of.cpp

#include "../common.h"

int armor_of(int level) { return 3 * level + 2; }

int damage_after_armor(int dmg, int level) {
    return dmg - armor_of(level);   // there will be no call here
}

int main() { return 0; }
