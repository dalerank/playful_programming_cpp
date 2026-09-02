// Example from: playfull_programming_chapter_1-3_objects_ru.md
// Source line: 300
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_Unit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_Unit 04_Unit.cpp

#include "../common.h"

struct Unit { int id; int hp; };

void realloc_demo() {
    std::vector<Unit> units;
    units.reserve(2);
    units.push_back({1, 100});
    units.push_back({2, 80});

    // Запоминаем «цель» адресом, но сразу числом, чтобы не держать
    // указатель, который вот-вот станет невалидным.
    auto addr = reinterpret_cast<std::uintptr_t>(units.data());

    units.push_back({3, 60});  // резерв кончился, буфер переехал

    assert(units.capacity() > 2);

    // Старый буфер обязан быть жив, пока из него переносят элементы,
    // поэтому новый адрес отличается от старого не случайно.
    assert(reinterpret_cast<std::uintptr_t>(units.data()) != addr);

    // addr теперь просто число: объекты живут по другому адресу,
    // а разыменовать старый указатель было бы неопределённым поведением
}

int main() { return 0; }
