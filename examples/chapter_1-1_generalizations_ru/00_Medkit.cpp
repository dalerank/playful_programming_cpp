// Example from: playfull_programming_chapter_1-1_generalizations_ru.md
// Source line: 45
// Build (MSVC): cl /std:c++20 /EHsc /nologo 00_Medkit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 00_Medkit 00_Medkit.cpp

#include "../common.h"

// «Здоровье» абстрактная сущность: вид значения, у него нет кадра рождения
using Health = int;

// Конкретная аптечка: появилась на кадре и пропадёт после подбора
struct Medkit {
    Health heal;
    unsigned spawn_frame;
};

void pickup_demo() {
    Health hp = 80;
    Medkit kit{20, 148392};  // склад в этом матче
    hp += kit.heal;          // +20 хитпоинтов
    // kit снят со сцены, остался только след в hp
}

int main() { return 0; }
