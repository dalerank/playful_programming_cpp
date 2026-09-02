// Example from: playfull_programming_chapter_1-1_generalizations_ru.md
// Source line: 108
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Pair.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Pair 03_Pair.cpp

#include "../common.h"

// Род: шаблон как правило над видами.
// Pair<T, U> сам по себе ещё не вид, это правило,
// которое двум видам сопоставляет третий
template<typename T, typename U>
struct Pair {
    T first;
    U second;
};

// Правило на уровне рода «влезает в рюкзак»: вес считается
// для любого вида, у которого есть weight, и под каждый
// новый предмет функцию переписывать не придётся
template<typename Item>
float total_weight(const std::vector<Item>& items) {
    float sum = 0;
    for (const Item& item : items) {
        sum += item.weight;
    }
    return sum;
}

int main() { return 0; }
