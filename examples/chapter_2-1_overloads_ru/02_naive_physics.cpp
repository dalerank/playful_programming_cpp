// Example from: playfull_programming_chapter_2-1_overloads_ru.md
// Source line: 61
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_naive_physics.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_naive_physics 02_naive_physics.cpp

#include "../common.h"

void naive_physics() {
    float acceleration = 9.8f;   // м/с²
    float velocity = 100.0f;     // м/с
    float mass = 50.0f;          // кг
    float dt = 2.0f;             // с

    // Компилятор позволяет писать бессмыслицу
    float nonsense1 = acceleration + mass;      // 9.8 + 50 = ???
    // складываем м/с² с кг, физически это абсурд

    float nonsense2 = velocity * mass;          // 100 * 50 = ???
    // получили кг·м/с, но это импульс, а не ускорение

    float nonsense3 = acceleration / velocity;  // 9.8 / 100 = ???
    // получили с⁻¹, частоту, не имеющую отношения к задаче

    float nonsense4 = velocity + dt;            // 100 + 2 = ???
    // а тут сложили скорость и время, и всё опять собралось

    // Собирается всё, а сигнала об ошибке смысла нет, на выходе
    // просто неверные числа, без ассертов и без ошибок компиляции
    printf("%.1f %.1f %.1f %.1f\n", nonsense1, nonsense2, nonsense3, nonsense4);
}

int main() { return 0; }
