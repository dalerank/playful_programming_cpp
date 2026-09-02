// Example from: playfull_programming_chapter_1-1_generalizations_en.md
// Source line: 45
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Medkit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Medkit 00_Medkit.cpp

#include "../common.h"

// "Health" as an abstract entity: a value kind with no birth frame
using Health = int;

// A concrete medkit: appeared on a frame and vanishes after pickup
struct Medkit {
    Health heal;
    unsigned spawn_frame;
};

void pickup_demo() {
    Health hp = 80;
    Medkit kit{20, 148392};  // stash in this match
    hp += kit.heal;          // +20 hit points
    // kit removed from the scene, only the trace in hp remains
}

int main() { return 0; }
