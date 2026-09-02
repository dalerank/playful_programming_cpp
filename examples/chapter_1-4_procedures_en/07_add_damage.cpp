// Example from: playfull_programming_chapter_1-4_procedures_en.md
// Source line: 299
// Build (MSVC): cl /std:c++20 /EHsc /nologo 07_add_damage.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 07_add_damage 07_add_damage.cpp

#include "../common.h"

// per-frame damage counter: read first, then write
void add_damage(int *stat, int amount) {
    *stat += amount;
}

// hitpoints: old value is read, new value is written, order matters
void clamp_hp(int *hp, int max_hp) {
    if (*hp > max_hp)
        *hp = max_hp;
}

int main() { return 0; }
