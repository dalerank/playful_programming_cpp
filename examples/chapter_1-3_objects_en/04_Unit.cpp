// Example from: playfull_programming_chapter_1-3_objects_en.md
// Source line: 298
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_Unit.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_Unit 04_Unit.cpp

#include "../common.h"

struct Unit { int id; int hp; };

void realloc_demo() {
    std::vector<Unit> units;
    units.reserve(2);
    units.push_back({1, 100});
    units.push_back({2, 80});

    // Remember the "target" by address, but as a number immediately,
    // so we do not keep a pointer that is about to become invalid.
    auto addr = reinterpret_cast<std::uintptr_t>(units.data());

    units.push_back({3, 60});  // reserve ran out, buffer moved

    assert(units.capacity() > 2);

    // The old buffer must stay alive while elements are moved out of it,
    // so the new address differs from the old one not by accident.
    assert(reinterpret_cast<std::uintptr_t>(units.data()) != addr);

    // addr is now just a number: objects live at a different address,
    // and dereferencing the old pointer would be undefined behavior
}

int main() { return 0; }
