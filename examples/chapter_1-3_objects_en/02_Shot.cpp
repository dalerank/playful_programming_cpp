// Example from: playfull_programming_chapter_1-3_objects_en.md
// Source line: 168
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_Shot.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_Shot 02_Shot.cpp

#include "../common.h"

struct Shot {
    char team;    // 1 byte
    int  damage;  // 4 bytes, with a three-byte hole before them
};

static_assert(sizeof(Shot) == 8);
static_assert(!std::has_unique_object_representations_v<Shot>);  // the hole blocks it
static_assert(std::has_unique_object_representations_v<int>);    // no holes here
static_assert(!std::has_unique_object_representations_v<float>); // no holes, yet the answer is "no"

void padding_demo() {
    Shot a;
    Shot b;
    std::memset(&a, 0x00, sizeof a);  // whole object zeros, hole included
    std::memset(&b, 0xFF, sizeof b);  // and this one ones

    a.team = 'A';  a.damage = 30;
    b.team = 'A';  b.damage = 30;

    assert(a.team == b.team && a.damage == b.damage);  // fields match

    // memcmp also compares the hole, whose contents the standard
    // does not promise at all, so here it will almost certainly say "not equal"
    std::cout << "memcmp: " << std::memcmp(&a, &b, sizeof a) << "\n";
}

int main() { return 0; }
