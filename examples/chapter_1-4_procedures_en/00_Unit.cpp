// Example from: playfull_programming_chapter_1-4_procedures_en.md
// Source line: 5
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Unit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Unit 00_Unit.cpp

#include "../common.h"

struct Unit { const char *name; int level; };

// do not do this, we will unpack why below
const char *unit_label(const Unit &u) {
    char buf[64];
    std::snprintf(buf, sizeof buf, "%s [%d]", u.name, u.level);
    return buf;   // the address lives shorter than the pointer to it
}

int main() { return 0; }
