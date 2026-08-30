# Values

There is a bug that makes most networked-game developers twitch. Everyone runs the same build, the server tick is synchronized, but someone sees the enemy two meters to the left of where others do, then grenade trajectories diverge, then the whole scene diverges, and if you ignore that, nobody will play the game. You will find nothing in the logs, nothing was lost on the wire, and every machine honestly computed the same thing. Well, computed... not quite the same thing: adding two numbers in a different order, or the optimizer folding an expression on one platform and leaving it on another, or a positive zero meeting a negative zero somewhere. Think there are few negative zeros in our games? Enough for the drift to multiply frame by frame until it becomes visible to the eye. The cause is not the network and not game logic; "the same number" turned out not to be the same, and the word "value" is trickier than it looks.

To get at the nature of that bug, we have to start at the bottom, with bits. When you run a program and look at memory, there is no `C++`, no types, not even numbers, only long sequences of zeros and ones. The processor does not know what "integer 42", "float 3.14", or "character 'A'" means; they are all different ways to read the same bits. Which reading is correct is not written in the bits themselves; the programmer, compiler, and architecture agree on it.

### Data, interpretation, and well-formedness

Take 32 bits and agree to read them as a signed integer. There are exactly 2³² bit strings of that length, the same count of integers in the allowed range, and each string maps to only one number. An agreement of this kind is called a value type: it says which bit strings we treat as well-formed representations of entities of a given kind, and which we do not.

If we have a concrete entity, say the integer "minus five", the bit pattern we use to encode it in memory is called a representation of that entity, and the entity itself, the number as a mathematical object, is the interpretation of those data. The word "value" covers both at once. Saying "32 bits 1111…" says nothing; a value is "a 32-bit signed integer in two's complement with value -5" or "a pair of 32-bit integers read as numerator and denominator of a rational number".

![](images/img_4275fc83ba.png)

For example, ordinary `int` on many platforms is 32 bits in two's complement, where bit pattern 0x00000001 is interpreted as 1 and 0xFFFFFFFF as -1. A rational number can be represented as concatenation of two such 32-bit integers, but not every bit sequence is a meaningful representation for an arbitrary value type. Data are well-formed relative to a type if they actually represent some abstract entity of that kind.

Any 32-bit sequence, if we agreed to interpret it as two's-complement integer, is well-formed: every bit pattern corresponds to some integer from minimum to maximum. And this is not just a property of familiar hardware: since `C++20`, two's complement for signed integers is in the standard, so ones' complement or sign-and-magnitude variants no longer exist in the language; before that they were formally allowed, though platforms of that kind are gone now.

But if we take "real number" in the mathematical sense and try to read arbitrary IEEE 754 values that way, we eventually hit `NaN`. Those bits mean "Not a Number" by standard, and they are not a well-formed representation of a real number, yet in `C++` `NaN` is a perfectly legal value of `float`/`double` that you can store, copy, and pass, and arithmetic on it follows its own rules and usually yields `NaN` again.

```cpp
    // Each of these bit patterns is meaningful as int
    uint32_t bits1 = 0x00000001u;
    uint32_t bits2 = 0xFFFFFFFFu;

    int32_t a, b;
    std::memcpy(&a, &bits1, 4); // 1
    std::memcpy(&b, &bits2, 4); // -1  (two's complement)
```

With floating point the trick fails, because there are bit patterns that correspond to no real number.

```cpp
    double y = std::nan("");   // "not a number"

    std::cout << "y = "        << y                 << "\n";
    std::cout << "y + 1  = "   << y + 1             << "\n";
    std::cout << "y * 0  = "   << y * 0             << "\n";
    std::cout << "y == y = "   << (y == y)          << "\n";
    std::cout << "sqrt(-1) = " << std::sqrt(-1.0)   << "\n";
```

```text
y = nan
y + 1  = nan
y * 0  = nan
y == y = 0
sqrt(-1) = nan
```



### Total and partial types

A type that expresses every entity of its kind is called total; one that covers only part is properly partial. `int` is partial, as you have probably already figured out, but the price of that partiality is usually underestimated.

Partiality of the type makes operations on it partial too: adding two perfectly valid `int`s may have no result in `int`, and signed overflow in `C++` is UB. The hole appears even where you do not expect it and the two's-complement range is asymmetric and has one more negative value, so `INT_MIN` has no opposite, and `-INT_MIN` together with `std::abs(INT_MIN)` are also UB. Taking absolute value, an operation that looks defined everywhere, is undefined on one value.

On 16-bit systems `int` was in range -32768…32767; now almost everywhere it is 32 bits, roughly -2³¹…2³¹-1. When moving to 64 bits, `int` itself did not grow in any model, but on Unix LP64 `long` stretched, on Windows LLP64 it stayed 32-bit, and only pointers, `long long`, and `size_t` grew.

For `float`, partiality is uneven too: the mantissa has 24 bits, so the step between representable values grows with the magnitude. At one meter from the origin neighbors are ten-thousandths of a millimeter apart; at a kilometer about 0.06 mm; at ten kilometers a millimeter; at a hundred almost a centimeter. On an open world map it looks smooth near zero, but at the edge of the world objects start to jitter because position snaps to the nearest grid node, and nodes there are sparse.

Totality is better. `bool` is total for logical values; `enum class Suit` is total for suits because there are exactly four and all four are in the enum. The benefit is that `switch` on a total type covers all cases and needs no `default`.

```cpp
enum class Suit { Clubs, Diamonds, Hearts, Spades };

// the kind is finite, the type covers it entirely, so default is not needed,
// and -Wswitch will remind you if a fifth suit appears in the enum
const char* to_string(Suit s) {
    switch (s) {
        case Suit::Clubs:    return "clubs";
        case Suit::Diamonds: return "diamonds";
        case Suit::Hearts:   return "hearts";
        case Suit::Spades:   return "spades";
    }
    return "?"; // unreachable while s holds a well-formed Suit
}
```



### Representation uniqueness and ambiguity

A separate practical topic is representation uniqueness: whether each abstract entity corresponds to at most one value of a given type. If we implement logical value as a byte where zero is false and any nonzero is true, representation is not unique: 0x01, 0xFF, and 0x7F all mean `true`, and now equality by meaning does not match equality of bit content.

A type representing integer as "sign bit plus magnitude" also lacks unique representation of zero, because "+0" and "-0" are different bit patterns. Two's complement, conversely, has unique representation: each integer in range is encoded by only one bit pattern, and zero has no "+0"/"-0" variants. Compilers and processors like such types because equality is trivial bitwise comparison, and the optimizer can use many tricks assuming replacing equal with equal preserves both bits and meaning.

But it also goes the other way, when we deliberately choose non-unique or even ambiguous representation, when one value can have more than one interpretation. Then you cannot recover the abstract entity from bits alone without extra context.

An example here is calendar year encoded as two decimal digits: "42", which may mean 1942, 2042, or something else depending on century, so the same data admits several interpretations and the type is ambiguous by itself, but we have an example much closer.

Entities in a game usually live in an array; each has an index; other subsystems find it by that index; when an entity dies the slot frees and then goes to a new one. The index is still the same number, but now it means a different entity, and a reference someone held too long starts pointing the wrong way, the worst kind of wrong because a live valid object sits there and nothing crashes. One bit sequence, two interpretations, and you cannot tell them apart from bits because context is missing.

This is treated by adding the missing context to the representation. Put a generation counter next to the index, and the reference becomes a pair where the low part says where to look and the high part says which generation of the slot it belonged to. A stale reference now differs from a fresh one in bits and can be rejected at the gate.

> The same disease lives far outside the game loop. A date stored without time zone takes less space and compares faster, but bitwise equality stops guaranteeing anything: two identical bit patterns can mean different moments in time, and the same moment written in two zones gives different bits.

![](images/img_fb17ab01e3.png)

### Two equalities and the price of comparison

To handle these cases carefully, distinguish two kinds of equality: equality as coincidence of interpretation, and representational equality as literal identity of bit sequences. Two values of one type are equal if they describe the same abstract entity; they are representationally equal if the sequences of zeros and ones in memory are identical.

If a type has unique representation, equality by meaning automatically implies equality by bits, because each entity has only one legal representation. If a type is not ambiguous, that is, each bit sequence corresponds to at most one entity, then representational equality implies equality by meaning: same bits, same entity.

So for ordinary two's-complement `int`, operator `==` essentially coincides with comparing representations: same meaning means same bits and vice versa. For `double` and `float` the picture differs: machine comparison (`==` per IEEE 754) is neither pure bit comparison nor pure mathematical equality: `+0.0 == -0.0` is `true` with different bits, and `NaN == NaN` is `false` even when bit patterns match. The compiler still lowers this to one or two instructions, but they follow floating-point rules and do not look like "memcmp over bytes".

```text
32-bit float bit layout (IEEE 754):

  31 30         23 22                                      0
 ┌──┬─────────────┬─────────────────────────────────────────┐
 │s │  exponent   │                fraction                 │
 │  │   (8 bits)  │                (23 bits)                │
 └──┴─────────────┴─────────────────────────────────────────┘
  1 bit

 1. Comparing +0.0f and -0.0f:
    +0.0f: 0 [00000000] [00000000000000000000000]  (sign bit = 0)
    -0.0f: 1 [00000000] [00000000000000000000000]  (sign bit = 1)
    Bit patterns are DIFFERENT (memcmp returns false),
    but IEEE 754 specifies that operation (+0.0f == -0.0f) returns true.

 2. Comparing NaN (Not-a-Number):
    NaN:   x [11111111] [non-zero fraction...   ]
    Bit patterns can be IDENTICAL (memcmp returns true),
    but IEEE 754 specifies that any comparison with NaN returns false (NaN == NaN yields false).
```

But in real life we often use types where uniqueness is deliberately broken for efficiency of creating and converting values. Rationals stored as numerator/denominator pairs without automatic reduction are the classic example: 1/2 and 2/4 are the same abstract entity but different representational forms, and checking equality by meaning requires reducing both fractions or cross-multiplying, much costlier than comparing two bit patterns.

Cross-multiplication has its price: `num * o.den` can overflow `int64_t`, and signed overflow in `C++` is UB. Fine for a teaching example; production code needs wider arithmetic or comparison via reduction. And since we speak of well-formed representations, the denominator must be nonzero and its sign normalized to plus, and the code must enforce that, not a comment next to the field, or the type allows building a value that represents nothing.

```cpp
#include <cassert>

// Non-unique representation
struct Rational {
    int64_t num; // numerator
    int64_t den; // denominator, strictly positive

    // Invariant lives here, not in a comment next to the field,
    // or well-formedness stays on the programmer's word alone
    static Rational make(int64_t n, int64_t d) {
        assert(d != 0);
        return d > 0 ? Rational{n, d} : Rational{-n, -d};
    }

    // comparing representations, fast and not about meaning
    bool naive_eq(const Rational& o) const {
        return num == o.num && den == o.den;
    }
    // slow check with normalization
    bool normalized_eq(const Rational& o) const {
        auto g1 = std::gcd(num, den);
        auto g2 = std::gcd(o.num, o.den);
        return (num / g1) == (o.num / g2) && (den / g1) == (o.den / g2);
    }
    // slow check with cross multiplication
    // careful: num * o.den can overflow int64_t
    bool cross_eq(const Rational& o) const {
        return num * o.den == o.num * den;
    }
};

void demo_rational() {
    auto a = Rational::make(1, 2); // 1/2
    auto b = Rational::make(2, 4); // 2/4, same abstract entity

    std::cout<<"1/2 naive_eq 2/4:"<<a.naive_eq(b)<<"\n"; // 0, representations differ, entity is one
    std::cout<<"1/2 normalized_eq 2/4:"<<a.normalized_eq(b)<<"\n"; // 1
    std::cout<<"1/2 cross_eq 2/4:"<<a.cross_eq(b)<< "\n"; // 1
}
```

Rotations tell the same story: quaternion `q` and quaternion `q'` with all four component signs flipped define the same rotation, one abstract entity, but different bytes in memory because each component's sign bit is flipped. Representation is non-unique again, and here we did not agree to that for speed; it is built in, and you cannot avoid it.

For us that means bitwise comparison of two rotations is useless, and equality must go through the dot product, modulo sign, because sign does not matter. And if you naively interpolate between those two representations, the animation takes the long way and the character's arm twists wrong, for example when gripping a weapon, because you took two representations of the same entity and averaged bits instead of comparing by meaning. So rotation interpolation first checks the sign of the dot product and flips one quaternion if needed.

```cpp
// One rotation, two representations: q and -q
struct Quat { float x, y, z, w; };

static float dot(const Quat& a, const Quat& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Bitwise different, same rotation by meaning
bool same_rotation(const Quat& a, const Quat& b, float eps = 1e-5f) {
    return std::abs(dot(a, b)) > 1.f - eps; // absolute value, sign does not matter
}
```

Another example is finite sets. They can be stored as unsorted lists, and then equality requires sorting and deduplication before comparing element by element. Same as with fractions: insertion gets cheaper, equality gets costlier, reasonable while you compare less often than you add. Standard containers show the same trade: a tree (`std::set`) has cheap ordered traversal and lookup by comparator but logarithmic insert and allocations; a hash table (`std::unordered_set`) has cheaper average insert and membership test, but equality and hash must stay consistent and hash quality and worst cases become their own cost line.

```cpp
// cheap insert, expensive equality
struct UnsortedSet {
    std::vector<int> elements; // order and duplicates arbitrary

    void insert(int x) {                              // O(1)
        elements.push_back(x);
    }

    bool operator==(const UnsortedSet& other) const { // O(n log n)
        auto a = elements;
        auto b = other.elements;
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        a.erase(std::unique(a.begin(), a.end()), a.end());
        b.erase(std::unique(b.begin(), b.end()), b.end());
        return a == b;
    }
};

// same kind, trade reversed
struct SortedSet {
    std::vector<int> elements; // invariant: sorted, no duplicates

    void insert(int x) {                              // O(n), tail shift
        auto it = std::lower_bound(elements.begin(), elements.end(), x);
        if (it == elements.end() || *it != x) {
            elements.insert(it, x);
        }
    }

    bool operator==(const SortedSet& other) const {   // O(n)
        return elements == other.elements;
    }
};
```



### When equality is unreachable

Sometimes implementing "true" behavioral equality is too expensive or even theoretically impossible. If you represent a computable function as a piece of code or a data structure describing an algorithm, then "two functions are equal if they give the same result on all possible arguments" turns out undecidable.

In general you cannot algorithmically check that two arbitrary algorithms behave the same on the whole domain; that follows from Rice's theorem, and then you settle for weaker equality, comparison of representations, when two values are equal only if their bit images match, meaning the same object in memory or an exact copy.

Values live in memory and have addresses, but a correctly implemented function behaves as if addresses do not matter, because it looks only at argument content, and if you copy a value elsewhere, applying the function to the old and new copy gives the same result.

### Regularity and substitution

When we say a function is defined on a value type and is regular, we mean it "respects" equality: if you replace an argument with any other equal value, the function's result does not change. For ordinary finite `double` with fixed rounding mode, familiar operations like `+`, `*`, or `sin` are regular, that is, equal inputs per `==` give equal outputs per `==`. Step outside finite numbers and the picture breaks, and what breaks it is `NaN` and the sign of zero.

From type theory and the compiler optimizer's view this matters because such functions allow substitution, replacing expressions with equal ones without changing program behavior, while irregular functions do not, and the choice of result inside them may depend on which bit pattern was used to store the value.

```cpp
double pos_zero =  0.0;
double neg_zero = -0.0;

std::cout<<"\n+0.0 == -0.0:"<<(pos_zero == neg_zero)<<"\n";
// 1, equal per ==

std::cout<<"signbit(+0.0):"<<std::signbit(pos_zero)<<"\n";
// 0

std::cout<<"signbit(-0.0):"<<std::signbit(neg_zero)<<"\n";
// 1, different result!
// signbit breaks regularity:
// replaced equal with equal, but the result changed
```

In C++ compiler history this distinction surfaced repeatedly in discussions of allowed optimizations: whether you may reorder floating-point computations or replace expressions with mathematically equivalent ones, when IEEE 754 format and `NaN`, infinities, and rounding subtleties make many familiar identities like associativity of addition false.

```cpp
float a = 1e15, b = -1e15, c = 1.5;

float left  = (a + b) + c; // (1e15 - 1e15) + 1.5 = 0 + 1.5 = 1.5
float right = a + (b + c); // 1e15 + (-1e15 + 1.5), precision loss

std::cout << "\n(a+b)+c = " << left  << "\n"; // 1.5
std::cout << "a+(b+c) = " << right << "\n"; // may differ
std::cout << "Equal:   " << (left == right) << "\n";
// With -ffast-math the compiler may reorder and get a different result
```

```text
(a+b)+c = 1.5
a+(b+c) = 0
Equal:   0
```

The standards committee and compiler developers sought balance between aggressive optimization and preserving correctness for functions and types the programmer treats as regular, and modes like `-ffast-math` in GCC and Clang and `/fp:fast` in MSVC deliberately weaken the regularity guarantee for performance. Such flags are everywhere in game projects because the frame usually matters more than the last mantissa bit, but the bill arrives later when the same scene computes slightly differently on two machines and replay diverges from the original. That is the desync the chapter opened with, and now you should understand why the logs were clean: nothing was lost on the wire, but why the values themselves diverged.

When designing a type, you must decide both questions together: which equality you want, bitwise or behavioral, with unique representation or without, and which functions over the type must stay regular so you and the compiler can safely substitute equal for equal.
