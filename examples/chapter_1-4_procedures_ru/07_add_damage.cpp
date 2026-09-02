// Example from: playfull_programming_chapter_1-4_procedures_ru.md
// Source line: 299
// Build (MSVC): cl /std:c++20 /EHsc /nologo 07_add_damage.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 07_add_damage 07_add_damage.cpp

#include "../common.h"

// счётчик урона за кадр: сначала чтение, потом запись
void add_damage(int *stat, int amount) {
    *stat += amount;
}

// хитпоинты: старое значение читается, новое пишется, порядок критичен
void clamp_hp(int *hp, int max_hp) {
    if (*hp > max_hp)
        *hp = max_hp;
}

int main() { return 0; }
