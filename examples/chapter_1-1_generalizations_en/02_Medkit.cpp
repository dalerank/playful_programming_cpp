// Example from: playfull_programming_chapter_1-1_generalizations_en.md
// Source line: 94
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_Medkit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_Medkit 02_Medkit.cpp

#include "../common.h"

// Kinds: each item has its own set of attributes
struct Medkit  { float weight; int heal; };
struct Ammo    { float weight; int caliber; int count; };
struct Grenade { float weight; float radius; float fuse; };

// Genera cut this set differently and match no single kind:
//   fits in backpack       Medkit, Ammo, Grenade
//   consumed in hand       Medkit, Grenade
//   explodes               Grenade

int main() { return 0; }
