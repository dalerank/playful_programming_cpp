// Example from: playfull_programming_chapter_1-6_regularity_en.md
// Source line: 234
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_has_eq.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_has_eq 04_has_eq.cpp

#include "../common.h"

// The concept answers "is the syntax in place?",
// not "is the meaning correct?".
template <class T>
concept has_eq = requires(const T& a, const T& b) {
    { a == b } -> convertible_to<bool>;
};

struct Fixed {                               // hero of the chapter
    int32_t raw;
    auto operator<=>(const Fixed&) const = default;
};

struct Loot { int gold; int ammo; };

class Enemy {                                // and the villain
    Loot* loot_;
    int   id_;
public:
    Enemy(int id, int gold) : loot_(new Loot{gold, 0}), id_(id) {}
    bool operator==(const Enemy& o) const { return loot_ == o.loot_; }
};

static_assert(regular<Fixed>);               // letter and meaning
static_assert(regular<string>);              // letter and meaning
static_assert(regular<string_view>);         // letter yes, meaning no
static_assert(regular<optional<int>>);       // if T itself is regular

static_assert(!regular<Enemy>);              // no default constructor
static_assert(has_eq<Enemy>);                // comparison exists, but it lies

static_assert(!regular<span<int>>);          // no comparison at all
static_assert(!has_eq<span<int>>);           // a == b will not compile
static_assert(copyable<span<int>>);          // it does copy

static_assert(!regular<unique_ptr<int>>);    // not copyable
static_assert(has_eq<unique_ptr<int>>);      // but it does compare

int main() { return 0; }
