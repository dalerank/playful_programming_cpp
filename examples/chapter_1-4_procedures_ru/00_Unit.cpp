// Example from: playfull_programming_chapter_1-4_procedures_ru.md
// Source line: 5
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Unit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Unit 00_Unit.cpp

#include "../common.h"

struct Unit { const char *name; int level; };

// так делать нельзя, дальше разберёмся почему
const char *unit_label(const Unit &u) {
    char buf[64];
    std::snprintf(buf, sizeof buf, "%s [%d]", u.name, u.level);
    return buf;   // адрес живёт меньше, чем указатель на него
}

int main() { return 0; }
