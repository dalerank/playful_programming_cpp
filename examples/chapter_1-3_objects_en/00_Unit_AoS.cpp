// Example from: playfull_programming_chapter_1-3_objects_en.md
// Source line: 21
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Unit_AoS.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Unit_AoS 00_Unit_AoS.cpp

#include "../common.h"

// array of structures: one unit's fields sit next to each other
struct Unit_AoS {
    float x, y, z;  // map position
    int hp;         // health
    int ai_state;   // AI state
};

void example_aos() {
    std::vector<Unit_AoS> units = {
        {10.0f, 0.0f, 5.0f, 100, 0},
        {12.0f, 0.0f, 7.0f,  80, 1},
        {14.0f, 0.0f, 9.0f,  60, 2}
    };

    // natural access, the whole unit is at hand
    std::cout << "AoS: " << units[1].x << ", " << units[1].z
              << ", hp " << units[1].hp << "\n";
}

int main() { return 0; }
