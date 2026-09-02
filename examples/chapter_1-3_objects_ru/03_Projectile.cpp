// Example from: playfull_programming_chapter_1-3_objects_ru.md
// Source line: 204
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Projectile.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Projectile 03_Projectile.cpp

#include "../common.h"

struct Projectile { int id; float ttl; };

// Один слот памяти, в разное время в нём живут разные объекты.
// Подробный разговор про размещение объектов в готовой памяти будет позже,
// сейчас важно только то, что адрес совпадает, а объекты разные.
alignas(Projectile) unsigned char slot[sizeof(Projectile)];

void pool_demo() {
    Projectile* first = new (slot) Projectile{1, 0.5f};
    first->~Projectile();                   // время жизни первого закончилось

    Projectile* second = new (slot) Projectile{2, 1.0f};
    assert((void*)first == (void*)second);  // то же самое место
    second->~Projectile();                  // но объект тут был уже другой
}

int main() { return 0; }
