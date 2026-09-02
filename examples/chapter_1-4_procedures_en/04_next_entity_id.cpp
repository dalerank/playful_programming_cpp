// Example from: playfull_programming_chapter_1-4_procedures_en.md
// Source line: 232
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_next_entity_id.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_next_entity_id 04_next_entity_id.cpp

#include "../common.h"

int next_entity_id() {
    static int last = 0;
    return ++last;
}

int main() { return 0; }
