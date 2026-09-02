// Example from: playfull_programming_chapter_1-4_procedures_ru.md
// Source line: 273
// Build (MSVC): cl /std:c++20 /EHsc /nologo 06_reset_damage.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 06_reset_damage 06_reset_damage.cpp

#include "../common.h"

#include <cstddef>
#include <cstdio>

// чистый выход: прежнее содержимое буфера не читается
void reset_damage(int *damage, std::size_t n) {
    for (std::size_t i = 0; i < n; i++)
        damage[i] = 0;           // только запись, чтения нет
}

void pure_out() {
    int damage[8];
    damage[0] = 42;   // мёртвая запись
    reset_damage(damage, 8);
    std::printf("%d\n", damage[0]);
}

int main() {
    pure_out();
}
