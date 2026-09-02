// Example from: playfull_programming_chapter_1-6_regularity_en.md
// Source line: 279
// Build (MSVC): cl /std:c++20 /EHsc /nologo 05_Loot.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 05_Loot 05_Loot.cpp

#include "../common.h"

struct Loot {
    int gold;
    int ammo;
    auto operator<=>(const Loot&) const = default;
};

struct Enemy {
    int  id;
    int  dmg;
    Loot loot;                     // loot inside, not behind a pointer

    // Copying, assignment, and default construction the compiler
    // will write itself and write correctly, because every field already
    // behaves like a value. Comparison too, and also in one line.
    auto operator<=>(const Enemy&) const = default;
};

// That same check that failed on the previous Enemy.
static_assert(regular<Enemy>);
static_assert(totally_ordered<Enemy>);

// Equality is now about contents, not addresses.
static_assert(Enemy{7, 40, {100, 30}} == Enemy{7, 40, {100, 30}});
static_assert(Enemy{7, 40, {100, 30}} != Enemy{7, 40, {100, 29}});

// And that same target sorter from the opening of the chapter. We sort values,
// not pointers, so there is nowhere left to pad the comparison with addresses.
long long resolve_targets(vector<Enemy>& v) {
    sort(v.begin(), v.end(), [](const Enemy& a, const Enemy& b) {
        if (a.dmg != b.dmg) return a.dmg > b.dmg;
        return a.id < b.id;        // <- value, not address
    });
    long long acc = 0;
    for (const Enemy& e : v) acc = acc * 31 + e.id;   // the same fold
    return acc;
}

int main() { return 0; }
