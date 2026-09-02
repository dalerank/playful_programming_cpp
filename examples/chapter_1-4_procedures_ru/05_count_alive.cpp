// Example from: playfull_programming_chapter_1-4_procedures_ru.md
// Source line: 247
// Build (MSVC): cl /std:c++20 /EHsc /nologo 05_count_alive.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 05_count_alive 05_count_alive.cpp

#include "../common.h"

#include <cstddef>
#include <cstdio>

// чистый вход: массив хитпоинтов count_alive только читает
std::size_t count_alive(const int *hp, std::size_t n) {
    std::size_t alive = 0;
    for (std::size_t i = 0; i < n; i++)
        if (hp[i] > 0) alive++;
    return alive;
}

void pure_in() {
    int hp[] = {100, 0, 35, 0, 12};
    std::printf("%zu %zu\n", count_alive(hp, 5), count_alive(hp, 5));
}

int main() {
    pure_in();
}
