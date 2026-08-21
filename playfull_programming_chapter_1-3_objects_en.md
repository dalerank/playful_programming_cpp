# Objects

I'll start with a story that, in one form or another, has happened to everyone who wrote logic for game units. A tower holds a pointer to its target, the unit under fire dies, its memory goes back into the unit pool, and a frame later a brand-new unit appears in that same slot, freshly spawned on the other side of the map. The tower's pointer never changed and still points at a live, valid object of the right type, whose fields you can read and whose methods you can call, so nothing crashes. But for the player it looks like a game bug, because the tower suddenly stops shooting (the new-old enemy turned out to be outside firing range) even though there are plenty of other enemies around it. The pointer all along answered "where does it live", while the tower was asking "who is this", and while the previous unit was alive both questions gave the same answer, but "where" and "who" are questions of different levels: you can mix them, but you shouldn't shake them.

To see where this difference between "where" and "who" comes from, we have to drop a couple of floors down the architectural staircase. When we say "object" in programming, it is very easy to jump straight to classes, methods, and all the OOP attribute fluff, even though it really starts much lower, with memory itself and how bits live in it. Picture memory as a big field of cells, each with an address (also a number) and fixed-width contents (a word or a byte, a set of bits), and to read a word at an address the processor does a load, and to change the "address → contents" binding it does a store. On modern machines that role is played by regions of RAM, and on disk the same model is implemented by blocks on an SSD or HDD, only with different latencies and rules, but the principle is the same: there is an address, there is content, there is read and write.

## An object as a contract

Against that background, an object can be understood as a contract between the programmer, the compiler, and the machine, that is, as a way to represent a concrete entity from our problem domain as a value living in memory. Every object has state, and that state is simply a value of some type. If the object describes that same unit from the story above, its state at the current moment is a "snapshot" of all the attributes we decided to store: map position, health pool, current target, team number.

That state can change over time, and that is what distinguishes an object from a plain value, because a value by itself, as a mathematical representation, is immutable, while an object can hold different values at different moments. The unit caught a projectile, and instead of 80 the health field now holds 60, but the number 80 itself did not go anywhere and did not become a different number; the object that stored it changed. To hold its state, an object owns a set of resources, and those can be words of memory on the heap, records in a file, rows in a database, or in the simplest case just a few consecutive bytes handed to a local variable on the stack.

## Why objects at all

The reason is how real computers are built. Like a classical Turing machine, they rest on stateful memory, so even "pure" functions at the language level end up executing through reads and writes of cells, registers, and cache, that is, through manipulating concrete objects or their short-lived copies in registers. So even when what we care about is a purely abstract value, like the square root of a number or the solution of a system of linear equations, we compute it with an algorithm over mutable memory, iterative methods, and accumulation of intermediate sums, and the object turns out to be the tool with which values are computed. That objects are also convenient for modeling mutable entities, a unit in a game, an employee record, or the state of a GUI window, is more a pleasant consequence than the cause.

![](images/img_f95fa1faba.png)

## Where the value physically lives

Contract or not, we still have to look at how a value actually sits in memory. Take that same unit with position, health, and AI state, and lay out an array of such units the obvious way first, with fields next to each other.

```cpp
// array of structures: one unit's fields sit next to each other
struct Unit_AoS {
    float x, y, z;  // map position
    int hp;         // health
    int ai_state;   // AI state
};

void example_aos() {
    std::vector<Unit_AoS> units = {
        {10.0f, 0.0f, 5.0f, 100, 0},
        {12.0f, 0.0f, 7.0f,  80, 1},
        {14.0f, 0.0f, 9.0f,  60, 2}
    };

    // natural access, the whole unit is at hand
    std::cout << "AoS: " << units[1].x << ", " << units[1].z
              << ", hp " << units[1].hp << "\n";
}
```

```text
===================================================================
ARRAY OF STRUCTURES (AoS) - the classic layout
===================================================================

Memory (bytes contiguous):

  ┌─────────────────────────────┬─────────────────────────────┐
  │           Unit[0]           │           Unit[1]           │
  ├─────┬─────┬─────┬─────┬─────┼─────┬─────┬─────┬─────┬─────┤
  │  x  │  y  │  z  │ hp  │ ai  │  x  │  y  │  z  │ hp  │ ai  │
  │10.0 │ 0.0 │ 5.0 │ 100 │  0  │12.0 │ 0.0 │ 7.0 │  80 │  1  │
  └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘

  0x1000                        0x1014                        0x1028
```

You also have to understand that although we logically think of an object's value as a continuous sequence of zeros and ones, the physical resources that hold those bits are not required to be contiguous. That same unit is a good example: its position is a triple of `float`s that may sit as one chunk inside a structure, or may be spread across different arrays, and the interpretation will still assemble it into one logical whole. If we manually switched from an array of structures `{x, y, z, hp, ai}[]` to separate arrays `x[]`, `y[]`, and `z[]`, then the fields of one logical unit physically live in different regions of memory. In the `C++` abstract machine these are still separate objects of type `float` (plus maybe a wrapper like `PositionRef`), and "one unit" here means our program's agreement about interpretation, not one language object.

Fields get split up, of course, not out of love for art. Take a hundred thousand entities whose position alone is checked. If the processor walks memory in 64-byte cache lines, then in an "array of structures" layout the rest of the unit arrives in cache along with the three needed floats: health, AI state, model references, and whatever else has piled up over years of development. Most of the memory traffic goes to data nobody needs in this loop. Lay those same entities out as a "structure of arrays", and positions sit contiguously, each line packed with what is needed right now, and on hot loops the difference is no longer measured in percentages.

You can estimate it on a napkin as the ratio of the unit's size to the size of its hot fields. In our toy `Unit_AoS` the useful twelve bytes of position sit next to only eight extras, so the win is modest, less than 2×, and there is no point building two layouts for that. But a real engine unit weighs not twenty bytes but a hundred-plus, and then each pulled 64-byte line still gives you those same twelve bytes of position, while the rest just occupies the bus and the cache. In a "structure of arrays" five positions in a row fit in that same line, and memory traffic differs by a factor of five, which is no longer a matter of taste.

```cpp
// structure of arrays: one unit's fields are spread across memory
struct UnitArray_SoA {
    std::vector<float> x;   // all x coordinates in a row
    std::vector<float> y;   // all y
    std::vector<float> z;   // all z
    std::vector<int> hp;    // health in a separate array

    size_t size() const { return x.size(); }

    // unit i's position is (x[i], y[i], z[i]), three references into
    // different arrays, not one language object you can take a pointer to
    struct PositionRef {
        float& x;
        float& y;
        float& z;

        PositionRef(float& px, float& py, float& pz): x(px), y(py), z(pz) {}

        void print() const {
            std::cout << x << ", " << y << ", " << z << "\n";
        }
    };

    PositionRef position(size_t i) {
        return PositionRef(x[i], y[i], z[i]);
    }
};

void example_soa() {
    UnitArray_SoA units;
    units.x  = {10.0f, 12.0f, 14.0f};
    units.y  = { 0.0f,  0.0f,  0.0f};
    units.z  = { 5.0f,  7.0f,  9.0f};
    units.hp = {  100,    80,    60};

    // hot loop touches only positions, and they sit contiguously
    for (size_t i = 0; i < units.size(); ++i) {
        units.x[i] += 1.0f;
    }

    std::cout << "SoA: ";
    units.position(1).print();
}
```

```text
===================================================================
STRUCTURE OF ARRAYS (SoA) - the split layout
===================================================================

Memory (arrays in different places):

  Array x[] (all x coordinates):
  ┌────────┬────────┬────────┐
  │  10.0  │  12.0  │  14.0  │  ← x[0], x[1], x[2]
  └────────┴────────┴────────┘
  0x2000   0x2004   0x2008

  Array y[] (all y coordinates):
  ┌────────┬────────┬────────┐
  │   0.0  │   0.0  │   0.0  │  ← y[0], y[1], y[2]
  └────────┴────────┴────────┘
  0x3000   0x3004   0x3008

  Array z[] (all z coordinates):
  ┌────────┬────────┬────────┐
  │   5.0  │   7.0  │   9.0  │  ← z[0], z[1], z[2]
  └────────┴────────┴────────┘
  0x4000   0x4004   0x4008

Logical position Unit[1] = {x[1], y[1], z[1]}
                           = {12.0 from 0x2004, 0.0 from 0x3004, 7.0 from 0x4004}
```

![](images/img_abf5d945ed.png)

You pay for this immediately, and not only with code beauty. The logical entity no longer has one address, and it has no single language object either, only an agreement that elements with the same index belong to one unit, and you cannot take a pointer or reference to that entity as a whole, only assemble a wrapper of field references like `PositionRef` above. And if a pass, on the contrary, touches all fields at once, SoA loses, because instead of one cache line you pull as many independent memory streams as you have arrays.

In distributed systems the resources of one logical entity can end up in different kinds of memory: some in RAM, some on disk or in remote storage. Usually, though, when we talk about an object in `C++`, we mean one process and one address space. A "classic" object with a contiguous representation has a starting address and field offsets, while for split schemes like SoA the logical entity may have no single address at all.

## Object type and value type

An object type in this picture of the world is a template for how we store and change values in memory, and you can say that for every object type there is a corresponding value type that describes all admissible states of objects of that type.

If we agreed that our object is a 32-bit signed integer in two's complement, little-endian byte order, aligned to a 4-byte boundary, then we have thereby defined a concrete object type, and the compiler now knows its size in bytes, where it may sit in memory, which processor instructions to use for load and store, while the value type for this object is the set of all integers in the allowed range. Part of this contract the language signed for us itself, and, as we saw in the chapter on values, rather late: two's complement became mandatory only with `C++20`.

Any concrete `int` object in a program belongs to this object type, its state is some concrete value, and its physical realization is a set of bytes in memory that the compiler accesses at a known address, or, if the object sits inside a structure, at an offset from its start. An important point worth realizing when you first try to write code is that values and objects are complementary, but fundamentally different in role.

Values as mathematical entities are immutable and do not depend on how we encoded them in memory, and the number 42 can be written on paper, spoken aloud, sent over the network as text or in a binary format, and the value itself does not change. Objects live differently: they are tied to a concrete machine and implementation, they have an address, a set of bytes, and a mechanics of change, and they move from one state to another over the life of the program. But an object's state at a concrete moment is always described by a value, so if you take a `std::string`, look at it as a sequence of characters, and serialize it to JSON, you get a "snapshot" of that state.

When we discuss equality of objects, we usually want to talk about equality of their states as values, abstracting away from the fact that one `std::string` stores its data in one region of memory and another in another. That view is especially familiar in languages that emphasize immutable data (where changing state is either forbidden or pushed to the edge), but in `C++` too it lets you separate "what" from "how" and not tie everything to concrete addresses.

Some properties of value types carry over to object types as well. If a value is well-formed and its binary representation is admissible for the chosen type, then an object with that state is well-formed too. If a value type is partial, like `int` relative to all integers, then the corresponding object type is partial too. And if the representation is unique, as with integers in two's complement, semantic equality for such objects often coincides with representational equality.

You can ask the language about the last point directly. `std::has_unique_object_representations_v<T>` answers "yes" when the type has no alignment holes and any two semantically equal objects match bit for bit. That is the permission the compiler relies on when it reduces comparison of trivially copyable types to a few word comparisons or straight to `memcmp`.

Without that permission the trick falls apart, and a "blind" `operator==` via `memcmp` over a whole structure with alignment holes yields false inequalities, because the standard makes no promise at all about the contents of padding bytes. You catch it very simply.

```cpp
struct Shot {
    char team;    // 1 byte
    int  damage;  // 4 bytes, with a three-byte hole before them
};

static_assert(sizeof(Shot) == 8);
static_assert(!std::has_unique_object_representations_v<Shot>);  // the hole blocks it
static_assert(std::has_unique_object_representations_v<int>);    // no holes here

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
```

That is why comparison is written field by field, and bytewise comparison is left to those types for which the language itself answered "yes".

## Object lifetime

All this time we have talked about an object as a contract, but the language already has its own definition that we need: an object is a region of storage that has a type, a storage duration, and a lifetime. We have already said enough about type and about where that region sits, but lifetime has not yet appeared in the conversation, even though without it half of what was said will not work.

An object does not begin when memory was found for it, but when its initialization finished, and it ends when the destructor starts, i.e. before the first moment and after the second there are perfectly live bytes at that same address, but there is no object there. That leads us to the fact that memory and the object physically live for different times, and a vector's buffer outlives the elements inside it and calmly survives several generations of those elements.

In games everything that has to run fast is built on this: memory for projectiles, particles, or units is allocated once per match or per frame, and inside that memory objects appear and die by the thousands, that is, a projectile finished, the destructor ran, and a frame later a different projectile already lives in that same place. The address matched, but the object is different, and there is no rule violation here; on the contrary, that is what the pool was for.

```cpp
struct Projectile { int id; float ttl; };

// One memory slot; at different times different objects live in it.
// A detailed talk about placing objects in ready-made memory comes later;
// right now what matters is only that the address matches and the objects differ.
alignas(Projectile) unsigned char slot[sizeof(Projectile)];

void pool_demo() {
    Projectile* first = new (slot) Projectile{1, 0.5f};
    first->~Projectile();                   // first object's lifetime ended

    Projectile* second = new (slot) Projectile{2, 1.0f};
    assert((void*)first == (void*)second);  // the same place
    second->~Projectile();                  // but the object here was already different
}
```

And here the story from the opening of the chapter unravels. The tower held a pointer, that is, it held a place, and the pool handed that place to the next unit, and from the language's point of view everything is flawless, because the target's lifetime ended, the new unit's lifetime began, and the pointer points at a live object of the right type. What broke was not in memory but in our head: we wrote into the pointer an answer to "who is this", while it can only answer "where does it live", and those two answers coincided only until the end of the target's lifetime.

## The notion of identity

Object identity deserves a separate conversation, because in the real world concrete entities remain themselves even through a complete change of state. Over a match a unit managed to change position, lose three quarters of its health, pick up a different weapon, and after capture move under a neighboring player's control, but in the match log and the results table it is still the same unit. An account remains the same account even if its nick, avatar, and rating changed. And overall this question is much older than programming: is a ship whose planks were all replaced still the same ship, or already a different one? And is a person whose body cells renew over 40 days still the same person?

To reflect this in a program, entities need a definition of identity separate from current state. The most convenient approach is to introduce an identity token, a unique value for "who is this", not "what state is it in now". The token can be a memory address, an index in an array, an employee number in an HR system. Do not mix the levels, because address equality answers "is this the same place in memory?", while equality of employee numbers or entity ids answers "is this the same domain entity in our model?". And even the first question does not coincide with "is this the same object?", because one address is legally shared by nested objects, say a base subobject and a derived object, or a structure and its first field, and after the end of a lifetime, as we just saw with the pool, the address goes to the next object. An index in a `vector` is closer to the second (a position in a collection) and by itself does not survive arbitrary reshuffles without extra discipline.

Over the life of a program an identity token can change, for example an element "moved" to another buffer after reallocation, a record got a new key, or an entity changed its handle. In the `C++` model the object itself is often already a different one (the old destroyed, and a new one created at a different address) or the same storage at the same address but with a different state after `move`. The logical identity of the domain entity is preserved only if we ourselves maintain a stable token (id) and a mapping "old pointer/index → same id".

Finally, it is worth separating equality of objects from their identity. Two objects of the same type are equal if their states are equal as values: two `std::string`s both containing `"hello"` are equal even if they sit in different places in memory and have different internal buffers. And likewise two `std::vector<int>` vectors containing the same sequences are equal as sequences of values.

In that case it is natural to say that one object is a copy of the other, and any changes made in one do not affect the copy. Equality checking here rests on the value model, not on addresses, while identity answers a different question: "is this the same object, or just another object with the same state?".

## How compilers did it

The difference between "equal" and "the same one" has been an implementation detail. Old libraries quietly shared internal buffers between objects, and the best-known case is copy-on-write in `std::string`: libstdc++ kept COW strings until GCC 5, that is, another four years or so after `C++11` effectively forbade that behavior for `basic_string`, and even afterward the old ABI never went away and is still available. The price was thoroughly practical bugs, when the reference count diverged between threads, and any non-const access to the string split the shared buffer and invalidated previously issued references and iterators.

```cpp
// Old libstdc++ with COW strings, before GCC 5:
std::string s1 = "hello";
std::string s2 = s1;  // Buffer is NOT copied

// s1 and s2 point to ONE buffer in memory
// Inside, reference count = 2

// The buffer splits already on the non-const operator[] call itself,
// which hands out a mutable char&, not on assignment.
// begin(), at(), and other non-const members worked the same way.
s2[0] = 'H';
```

![](images/img_2179d4a5a6.png)

In early cfront an object was literally a C structure. A method turned into a free function that received `this` as its first parameter; after mangling it got a name like `move__5Pointii`, the structure's address played the role of identity token, and the distinction between value and object lived only in comments and documentation. In `Borland C++` and early MSVC, object layout was tightly bound to the platform's memory model (alignment, near and far pointers, `#pragma pack`, and calling conventions), so for low-level memory work and binary compatibility you had to account for x86 details explicitly, even if the object looked "abstract".

Today each compiler has its own intermediate language: GCC runs code through GIMPLE and then through RTL, Clang through LLVM IR, MSVC has its own IL between frontend and backend. What they still share is the object as a source of mutable state and the value as an immutable entity, and now values live in intermediate-language slots while memory is accessed by separate loads and stores. The optimizer may temporarily keep an object's state in registers (or fold it entirely into constants) if under as-if nobody observes the address and the extra stores to memory, and when needed the state materializes in memory again.

```text
C++:
  struct Point { int x, y; };
  void foo() {
    Point p;
    p.x = 10;
    p.y = 20;
    use(p.x + p.y);
  }

Without optimizations (written in the spirit of LLVM IR):
  %p = alloca Point
  store 10 -> p.x
  store 20 -> p.y
  load p.x, load p.y, add, use

After optimizations (as-if, address of p was not observed):
  use(30)
  // addition folded on constants already,
  // there may be no separate Point in memory,
  // though in the abstract machine object p still "was"
```

## A token that survives relocation

In this picture an address stops being a convenient token of an entity's *logical* identity, because the optimizer may not keep a stable address at all (if nobody takes it), and move semantics plus container reallocation create new objects at new addresses anyway. You can store a pointer to a `std::vector` element and treat it as a token, but any reallocating operation invalidates both pointers and iterators. An index in a vector remains a meaningful position ("which in order"), though that is no longer "the same `C++` object", but a different model where "who is this" is set not by "where it lives in RAM" but by "which place in the sequence" or by a separate stable id. Containers like `std::list`/`std::map` have iterators more resilient to insertions, but for `vector` you should not count on reallocation. This is checked in a few lines, and it is better not to keep the old pointer at all, but to turn the address into a number right away.

```cpp
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
    assert(reinterpret_cast<std::uintptr_t>(units.data()) != addr);

    // addr is now just a number: objects live at a different address,
    // and dereferencing the old pointer would be undefined behavior
}
```

The addresses could not have matched here, because the new buffer is allocated before the old one is freed: the elements have to be moved somewhere first.

![](images/img_f690b15366.png)

In modern game systems the identity token is increasingly an explicitly introduced value independent of the address: in popular ECS architectures each entity gets a numeric identifier at creation, and it stays unchanged no matter how components move in memory, and this is usually not a flat counter but the familiar pair from the values chapter of a slot index and a generation number, because the slots themselves are reused and without a generation the new occupant of a slot would be indistinguishable from the previous one. In databases primary keys play the same role, and in distributed systems people often take a UUID/GUID (a typical random UUID needs no central registrar, though not all identifier schemes are like that). The common idea is one: the token should be a stable value that survives moves and data reorganization and that you can safely store and compare without fear that reallocation, relocation, or a change of physical address will suddenly make the reference meaningless.

The practical rule that comes out of all this is: a raw pointer, reference, or index is an excellent way to say "where", but they only work inside one pass while you yourself guarantee that nobody spawns, dies, or reshuffles the array. As soon as a reference must survive a frame, land in an event queue, a save, or another thread, it has to turn into a token that answers "who", be it an id with a generation, a primary key, or a handle with a check. The tower from the opening of the chapter, had it held such a token, would simply have failed to find its target and taken the next one, instead of diligently shooting at the old address.

The distinction between "where it lives" and "who it is" runs through the middle of this whole construction, and from it grow dangling pointers, generations in ECS, and the whole further conversation about ownership and lifetime.
