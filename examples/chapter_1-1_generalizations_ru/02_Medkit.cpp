// Example from: playfull_programming_chapter_1-1_generalizations_ru.md
// Source line: 94
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_Medkit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_Medkit 02_Medkit.cpp

#include "../common.h"

// Виды: у каждого предмета свой набор атрибутов
struct Medkit  { float weight; int heal; };
struct Ammo    { float weight; int caliber; int count; };
struct Grenade { float weight; float radius; float fuse; };

// Роды режут это множество по-разному и не совпадают ни с одним видом:
//   влезает в рюкзак       Medkit, Ammo, Grenade
//   расходуется в руках    Medkit, Grenade
//   взрывается             Grenade

int main() { return 0; }
