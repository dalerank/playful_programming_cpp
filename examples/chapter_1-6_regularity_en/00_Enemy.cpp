// Example from: playfull_programming_chapter_1-6_regularity_en.md
// Source line: 5
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Enemy.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Enemy 00_Enemy.cpp

#include "../common.h"

struct Enemy { int id; int dmg; };

// Sort targets by damage, and on equal damage... on equal damage
// someone once wrote "well somehow, as long as it is stable".
long long resolve_targets(vector<Enemy*>& v) {
    sort(v.begin(), v.end(), [](const Enemy* a, const Enemy* b) {
        if (a->dmg != b->dmg) return a->dmg > b->dmg;
        return a < b;                    // <- and here addresses are compared
    });
    long long acc = 0;
    for (const Enemy* e : v) acc = acc * 31 + e->id;   // order matters
    return acc;
}

int main() { return 0; }
