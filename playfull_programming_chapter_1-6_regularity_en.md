# Regularity

Imagine a bug lands on your desk with a game recording attached. The match itself is recorded, but the replay is unstable: a unit goes around a corner on the left, while QA writes in the report that very rarely it goes on the right, and then the game crashes on an assert about a wrong checksum. The input state is bit-for-bit identical, the generator seed is the same, the binary is the same, and only on certain machines the results come out wrong. You can hunt for the cause of such a bug for a very long time, and that delightful process feels like looking for a needle in a haystack, especially when the needle is not there and the haystack sits on a remote machine. Desync already showed up in the chapter on values; there the numbers themselves diverged, and here... here we still have to figure out what is wrong.

```cpp
struct Enemy { int id; int dmg; };

// Sort targets by damage, and on equal damage... on equal damage
// someone once wrote "well somehow, as long as it is stable".
long long resolve_targets(vector<Enemy*>& v) {
    sort(v.begin(), v.end(), [](const Enemy* a, const Enemy* b) {
        if (a->dmg != b->dmg) return a->dmg > b->dmg;
        return a < b;                    // <- and here addresses are compared
    });
    long long acc = 0;
    for (const Enemy* e : v) acc = acc * 31 + e->id;   // order matters
    return acc;
}
```

While investigating this bug we land on the target-sorting function. It does sort stably, but only within a single run, and if you take eight enemies with equal damage and "assemble" them on two "machines" from the same logical state, yet allocate memory in a different order, you get the same data, the same code, and different answers, something like `116267632558` and `165191219038`. The standard, by the way, promises nothing about `<` on pointers to unrelated objects; only `std::less` gives a defined answer here, so this sort is already questionable before any desync enters the picture.

Talk about equality, copying, and mathematical correctness is easy to dismiss as something only theory fans need, yet it is all about bugs like this. We will come back to this example later; for now let us talk about regularity and why it matters to a developer.

## What a regular type is

It is more convenient to start the talk about regularity from the opposite example, a type that is not regular. We need not look far: take the same `Enemy` from the opening of the chapter and look inside.

```cpp
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
```

A regular type in the modern sense behaves like value types such as `int` (with caveats for `double` and NaN/±0; see the chapter on values), providing correct equality, copying, assignment, and a default constructor, with copies independent of the original. The key axiom is that if `a == b`, then after the same regular operation the results are equal again (`f(a) == f(b)`), and that predictability lets you replace equals with equals without losing the meaning of the program (congruence).

Now skim `Enemy` again: notice that congruence actually holds for it? Two enemies become equal under such an `==` only when one is made a copy of the other, and the copy brings along the same `Loot` and the same `id_`; you cannot touch them from outside and the class exposes no fields, so `f(a) == f(b)` falls out by itself. It hangs by a thread, and the moment you add a constructor `Enemy(int id, Loot* shared)`, enemies with different ids become equal and congruence is gone.

What is broken in this class is something else: copies are dependent through a pointer, and after looting one copy you zero out the pockets of all the others. A good counterexample is `FixedOrdered` from the previous chapter, whose entire state is one 32-bit field `raw`; it turns out regular immediately with no extra effort, and the pair `Enemy` versus `Fixed` is a convenient place to see how one differs from the other.

## Where the word itself came from

It is strange, generally speaking, that such an obvious thing needed a separate term, because while you write your own code you are unlikely to stop and think about making your classes regular. You wrote `Enemy`, wrote `==` for it, and know perfectly well what the operator does inside. But all of that is needed by people who write code that has never seen your types and still has to work with them.

The term was born not in `C++` but in Ada, which already had generics since 1983, so you could write logic that works with any element type, and as soon as you wrote that, you needed some way to explain what you require of a foreign type. Not merely "it must work with this generic", but a list: it copies, it compares, the copy is independent, equals behave the same. That list of requirements is regularity.

> **1981.** Deepak Kapur, David Musser, and Alexander Stepanov, `Operators and Algebraic Structures`. A type is described not by fields but by operations and the laws those operations obey. Code is still far away; this is pure algebra for now.

> **1989.** Musser and Stepanov, `The Ada Generic Library`. Here the term "regular type" already sits in the text as a working notion, and it sticks in a language with generics because there the requirements on a foreign type have to be written out explicitly.

> **1994.** The committee votes to include the STL in the `C++` standard. Regularity turns from a paper into a requirement on your classes, only nowhere written down; `std::sort` expects it but has no way to say so.

> **2000.** James Dehnert and Stepanov, `Fundamentals of Generic Programming`. Regularity is taken apart bone by bone, including the part the language still lacks.

> **2009.** Stepanov and Paul McJones, `Elements of Programming`. The book from which half the standard library is derived from these laws.

> **2020.** The concept `std::regular` lands in the language. Between the term sticking and the compiler learning to check it, thirty-one years passed.

Thirty-one years of evolution are hard to pack into one paragraph, and there is no need to; it is enough to remember what actually arrived in the language in 2020. What arrived is a check that the type has all the needed operations, while independence of copies and equality by value the compiler does not have to verify; that needs mathematical machinery the language does not have.

## A regular function

A function can be regular too; that discussion was in the chapter on values, where `NaN` and the sign of zero broke the regularity of familiar operations on `double`. The assumption that equal arguments yield equal results is what the whole algorithmic layer of the standard library for value types rests on, from sorting and search to sequence transforms, hence a single `std::sort` for `int`, `std::string`, and your types; and what that assumption does *not* give you is visible in the example below:

```cpp
// That same Fixed from the previous chapter, already with <=> added,
// a 16.16 value, the whole state one 32-bit number.
struct Fixed {
    int32_t raw;
    auto operator<=>(const Fixed&) const = default;
};

template <typename T>
T midpoint_naive(T a, T b) {
    // Regularity guarantees that equal inputs give an equal result,
    // but it does not conjure arithmetic out of thin air.
    return (a + b) / T(2);   // <- and here lurks overflow
}

template <class T>
concept has_plus = requires(const T& a, const T& b) { { a + b } -> same_as<T>; };

// Fixed is regular by every concept requirement...
static_assert(regular<Fixed>);
// ...but it has no addition, so midpoint_naive<Fixed> will not compile.
static_assert(!has_plus<Fixed>);
static_assert(has_plus<int>);
```

I put the word "naive" in the name on purpose, and for `T = int` this function does not compute a midpoint at all. Take `a = 2000000000` and `b = 2000000001`, which overflow `int` when added, and instead of the expected `2000000000` you get `-147483647`. On the author's machine `/Od` and `/O2` give the same result, but you cannot rely on that, because signed overflow is undefined behavior, and another compiler with other flags is free to produce anything.

That bug lived for two decades in the binary search of almost every textbook until Joshua Bloch wrote about it in 2006, and because of it `C++20` got `std::midpoint`, which computes the middle so the intermediate result stays in range. As you can see, the regularity of `int` did not help here and could not have helped, because it promises that equal inputs give an equal output, not that the output will be correct.

You can also see what regularity does not give. `Fixed` passes `std::regular`; it has equality, independent copies, and a default constructor, yet `midpoint_naive<Fixed>` still will not compile, because nobody taught `Fixed` to add. Regularity is about the type behaving like a value; you bring the arithmetic to that value yourself. And conversely, inside `Fixed` sits the same `int32_t`, so once you add blunt addition you get the same overflow.

## What equality costs

When we work with built-in types like `int` or `bool`, we usually treat equality, copy, and assignment as constant time, one or two machine instructions. For `double` comparison is fast too, but it is no longer "just bits", because `(+0.0 == -0.0) == true` while `(NaN == NaN) == false`.

Working with complex types, we expect an equality check to take time proportional to the total volume of data. That expectation is reasonable, but linearity is not guaranteed from above or below, and the spread is substantial; take two megabyte-long strings and measure, remembering that the machine is one machine and your numbers will be your own.


| what we compare              | time of one `==` |
| ---------------------------- | ---------------- |
| strings of different length  | 1.1 ns           |
| differ in the first byte     | 4.8 ns           |
| differ in the last byte      | 39 µs            |
| fully equal                  | 39 µs            |


Between the first and last row of the table the difference is thirty-five thousand times, on the same `operator==`. From below, linearity is broken by early exit: comparison ends on the first character, and even earlier a length check fires, so strings of different size are rejected without reading any data at all. The top row of the table is almost entirely measurement overhead; an empty loop in the same harness gives 0.5 ns.

From above, linearity is propped up by the library, and for trivially comparable types in contiguous memory `std::equal` may collapse an element-wise walk into a single `memcmp` call, which then runs with vector instructions and processes a megabyte in forty microseconds. That is, the cost of comparison depends on the data more than on the type, and if your profiler shows pennies on synthetic input, on production data it may show something else entirely.

And sometimes the notion of equality itself gets expensive, and byte comparison has nothing to do with it. Take a multiset, an unordered collection of elements with possible duplicates, say a player's inventory: three potions, two keys, and a sword, stored in a `std::vector` in the order they were picked up. Two such inventories are equal when they hold the same things, and pickup order interests nobody, and to check that you either sort both and compare lexicographically, which is O(n log n), or for each element of the first check its presence in the second with multiplicity, which is O(n²). The data are still the same n items, but the price of comparison has grown, because "equal" here means "the same elements in any order".

## When equality is too expensive

In some cases equality by meaning can turn out computationally heavy enough that people abandon it and look for other ways to compare types. The textbook example here is checking graph isomorphism. Graph A and graph B look different: different vertex names, different layout on the diagram, but if you rename `0→A, 1→C, 2→B, 3→D`, every edge matches one to one, so the graphs are isomorphic; it is the same structure, just drawn differently.

```text
       Graph A                       Graph B

          0                             A
         / \                           / \
        /   \                         /   \
       1-----2                       B-----C
       |                                   |
       3                                   D

   edges  0-1  0-2  1-2  1-3     edges  A-B  A-C  B-C  C-D

               isomorphism:  0→A   1→C   2→B   3→D
```

The tempting correspondence "by position on the diagram", where `1→B` and `2→C`, does not work, because vertex `1` has three neighbors and `B` only two, and the dangling edge `1-3` has nowhere to go. The only handle you can grab by eye here is vertex degrees, and even that only weeds out obviously wrong options; it does not find the right one.

Checking isomorphism the blunt way means enumerating every way to rename the vertices of one graph into the vertices of the other, and that is `n!` variants; already at `n = 20` there are 2,432,902,008,176,640,000 of them, two and a half quintillion, so brute force is out immediately. Nobody has found a fast algorithm for this problem, but nobody has proved that none exists either. Furthest among the "fast solutions" went László Babai in 2015, and his algorithm is incomparably faster than brute force, though it still does not reach a truly fast one.

In practice things are not so grim, and existing solvers like `nauty` and `VF2` handle graphs with thousands of vertices instantly, because live graphs almost never turn out to be that worst case. But you still have no guarantee that you will not hit exactly that case.

In such situations a programmer is forced either to give up on full equality by meaning or to settle for representational equality and compare bits directly, that is, treat two values as equal if their bit images are identical. For composite objects that is often a recursive field comparison, but a raw `memcmp` over the whole structure is dangerous if it has "holes" (padding), and the same meaning with different garbage in the "holes" will give a false result for equal values. Conversely, `memcmp` should not be taken for a meaningful `operator==`, even when it happens to give correct answers.

## Structural order instead of semantic order

The same story with order: when a true order "by meaning" is unavailable or too expensive, people take a lexicographic one or order by the first differing fields, and that is usually enough to sort and search.

```cpp
/*
   Structural order instead of semantic order
   When a true order "by meaning" is unavailable or expensive,
   lexicographic / by fields is a practical alternative.
   std::sort, std::map, std::lower_bound work with any
   strict weak ordering and do not require the "right" meaning.
*/

struct Fixed {
    int32_t raw;

    // Structural order on the single field. It also turns out
    // semantic, because raw grows with the value, and this is
    // the rare case where one coincides with the other for free.
    //
    // A separate operator== is not needed: defaulted <=> declares
    // defaulted == itself, != is derived from ==, and <, >, <=, >=
    // are derived from <=>. Six operators from one line.
    auto operator<=>(const Fixed&) const = default;
};

struct ReplayHeader {
    std::string map;
    int         tick;
    std::string winner;

    // Here the order is purely structural: map → tick → winner.
    // Match records have no semantic order; nobody can say
    // which replay is "greater", but for std::set and std::map it is enough.
    auto operator<=>(const ReplayHeader&) const = default;
};

// Check that the six operators really appeared.
// In 16.16 format, one is 65536, a half is 32768, one and a half is 98304.
static_assert(Fixed{65536} == Fixed{65536});
static_assert(Fixed{65536} != Fixed{32768});
static_assert(Fixed{32768} <  Fixed{65536});
static_assert(Fixed{98304} >= Fixed{65536});
```

Not every object admits copying or equality at all, though, and if an object owns a unique resource such as an open file, a network connection, or a GPU buffer, then neither copying nor assignment makes sense for it, and we treat such types as "naturally irregular" and use them in special contexts where their semantics are stated explicitly.

## Letter and meaning

In modern `C++` some of the ideas of regularity got a language expression in concepts. Concepts fix a syntactic contract, that is, that a type can be copied and compared, and that contract is fairly close to the idea Stepanov described back in the eighties. Only between "can" and "does so meaningfully" there is a gulf.

It shows best if you run both heroes of the chapter through the concepts alongside the standard types. `Fixed` passes the check both in letter and in meaning, while `Enemy` fails `std::regular` because it has no default constructor, and that is the only thing the compiler noticed. The real trouble with `Enemy` is that its `operator==` lies, and no concept will tell you that. Add a default constructor to `Enemy`, and `std::regular<Enemy>` becomes true, even though equality still compares addresses. So we can perfectly well write a type that passes every conceivable check and still behaves incorrectly.

`std::optional` with a regular `T` sits well on value semantics; its equality and copying are about the value. On `std::string_view` it is convenient to show how letter differs from meaning. Formally `std::regular<std::string_view>` is true: the type copies, default-constructs, compares for equality; every requirement is met. But in meaning a copy duplicates only the pointer and the length, while the data underneath stay shared. With `std::span` the committee went further and took comparison away: the operators were in the drafts but were removed before `C++20` shipped, so `std::span` is not merely irregular; it is not even `equality_comparable`, and writing `a == b` for two `span`s will not compile.

```cpp
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
```

The optimizer knows nothing about `std::regular` and draws no conclusions from the concept. Replacing equals with equals and caching results are rules you reason by; the compiler has its own, independently proving absence of side effects, and it does not care about the declared concept. Still less does it derive algebraic properties like commutativity (`a + b` versus `b + a`), which `std::regular` does not promise and cannot promise. You can write `template<std::regular T> void process(T value);` today, and the compiler will check the concept's syntactic requirements: copyability, equality comparability, and the rest in that spirit, while equality by value and independence of copies remain on the author of the type.

## How to fix `Enemy`

Let us try to fix the main "villain" of the chapter so it works correctly with the algorithms of the standard library, and for that we stop hiding state behind a pointer.

```cpp
struct Loot {
    int gold;
    int ammo;
    auto operator<=>(const Loot&) const = default;
};

struct Enemy {
    int  id;
    int  dmg;
    Loot loot;                     // loot inside, not behind a pointer

    // Copying, assignment, and default construction the compiler
    // will write itself and write correctly, because every field already
    // behaves like a value. Comparison too, and also in one line.
    auto operator<=>(const Enemy&) const = default;
};

// That same check that failed on the previous Enemy.
static_assert(regular<Enemy>);
static_assert(totally_ordered<Enemy>);

// Equality is now about contents, not addresses.
static_assert(Enemy{7, 40, {100, 30}} == Enemy{7, 40, {100, 30}});
static_assert(Enemy{7, 40, {100, 30}} != Enemy{7, 40, {100, 29}});

// And that same target sorter from the opening of the chapter. We sort values,
// not pointers, so there is nowhere left to pad the comparison with addresses.
long long resolve_targets(vector<Enemy>& v) {
    sort(v.begin(), v.end(), [](const Enemy& a, const Enemy& b) {
        if (a.dmg != b.dmg) return a.dmg > b.dmg;
        return a.id < b.id;        // <- value, not address
    });
    long long acc = 0;
    for (const Enemy& e : v) acc = acc * 31 + e.id;   // the same fold
    return acc;
}
```

Notice that almost nothing is written by hand here. No copy constructor, no assignment, no `==`, no four comparison operators; the compiler generated all of that itself and generated it correctly, because every field now behaves like a value: `int` does, `Loot` does, so `Enemy` does too. Regularity assembles from the bottom up, and that is perhaps its most pleasant property: build a type out of values and it gets value behavior for free.

Looting a copy no longer touches the original; it still has its hundred gold, and two "machines" that assembled eight enemies from the same logical state with different allocation order now produce the same fold, `947656708` and `947656708`, instead of the diverging `116267632558` and `165191219038`. There is no desync, and we removed it by making addresses disappear from the simulation; no clever comparison of equals was needed for that.

It may turn out that you do not need `Enemy` as a value at all. If an enemy is an entity on the battlefield, with its own identity and its own lifetime, then two enemies with the same gold and ammo are not "equal" at all; they are merely similar, and the right answer then is simply not to write `==` at all. Regularity is for values: damage, a coordinate, `Fixed`, a string in a results table. The trouble with the previous `Enemy` was that we wanted to hang value behavior on a type that was irregular by meaning. It copied, it compared, it looked like a value, and it was not one, and the bugs from that surfaced here and there.

## What this means for your types

**Count equality only by value.** Neither an address, nor `capacity`, nor creation time, nor a cell number should enter `==`. Everything you let in there you will later have to drag into ordering and into the hash, or they will diverge from equality and containers will start losing your objects.

**A comparator must be strict.** For equal elements it must return `false`, that is `<`, not `<=`. A debug MSVC build still catches a violation with an `invalid comparator` check, while release will stay silent and the bug will most likely ship to players.

**A copy must be independent.** If after copying a change to one object is visible in the other, you do not have a value type; you have two names for one thing.

**Traversal order must not depend on addresses.** Sorting by pointer is not reproducible across runs, and neither is an `unordered_map` with pointer keys; three runs of the same binary will give three different traversal orders. With value keys, `int` or `string`, traversal is stable from run to run.

`std::regular` **checks syntax, not meaning.** A checkmark from the compiler will not help you avoid mistakes if you have not thought through your type.

Regularity of a type is not about beauty and not about matching a concept; it is about the right to replace equals with equals. While you guarantee that, `std::sort` will sort correctly, `unordered_map` will find, and a replay on two machines will reproduce the same way. The moment regularity is lost, the ability to reason about the correctness of the written code is lost with it, and only bugs remain.
