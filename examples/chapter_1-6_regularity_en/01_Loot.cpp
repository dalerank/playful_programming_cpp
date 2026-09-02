// Example from: playfull_programming_chapter_1-6_regularity_en.md
// Source line: 29
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_Loot.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_Loot 01_Loot.cpp

#include "../common.h"

/*
   Enemy, an irregular type. Each enemy has its own loot; it lives
   on the heap, and Enemy holds a raw pointer to it.
   Copying is shallow, and == compares the loot address: a copy is "equal"
   to the original but not independent of it; they share one loot.
   Meanwhile two enemies with the same pocket contents are unequal.
*/
struct Loot { int gold; int ammo; };

class Enemy {
    Loot* loot_;
    int   id_;
public:
    // There is no default constructor, and std::default_initializable will not accept such a type.
    // Fields are initialized strictly in declaration order. The compiler
    // can warn about a violation (C5038), but that warning is off by
    // default and stays silent even on /W4, and with dependent fields
    // a swap would already read garbage.
    Enemy(int id, int gold)
        : loot_(new Loot{gold, 0}), id_(id) {}

    // There is no destructor either, and that is deliberate: with one, a shallow copy
    // would hand the same Loot to the OS twice.

    // The copy constructor copies the pointer, not the loot.
    // After that copy.loot_ == orig.loot_, and looting one corpse
    // empties the pockets of the other. That is a regularity violation:
    // the copy depends on the original.
    Enemy(const Enemy& o)
        : loot_(o.loot_), id_(o.id_) {}       // <- deliberate bug

    // operator== compares addresses, so two enemies with the same
    // gold and ammo count as unequal. This is equality by
    // pointer identity, not by value of the contents.
    bool operator==(const Enemy& o) const {
        return loot_ == o.loot_;              // <- deliberate bug
    }
};

int main() { return 0; }
