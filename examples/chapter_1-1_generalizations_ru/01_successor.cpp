// Example from: playfull_programming_chapter_1-1_generalizations_ru.md
// Source line: 77
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_successor.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_successor 01_successor.cpp

#include "../common.h"

// Вид: натуральное число
using Natural = unsigned int;

// Функция следования: n → n + 1
// Правило, которое каждому Natural сопоставляет следующий Natural
Natural successor(Natural n) {
    return n + 1;
}

int main() { return 0; }
