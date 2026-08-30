# Generalizations

Imagine for a moment that behind the odd keywords and linker errors, `C++` is yet another way to talk about the world around us, about people, numbers, colors, events, and cats. We are used to thinking of programming as something purely technical, where you memorize syntax, place keywords, and "guess" what the compiler wants right now. So when you first ask "what does a program actually operate on?", you almost always feel a slight confusion, because behind all those `int`, `struct`, and `template` you suddenly find fairly simple things like items, their properties, groups of similar items, and a rule by which some items turn into others. Confusion here gets more and more expensive over time.

If you have ever built weapons in a game, you may recognize this example: first comes a base class `Weapon`, because a weapon is a kind of item, then an automatic rifle, a shotgun, and a sniper rifle inherit from it, which looks logical and even elegant. Then designers bring a shield that takes the same slot as the rifle but does not shoot, then a grenade that fires once and disappears, then a sapper shovel that does not shoot at all but can dig, and it too takes a weapon slot. And it turns out that "shooting" is not a kind at all but one property among many, that is, a genus, and it never fit the hierarchy from the start. By then half the subsystems depend on `Weapon`, the rework costs weeks, and the mistake was made in the first half hour of work, when a kind was confused with a genus.

```text
False inheritance hierarchy (OOP):

             ┌──────────────┐
             │    Weapon    │  (base class: shoot(), reload(), ammo)
             └──────┬───────┘
       ┌────────────┼────────────┬────────────┐
       ▼            ▼            ▼            ▼
  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐
  │  Rifle  │  │ Shotgun │  │ Shield  │  │ Shovel  │
  │ shoot() │  │ shoot() │  │ ???     │  │ dig()   │
  └─────────┘  └─────────┘  └─────────┘  └─────────┘
                            (cannot shoot,(cannot shoot,
                             in weapon    can dig)
                             slot)

Intersecting genera (properties and capabilities):

  ┌────────────────────────────────────────────────────────┐
  │ Genus "Fits in weapon slot":                           │
  │   Rifle, Shotgun, Shield, Shovel, Grenade              │
  ├────────────────────────────┬───────────────────────────┤
  │ Genus "Shoots":            │ Genus "Blocks damage":    │
  │   Rifle, Shotgun           │   Shield                  │
  ├────────────────────────────┼───────────────────────────┤
  │ Genus "Digs":              │ Genus "Single-use":       │
  │   Shovel                   │   Grenade                 │
  └────────────────────────────┴───────────────────────────┘
```

When you try to explain what objects and types are, you have to go beyond technical language and speak of categories humanity worked with long before any programming languages existed. Words like "entity", "kind", and "genus" show up inevitably. Let us agree right away: this is our working vocabulary for one chapter; outside it nobody uses this terminology, and in books on `C++` the same things are called types, values, and concepts, and I will use those words later too, but to see where these notions come from it is easier to start with concepts that came from... surprise, philosophy.

### Entities, attributes, and kinds

Following a Platonic reading, numbers and colors are abstract entities that do not exist in space and time the way a table or a person does, because the number 13 was not born on a particular day and will not die tomorrow, blue was not invented and cannot vanish. Such entities live in the mind and in logic; you cannot drop them on your foot, but you can shoot that same foot just fine, and whether abstract entities exist at all and whether numbers and colors are built the same way we leave to philosophers. They argued about this in Plato's time, argue now, and will probably argue forever, and for us it is enough that these notions are convenient to work with.

A concrete entity, by contrast, is always tied to history. Socrates was born and died; any country as a political unit was created on a certain date, and although the country continues to exist, it is clear that someday it will be gone or change beyond recognition. Games work the same way: "Health" as a notion is not born and does not die, it simply is, but a concrete medkit created in a match appeared on a particular frame and will vanish once picked up, leaving nothing behind except plus twenty hit points on whoever took it.

```cpp
// "Health" as an abstract entity: a value kind with no birth frame
using Health = int;

// A concrete medkit: appeared on a frame and vanishes after pickup
struct Medkit {
    Health heal;
    unsigned spawn_frame;
};

void pickup_demo() {
    Health hp = 80;
    Medkit kit{20, 148392};  // stash in this match
    hp += kit.heal;          // +20 hit points
    // kit removed from the scene, only the trace in hp remains
}
```

An attribute is a correspondence between a concrete and an abstract entity that describes some property: Socrates' eye color is a concrete instance of an abstract color, and the round count in a magazine is a concrete value of an abstract natural number.

If at some moment you snapshot a concrete entity and fix the full set of its attributes here and now, it becomes clear that attributes change over time while identity remains, and the sense of "it is still the same one" lets us say a person, a country, or a program object continues to be itself even as its properties change. Where exactly the line lies beyond which an entity stops being itself, nobody has agreed; people have argued about the Ship of Theseus for two and a half thousand years, but in a program the question is closed by the author's decision, and while the object lives, it is the same object.

Once we want to group entities, we need kinds and genera, and each of those can also be abstract or concrete. The abstract kind "natural number" covers all numbers like 0, 1, 2, 3, and so on (whether zero belongs to the naturals is a disputed question best left to mathematicians); the kind "color" is all possible shades, and notice that the same word just worked for us as a separate abstract entity whose instance was Socrates' eye color, and kind and its elements share a name. You can reason about them in general without tying yourself to a concrete instance.

A concrete kind describes a set of attributes for a family of concrete entities: "human" is a concrete kind including all humans with a certain set of biological and other characteristics; "continent" is a concrete kind including Europe, Asia, America, and others, each with borders and area, and although they differ as entities, within one kind you can speak of recurring structural properties and model them in code as objects of one data type.

### Functions and genera

The notion of function carries over from mathematics almost verbatim, and a function is a rule that maps some set of abstract entities, called arguments and belonging to certain kinds, to another abstract entity belonging possibly to another kind. The successor function maps each natural number to the next one; a blending function maps two arguments of kind "color" to a third color, and if you have ever written code that takes two `Color` values and returns a new `Color`, you implemented exactly such a function, you just thought about it in other words then.

Every day in programming we live next to this definition, though not quite inside it, because a function call in `C++` applies a rule to arguments that are concrete casts of abstract entities. The result varies: a new value, a reference to an existing object, in-place change of arguments, or nothing at all (`void`).

```cpp
// Kind: natural number
using Natural = unsigned int;

// Successor function: n → n + 1
// Rule that maps each Natural to the next Natural
Natural successor(Natural n) {
    return n + 1;
}
```

If you move up one level of abstraction and speak of genera, they let you talk not only about concrete values and kinds but about large classes of notions. A genus is a way to describe a set of abstract notions similar in some respect: the genus "number" includes "natural number", "integer", and "real number", each living by its own rules, but we can reason about them as numbers in general without specifying details. The genus "binary operator" includes arithmetic operations like addition and multiplication, logical and/or, and bitwise operations, and all follow the scheme where we take two arguments and get a result, or we get nothing, which is still a binary operator.

A concrete genus describes a set of concrete kinds similar in some respects: "mammal" unites human, cat, and whale; "biped" draws a different boundary, including humans, birds, and perhaps some fictional creatures, so one and the same entity can belong to several genera at once.

A game inventory shows this even better than whales. A medkit, ammo, and a grenade are concrete entities of different kinds, because a medkit stores heal amount, ammo caliber, a grenade radius and fuse time. Meanwhile "fits in backpack" is a genus that includes all three; "consumed directly in hand" is another genus that includes medkit and grenade but not ammo, because ammo goes through a weapon; "explodes" is a third genus where only the grenade remains. Three boundaries drawn across the same set of items, and none of them is a kind, although everyone's first impulse is exactly that: make a common base class and cram everything in.

```cpp
// Kinds: each item has its own set of attributes
struct Medkit  { float weight; int heal; };
struct Ammo    { float weight; int caliber; int count; };
struct Grenade { float weight; float radius; float fuse; };

// Genera cut this set differently and match no single kind:
//   fits in backpack       Medkit, Ammo, Grenade
//   consumed in hand       Medkit, Grenade
//   explodes               Grenade
```

In the code above genera live only in comments because the language does not know about them yet, and we need a way to express a rule over kinds that the compiler understands; the first such way is a template.

```cpp
// Genus: template as a rule over kinds.
// Pair<T, U> is not yet a kind by itself; it is a rule
// that maps two kinds to a third
template<typename T, typename U>
struct Pair {
    T first;
    U second;
};

// Rule at the "fits in backpack" genus level: weight is computed
// for any kind that has weight, and you do not rewrite the function
// for every new item
template<typename Item>
float total_weight(const std::vector<Item>& items) {
    float sum = 0;
    for (const Item& item : items) {
        sum += item.weight;
    }
    return sum;
}
```

Socrates can be viewed simultaneously as an instance of kind "human" and as a member of genera "mammal" and "biped", and any entity belongs to one kind that defines the rules of its construction or existence, but can belong to many genera, each describing only one aspect of its properties.

In programming an object has one concrete type, the one it was created with, and that type can satisfy many concepts, each fixing only a slice of operations and requirements. A caveat is inheritance: an object of a derived class is legitimately also an object of each of its bases, and if you look at it through a base reference the compiler sees one type while another lies in memory, so "one kind" here is counted by what the object was created as, and what it may be treated as when accessed is a separate question.

Separately, in OOP style, a class can inherit several interfaces and bases, which is also "many genera", but the mechanism differs, through virtual functions and hierarchy, whereas concepts check expressibility through `requires`. And `Weapon` from the start of the chapter was exactly an attempt to express a genus through a kind, to make "shooting" a base class instead of leaving it one property among many; inheritance obediently allowed that because it builds hierarchies and knows nothing about whether you divided the world into kinds correctly.

### From philosophy to types in the language

If we build a bridge from this philosophical picture to the world of `C++`, a type in the language plays the role of both a concrete kind and an abstract one, depending on the level we look at. This picture is not the only one, and later in the book types will look completely different, as byte layout in memory and as what does or does not fit in cache, and both pictures are true, they just answer different questions, one "what we model", the other "what it costs us".

From the program's point of view, a class

```cpp
struct Person {
   std::string name;
   int age;
};

Person p{"Socrates", 70};
```

defines the kind "human in our model", specifying the attributes we chose as essential, that is name and age, and an object is a concrete entity, one human in our program with its attribute values at the current moment.

From type theory inside the compiler, "type" is closer to an abstract kind, because the compiler works not with concrete `p` but with the set of all possible values of type `Person`, knows their size, structure, copy and destroy rules, and checks whether you apply functions to entities of the corresponding kinds correctly, that is, whether you pass a `std::string` object to a function expecting `double`, and vice versa. With the caveat of implicit conversions, of which the language has so many that it will pass `int` for `double` without flinching, but that is a separate topic.

### How this works inside the compiler

Inside the compiler, kind, genus, and entity differ: compiler data structures have type tree nodes and tables of information about classes, functions, and templates corresponding to abstract kinds and genera; separately there are variables, objects, and temporaries that appear and vanish during execution; and functions, entering intermediate representation (IR, intermediate representation), become rules for transforming one set of values into another.

In early compilers like `cfront` there was no separate intermediate representation at all, and the output was C text, and almost everything the translator knew about kinds and genera died before that output, while the notions of kind and genus existed only in the author's head and design. Today it is the opposite, and knowledge of kinds lives in the compiler from parsing through code generation, changing shape at each floor.

LLVM has a strictly formalized type system in IR, and each type has its own rules of representation, conversion, and allowed operations. For the optimizer, `i32` is a kind of values with fixed width and a fixed set of applicable instructions (`add`, `icmp`, and so on) that it relies on in transformations.

The levels are easy to confuse although they differ, because `C++` rules (nominal classes, access, overloads, ODR) are checked by the frontend (Clang and others) before LLVM; in IR types are already a different model convenient for analysis and code generation, with its own equivalence rules, and in places it is noticeably coarser than the source, because pointers in modern LLVM are opaque, just `ptr`, with no type of what they point to, because the optimizer gets that information separately and in another form.

If two names in `C++` denote the same type, lowering to IR is usually consistent too; separately the compiler may attach metadata like TBAA to optimize memory access, but that is not a "second `C++` type system inside LLVM" that catches errors the language missed. Still, across the chain from AST to machine code it is convenient to preserve representation invariants like size, alignment, and allowed casts at the IR level.

MSVC took a different path and grew its own internal representation together with the platform and its memory model, while LLVM built a universal optimizer language, and MSVC's type system was historically more pragmatic and less abstract but tightly fused with debug information in PDB and everything built around it. The debugger showed not just "type T" but an expanded template with substituted parameters, and that information traveled through compile and link straight to the breakpoint.

DWARF can record template parameters too, so the issue was not capability itself; one company owned the whole path from compiler to debugger, and in practice it worked out of the box, whereas in the GCC world you had to assemble tools piece by piece. MSVC was criticized most for unreadable template errors, and it got two-phase name lookup later than others, so the credit here is for matched tools, and it has nothing to do with language strictness.

### Concepts and modules

With `C++20` and concepts the situation changed, and checking compatibility "like an ordinary signature" is not enough for the compiler; it must ensure that required operations and related types are expressible for the type (there are `begin`/`end`, there is `value_type`, the needed `requires` holds, and so on). What it checks here is whether the needed expressions compile, and whether they do the right things the language does not ask, because associativity of addition, correctness of comparison, or container invariants it neither derives nor proves; the programmer still holds those.

Nevertheless GCC, Clang, and MSVC added separate layers of constraint checking and diagnostics, so the type system can compare not only signature shapes but sets of requirements on type operations; we are still far from a "theorem prover", but readable errors like "does not satisfy `Sortable`" are already here, at least until you dig into the machinery that handles all of this, where half-screen error dumps are still with us.

Modules forced compilers to rethink how to store and pass type information between translation units. The old header model assumed the compiler reparses type declarations and rebuilds internal representation from scratch each time, while modules require serializing an already built type tree into a binary format for fast loading when compiling a dependent module.

It turned out everyone stores this information differently. Clang emits `.pcm`, a serialized AST, while MSVC writes `.ifc` on top of its own language representation, which has a separately published format spec. GCC puts its `.gcm` files in `gcm.cache` in a third form unlike the others. None of the three reads another's format, and the standard did not require it; it deliberately left the format to the implementation.

In practice modules became "a standard language feature with vendor-specific details", and this divergence shows how deeply the type system grows into compiler architecture: standardizing the language turned out much easier than standardizing how to store it. Fairness requires saying the process moves, and build systems finally learned about modules, `import std;` exists in `C++23`, so the story is not finished. We will talk more about modules in later chapters.

### What we will call generalization

Ask yourself about any property. If an entity without it stops being itself, that is part of its kind; if the property describes only what you can do with the entity, that is a genus. A medkit without heal amount is not a medkit, so healing sits in its kind. "Fits in backpack" a medkit can lose if tomorrow the designer decides it is placed on the ground, and it remains a medkit, so that is a genus. `Weapon` from the start of the chapter fails this question, and a rifle without "shoots" is indeed no longer a rifle, but a shield without "shoots" is still a shield, and if the same property is part of the kind for some entities and not for others, you cannot draw a hierarchy boundary along it, no matter how convenient that seemed.

In the rest of the book I will call generalization the case where code is written not for a concrete entity and not even for one kind, but for everything that obeys the needed rules, as `total_weight` did in the example above, and that is why we needed this whole vocabulary. Next we will take it apart piece by piece: first values and how a value differs from an object, then objects with their lifetime, then procedures as rules over them, then the computational basis, the minimum from which everything else is built, and finally the set of properties that make a type convenient to work with at all. How the language lets you express all of this we will approach later, and there the talk will turn to new features, concepts, and new standards.
