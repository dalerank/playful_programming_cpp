// Example from: playfull_programming_chapter_1-1_generalizations_en.md
// Source line: 108
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Pair.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Pair 03_Pair.cpp

#include "../common.h"

// Genus: template as a rule over kinds.
// Pair<T, U> is not yet a kind by itself; it is a rule
// that maps two kinds to a third
template<typename T, typename U>
struct Pair {
    T first;
    U second;
};

// Rule at the "fits in backpack" genus level: weight is computed
// for any kind that has weight, and you do not rewrite the function
// for every new item
template<typename Item>
float total_weight(const std::vector<Item>& items) {
    float sum = 0;
    for (const Item& item : items) {
        sum += item.weight;
    }
    return sum;
}

int main() { return 0; }
