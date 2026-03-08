# Generalizations, Values, and Objects in C++

## Generalizations

Imagine for a moment that C++ is not a set of odd keywords and linker errors, but simply another way to talk about the world around us: people, numbers, colors, events, and cats. We are used to thinking of programming as something purely technical, where it matters to memorize syntax, place semicolons, and "guess" what the compiler wants right now.

But if we ask ourselves "what does a program actually operate on?", it suddenly turns out that behind all those `int`, `struct`, and `template` there are rather simple and clear ideas: things, their properties, groups of similar things, and rules by which some things turn into others.

And when trying to explain what objects, types, and other fundamental concepts of computer science are, one inevitably has to go beyond purely technical language and speak of more general categories of ideas that humanity has been working with for thousands of years—and it is here that the words "entity", "kind", and "genus" become useful.

### Abstract and Concrete Entities

When philosophers and logicians speak of abstract entities, they mean individual things that do not exist in space and time the way a table, a person, or a computer do, but as something unchanging: for example, the number 13 or the color blue as such were not born at some moment and do not "die" after some time; they are not objects of the physical world but ideas we work with in our heads and in mathematics.

A concrete entity, by contrast, is always tied to history, to a moment of appearance and a moment of disappearance: Socrates was once born and once died; any country as a political entity was created on a certain date, and although the country continues to exist, it is clear that at some point it will no longer exist or may change radically.

### Attributes and Identity

At the same time, we can speak not only of entities themselves but also of their attributes—the correspondence between a concrete entity and an abstract entity that describes some property: Socrates' eye color can be described as a concrete instance of an abstract color; the number of German states as a concrete value of the abstract natural number.

If at some moment we take a "snapshot" of a concrete entity and fix the full set of its attributes here and now, we see that over time individual attributes may change while identity remains, and it is identity—this very primitive but deep sense of "it is still the same one"—that lets us say that a person, a country, or a program object continues to be itself even as its properties change.

### Kinds and Genera

Next arises the need to group entities in some way, and here the notions of kind and genus enter the stage; each of these categories can itself be abstract or concrete. When we speak of an abstract kind, we describe the common properties of a whole family of abstract entities.

Thus the kind "natural number" covers all individual numbers 0, 1, 2, 3, and so on (zero may or may not be included in the naturals and may be treated as the empty set); the kind "color" covers all possible shades from dark blue to bright orange, and we can reason about them as something general without tying ourselves to a concrete instance.

A concrete kind, by contrast, describes a set of attributes for a family of concrete entities: when we say "human", we mean a concrete kind that includes all humans with a certain set of characteristics such as biology, consciousness, etc.; when we say "continent", we describe a concrete kind that includes Europe, America, or Asia, each with boundaries, population, area, and other attributes. They differ as entities, but within the kind "human" or "continent" we can speak of recurring structural properties and model them in code as objects of one data type.

### Functions as Rules

An important role in this picture is played by the notion of function, and here the mathematical definition carries over into programming almost verbatim: a function is a rule that maps some set of abstract entities, called arguments and belonging to certain kinds, to another abstract entity, called the result, belonging possibly to another kind.

A classic example from mathematics is the successor function, which maps each natural number to the next one—the function "n → n + 1" in terms of the natural-number kind; another, more figurative example is a color-blending function that maps two arguments of kind "color" to a third color as the result of mixing them—and if you have ever written code that takes two `Color` values and returns a new `Color`, you were in effect implementing such a function.

In programming we live inside this definition: any function call in C++ is the application of a rule to arguments that are concrete "casts" of abstract entities, and the result is a new object that can also be viewed as a concrete embodiment of an abstract entity of a given kind.

```cpp
// Kind: natural number
using Natural = unsigned int;

// Successor function: n → n + 1
// Rule that maps each Natural to the next Natural
Natural successor(Natural n) {
    return n + 1;
}
```

### Genera as Generalizations of Kinds

If we move up one level of abstraction and speak of genera, they let us talk not only about concrete values or even kinds, but about large classes of notions.

Abstractly, a genus is a way to describe a set of different abstract kinds that are similar in some respect: for example, the genus "number" includes kinds such as "natural number", "integer", "real number", each with its own rules of existence, but we can reason about them as number in general without specifying the kind; the genus "binary operator" includes both arithmetic operations (addition, multiplication) and logical operations (and, or) and bitwise operations, but all of them follow the general scheme "take two arguments and produce a result".

A concrete genus, in turn, describes a set of different concrete kinds similar in some respects: "mammal" describes different concrete kinds such as human, cat, and whale; "biped" describes another classification that includes humans, birds, and perhaps some fictional creatures—and one and the same entity (e.g. Socrates) can be viewed simultaneously as an instance of the kind "human" and as a member of the genera "mammal" and "biped". It is important to understand that any entity belongs to exactly one kind, which defines the rules of its construction or existence, but can belong to many genera, each of which describes only one aspect of its properties—and in programming it is the same: an object of one concrete type can implement many interfaces or concepts, each emphasizing only part of its behavior.

```cpp
// Genus: template—a rule over kinds
// This is the "genus" level: Pair<T, U> is not a concrete kind
// but a rule that maps two kinds to a new kind
template<typename T, typename U>
struct Pair {
    T first;
    U second;
};

// Function at the genus level: for any kind T
// returns a pair of two elements of that kind
template<typename T>
Pair<T, T> make_pair_of(T a, T b) {
    return {a, b};
}
```

### Types in C++ as Kinds

If we build a bridge from this philosophical picture of the world to our world of compilers and C++, we can see that a type in the language plays the role of both a concrete kind and an abstract kind, depending on the level we look at.

From the program's point of view, a class:

```cpp
struct Person {
   std::string name;
   int age;
};
```

defines the kind "person in our model", specifying the set of attributes we chose as essential: name and age; and an object:

```cpp
Person p{"Socrat", 70};
```

is a concrete entity—one person in our program with its attribute values at the current moment in time.

From the point of view of type theory in the compiler, "type" is closer to an abstract kind: the compiler works not with a concrete `p` but with the set of all possible values of type `Person`; it knows their size, layout, copy and destruction rules, and checks that you apply functions (in our sense—rules) correctly to entities of the corresponding kinds—i.e. that you do not pass a `std::string` to a function expecting `double`, and vice versa.

### Evolution of Type Representation in Compilers

Historically, different compilers implemented this model differently at the level of internal program representation, but the conceptual picture was always similar: in the compiler's internal data structures there are entities corresponding to abstract kinds and genera—nodes of the type tree, tables of information about classes, functions, and templates; there are entities corresponding to concrete entities—variables, objects, temporaries that appear and disappear during program execution; and there are functions that, in the form of intermediate representation (IR), become a set of rules for transforming one set of values into another.

In early compilers like cfront, many of these things were encoded in the textual C code produced by the translator, and the notions of kind and genus existed only in the author's head and in the design. As GCC, Clang, and MSVC evolved, increasingly sophisticated type systems and checking emerged that formalize these categories and allow, for example, at the LLVM IR level, to speak of a type as a well-defined "kind" of values for which equivalence, compatibility, and validity of operations can be checked.

```
┌───────────────────────────────────────────────────────────────────────┐
│  cfront (1983-1993) - Translation to C                                │
├───────────────────────────────────────────────────────────────────────┤
│  C++ code:                         cfront output (C code):             │
│  ┌─────────────────────┐          ┌──────────────────────────┐        │
│  │ struct Person {     │          │ struct Person {          │        │
│  │   string name;      │   →      │   struct string name;    │        │
│  │   int age;          │          │   int age;               │        │
│  │                     │          │ };                       │        │
│  │   void greet();     │          │                          │        │
│  │ };                  │          │ void Person_greet(       │        │
│  │                     │          │   struct Person* this    │        │
│  │ Person p;           │          │ ) { ... }                │        │
│  │ p.greet();          │          │                          │        │
│  └─────────────────────┘          │ struct Person p;         │        │
│                                   │ Person_greet(&p);        │        │
│                                   └──────────────────────────┘        │
│  Type information:                                                    │
│  ┌──────────────────────────────────────────────────────────┐         │
│  │ • Exists only in the programmer's head and in            │         │
│  │   code comments                                          │         │
│  │ • C compiler sees only struct Person                     │         │
│  │ • Methods → plain functions with this*                  │         │
│  │ • Type checking is minimal (only at C level)            │         │
│  └──────────────────────────────────────────────────────────┘         │
└───────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────────────┐
│  GCC/Clang (2000+) - Formal type system                                │
├───────────────────────────────────────────────────────────────────────┤
│  C++ code → AST → Type System → LLVM IR → Machine Code                │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐         │
│  │  C++ Source  │  →   │     AST      │  →   │      IR      │         │
│  ├──────────────┤      ├──────────────┤      ├──────────────┤         │
│  │ struct Person│      │ RecordDecl   │      │ %Person =    │         │
│  │ {            │      │   name:      │      │   type {     │         │
│  │   string name│      │   FieldDecl  │      │     %string, │         │
│  │   int age;   │      │     Type:    │      │     i32      │         │
│  │ };           │      │     string   │      │   }          │         │
│  │              │      │   age:       │      │              │         │
│  │ Person p;    │      │   FieldDecl  │      │ %p = alloca  │         │
│  │ p.greet();   │      │     Type: int│      │   %Person    │         │
│  └──────────────┘      └──────────────┘      └──────────────┘         │
│  Type information is stored in compiler data structures:               │
│  ┌────────────────────────────────────────────────────────────┐       │
│  │  Type Database:                                            │       │
│  │  ┌──────────────────────────────────────────────────────┐  │       │
│  │  │ Type: Person                                         │  │       │
│  │  │ ├─ Size: 32 bytes                                    │  │       │
│  │  │ ├─ Alignment: 8                                      │  │       │
│  │  │ ├─ Members:                                          │  │       │
│  │  │ │  ├─ name: string at offset 0                       │  │       │
│  │  │ │  └─ age: int32 at offset 24                        │  │       │
│  │  │ ├─ Methods:                                          │  │       │
│  │  │ │  └─ greet(): void                                  │  │       │
│  │  │ ├─ Copy Constructor: auto-generated                  │  │       │
│  │  │ ├─ Destructor: calls ~string()                       │  │       │
│  │  │ └─ Move semantics: auto-generated                    │  │       │
│  │  └──────────────────────────────────────────────────────┘  │       │
│  │                                                            │       │
│  │  Checks:                                                   │       │
│  │  • greet(Person) ← alice:Person  OK                        │       │
│  │  • greet(Person) ← x:double      ERROR                     │       │
│  │  • person.name ← "text"          OK (string = char*)       │       │
│  │  • person.age ← "text"           ERROR (int ≠ char*)       │       │
│  └────────────────────────────────────────────────────────────┘       │
└───────────────────────────────────────────────────────────────────────┘
```

## Values

When you run a program and look at memory from the processor's point of view, there is no C++, no types, and not even numbers—only long sequences of zeros and ones. The processor does not know what "the integer 42", "the float 3.14", or "the character 'A'" are: to it, all of these are just different ways to interpret the same bits.

As long as we have no interpretation of this data, we see only zeros and ones as a bit sequence; everything else is the result of an agreement between the programmer, the compiler, and the architecture on how to interpret these bits.

### Value Type as a Bridge

A value type in this context can be thought of as a bridge between the abstract world of mathematical entities and the very concrete world of bit sequences: on one side we have a kind of abstract things (e.g. integers or rational numbers), on the other we have the set of all possible bit strings of the required length, and the value type says which bit strings we consider valid representations of entities of that kind and which we do not.

If we have a concrete entity, say the integer "minus five", the set of bits with which we encode it in memory is called the representation of that entity, and the entity itself—the number as a mathematical object—is the interpretation of that data. When we say "value", we mean the data together with its interpretation: not just "32 bits 1111…", but "this is a 32-bit signed integer in two's complement with value -5" or "this is a pair of 32-bit integers interpreted as numerator and denominator of a rational number".

### Correctness of Representation

For example, a plain `int` on many platforms is 32 bits in two's complement, where the bit pattern 0x00000001 is interpreted as 1 and 0xFFFFFFFF as -1. A rational number can be represented as the concatenation of two such 32-bit integers, but it is important to understand that not every bit sequence is a meaningful representation for an arbitrary value type. We can say that data is well-formed with respect to a type if it actually represents some abstract entity of that kind.

Any 32-bit sequence, if we agree to interpret it as a two's complement integer, will be well-formed: every bit pattern corresponds to some integer from the minimum to the maximum. But if we take the type "real number in the sense of mathematical representation" and try to interpret an arbitrary IEEE 754 value, then when we encounter NaN we run into bits that by the IEEE 754 standard mean "Not a Number", and from the point of view of pure mathematics they are not a well-formed representation of a real number, although as an element of the type "machine float" they are perfectly valid.

That is why the C++ standard distinguishes the "NaN bit pattern" and the "value of type double" as abstract entities; the compiler and the processor can physically store and pass NaN without any problem, but mathematical functions may be undefined for NaN.

```cpp
// Each of these bit patterns is meaningful as int
uint32_t bits1 = 0x00000001u;
uint32_t bits2 = 0xFFFFFFFFu;

int32_t a, b;
std::memcpy(&a, &bits1, 4); // 1
std::memcpy(&b, &bits2, 4); // -1  (two's complement)
```

### Completeness and Partiality of Types

Next arises the question: how completely does our value type cover the corresponding abstract kind? If the type represents only part of the possible entities, we call it "properly partial"; if every abstract entity of that kind can be expressed as a value of that type, then the type is complete.

A good example of a properly partial type is the ordinary `int`, which represents only a finite range of integers although the set of mathematical integers is infinite; the type `bool`, by contrast, is complete for the kind of Boolean values, because the abstract "true" and "false" map uniquely to the two possible values `true` and `false`.

Historically, compilers have limited the range of `int` in different ways: on 16-bit systems it was on the order of -32768…32767, on 32-bit and 64-bit systems much larger, but in all cases `int` remained a partial representation of the abstract kind "integer", and programmers had to remember overflow and that not every mathematical expression has a valid representation in this type.

```cpp
y = nan
y + 1  = nan
y * 0  = nan
y == y = 0
sqrt(-1) = nan
```

### Uniqueness of Representation

A separate and very important topic in practice is uniqueness of representation—the question of whether each abstract entity corresponds to at most one value of the given type. If we have a Boolean type implemented as a byte where zero is false and any non-zero value is treated as true, then the representation is not unique: 0x01, 0xFF, and 0x7F are all interpreted as true, and equality in meaning does not coincide with equality of bit content.

A type representing an integer as "sign bit plus magnitude" also does not have a unique representation of zero, because both "+0" and "-0" are different bit patterns with the same interpretation.

The two's complement format, by contrast, has unique representation: every integer in the valid range is encoded by exactly one bit pattern, and zero has no "+0"/"-0" variant. Compilers and processors like such types with unique representation because the equality operation is trivially implemented as bitwise comparison, and the optimizer can apply many techniques based on the fact that replacing equal with equal preserves both bits and meaning.

### Ambiguity of Representation

But the opposite can also happen: sometimes we deliberately choose a representation without uniqueness or even with ambiguity, where the same value can have more than one interpretation—i.e. the bits alone cannot uniquely recover the abstract entity without additional context.

A favorite example is a calendar year encoded by two decimal digits: "42" can mean 1942, 2042, or some other year depending on the century; here the same data sequence admits several interpretations, and the type itself is ambiguous. In everyday work this shows up when you store dates without an explicit timezone—bits take less space, computations are faster, but bit equality no longer guarantees equality in meaning, and conversely, entities equal in meaning can have different representations.

### Two Kinds of Equality

To distinguish these situations cleanly, it is useful to distinguish two kinds of equality: equality as coincidence of interpretations, and "representational" equality as literal coincidence of bit sequences. Two values of the same type are equal if they describe the same abstract entity; they are representationally equal if the sequences of zeros and ones in their memory are identical.

If a type has unique representation, then equality in meaning automatically implies equality in bits, because for each entity there is only one valid representation. If the type is not ambiguous—i.e. each bit sequence corresponds to at most one entity—then conversely, representational equality implies equality in meaning: same bits, same entity.

That is why for types like ordinary `int` in two's complement or `double` (excluding NaN), compilers readily implement the `==` operator as either direct bit comparison or one or two machine comparison instructions, and the optimizer can replace comparison with cheaper checks without fear of breaking semantics.

### Trade-offs in Representation

But in real life we often encounter types where uniqueness of representation is deliberately sacrificed for efficiency of value creation and transformation. Rational numbers stored as pairs of numerator and denominator integers without automatic reduction are a classic example: 1/2 and 2/4 represent the same abstract entity but have different representational forms, and to check their equality in meaning we must either reduce both fractions to lowest terms or multiply and compare products—much more expensive than a simple comparison of two bit patterns.

```cpp
// Complete type for a bounded kind
// If the kind itself is finite—the type can be complete.
// Card suit: exactly 4 entities, exactly 4 enum values.

enum class Suit { Clubs, Diamonds, Hearts, Spades };

// Non-unique representation
struct Rational {
    int64_t num; // numerator
    int64_t den; // denominator (> 0)

    bool naive_eq(...) // fast naive implementation
    bool normalized_eq(...) // slow check with normalization
    bool cross_eq(...) // slow check with cross-multiplication
};

auto a = Rational::make(1, 2); // 1/2
auto b = Rational::make(2, 4); // 2/4 -- same abstract entity

cout<<"1/2 naive_eq 2/4:"<<a.naive_eq(b)<<"\n"; // 0 -- wrong!
cout<<"1/2 normalized_eq 2/4:"<<a.normalized_eq(b)<<"\n"; // 1
cout<<"1/2 cross_eq 2/4:"<<a.cross_eq(b)<< "\n"; // 1
```

Similarly, finite sets can be stored as unsorted lists of elements, where checking set equality requires sorting and removing duplicates before comparing element by element. In such situations the representation designer deliberately trades cheaper creation and modification of values for more expensive equality checks, if the latter is considered a rare operation. Historically, many standard libraries show traces of such trade-offs: from old implementations of `std::set` and `std::multiset`, where value comparison was cheap but insertion more expensive, to experimental structures like hash sets where insertion and membership were optimized at the cost of a more complex equality and hashing scheme.

```cpp
// Representation 1: Unsorted list
// Fast insert O(1), slow comparison O(n log n)
struct UnsortedSet {
    std::vector<int> elements;

    // Insert - O(1), just append
    void insert(int x) {
        elements.push_back(x);
    }

    // Equality - O(n log n), must sort
    bool operator==(const UnsortedSet& other) const {
        // Copy and sort for comparison
        auto a = elements;
        auto b = other.elements;
        
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        
        // Remove duplicates
        a.erase(std::unique(a.begin(), a.end()), a.end());
        b.erase(std::unique(b.begin(), b.end()), b.end());
        
        return a == b;
    }
};

// Representation 2: Sorted list (no duplicates)
// Slow insert O(n), fast comparison O(n)
struct SortedSet {
    std::vector<int> elements;  // Always sorted!
    
    // Insert - O(n), must maintain order
    void insert(int x) {
        auto it = std::lower_bound(elements.begin(), elements.end(), x);
        if (it == elements.end() || *it != x) {
            elements.insert(it, x);  // O(n) due to element shift
        }
    }
    
    // Equality - O(n), just compare element by element
    bool operator==(const SortedSet& other) const {
        return elements == other.elements;  // Linear comparison
    }
};
```

### Undecidability of Behavioral Equality

Sometimes implementing "true" behavioral equality is too expensive or even theoretically impossible: if you represent a computable function as a piece of code or a data structure describing an algorithm, then the notion "two functions are equal if they give the same result on all possible arguments" turns out to be undecidable.

In general it is impossible to check algorithmically that two arbitrary algorithms behave the same on the entire domain, and in practice one has to settle for a weaker notion of equality—comparison of representations, where two values are considered equal only if their bit patterns coincide, which corresponds to them being the same object in memory or an exact copy. Returning to functions: computers implement mathematical functions over abstract entities through concrete procedures over values—i.e. over bits plus their interpretation.

### Address Independence

Although values live in memory and have addresses, a correctly implemented function should behave as if it does not care at which addresses the arguments lie and is interested only in their content as values; if we copy a value to another place, the result of applying the function to the old and new copy should be the same.

When we say that a function is defined on a value type and is regular, we mean that it "respects" equality: if we replace an argument with any other value equal to it, the result of the function does not change. Most familiar numeric functions such as sin, cos, +, or *, when implemented carefully and with a fixed rounding mode, behave this way: if two `double` values really represent the same real number, then the result of addition or multiplication with any third arguments will be equal in meaning.

From the point of view of type theory and the compiler optimizer this is a very important remark, because such functions allow substitution—replacing expressions with equal ones without changing program behavior—whereas non-regular functions do not, because inside them the choice of result may depend on the exact bit pattern of the value.

```cpp
double pos_zero =  0.0;
double neg_zero = -0.0;

cout<<"\n+0.0 == -0.0:"<<(pos_zero == neg_zero)<<"\n"; 
// 1 -- equal by ==

cout<<"signbit(+0.0):"<<std::signbit(pos_zero)<<"\n";
// 0

cout<<"signbit(-0.0):"<<std::signbit(neg_zero)<<"\n";
// 1 -- different result!
// signbit violates regularity:
// replaced equal with equal, but result changed
```

### Floating-Point Optimization Issues

In the history of C++ compilers this distinction has repeatedly surfaced in discussions of allowed optimizations: can we, for example, reorder floating-point computations or replace expressions with mathematically equivalent ones, when the IEEE 754 format and the presence of NaN, infinities, and rounding subtleties make many familiar identities—such as associativity of addition—false.

```cpp
float a = 1e15, b = -1e15, c = 1.5;

float left  = (a + b) + c; // (1e15 - 1e15) + 1.5 = 0 + 1.5 = 1.5
float right = a + (b + c); // 1e15 + (-1e15 + 1.5) -- loss of precision

std::cout << "\n(a+b)+c = " << left  << "\n"; // 1.5
std::cout << "a+(b+c) = " << right << "\n"; // may differ!
std::cout << "Equal:   " << (left == right) << "\n";
// With -ffast-math compiler could reorder and get a different result

// Output:
// (a+b)+c = 1.5
// a+(b+c) = 0
// Equal:   0
```

The standard committee and compiler developers sought a balance between the desire to optimize aggressively and the need to preserve correctness for those functions and types that the programmer considers regular, and as a result modes like `-ffast-math` in GCC and Clang appeared, which deliberately relax the regularity guarantee for the sake of performance.

In the end, when designing the representation of any value type, two tasks inevitably go hand in hand: you must decide what equality you want—bitwise or behavioral, with unique representation or without—and which functions over this type you want to keep regular, so that the compiler and you yourself can safely apply the rule "substitute equal for equal". In C++ this shows up in many places, from the choice of number and string formats to the design of wrapper classes and containers, and as compilers evolved from simple single-pass translators to modern multi-stage optimizers, they rely more and more on these type properties to generate fast and correct code.

## Objects

When we say the word "object" in programming, it is very easy to jump straight to classes, methods, and all sorts of OOP trappings, although in reality it all starts much lower—with memory itself and how bits live in it. Imagine memory as a large field of cells, where each cell has an address and contents: the address and the contents (data length) are also numbers, and to read a word at an address the processor performs a load operation, and to change the association between address and contents it does a store. On modern machines this role is played by regions in RAM, and at the level of disk storage the same model is implemented by blocks (logical storage regions) on SSD or HDD, only with different latencies and rules—but the principle is the same: there is an address, there are contents, and there are read and write operations.

### Object as a Contract

Against this background, an object can be understood as a contract between the programmer, the compiler, and the machine: a way to represent a concrete entity from our domain as a value living in memory. Each object has state, and this state is simply a value of some value type. If an object describes, say, an employee, then its state at the current moment is a "snapshot" of all the attributes we decided to store: name, salary, department id.

This state can change over time, and that is how an object differs from a plain value—because a value as such, as a mathematical representation, is immutable, while an object can have different values at different moments. To hold its state, an object owns a set of resources—words of memory on the heap, file records, database rows; in the simplest case it is just a few consecutive bytes in dynamic memory.

### Array of Structures vs Structure of Arrays

```cpp
// ============================================
// Variant 1: Array of Structures (AoS)
// Classic representation - object data laid out together
// ============================================
struct Complex_AoS {
    double real;
    double imag;
};

void example_aos() {
    std::vector<Complex_AoS> numbers = {
        {1.0, 2.0},  // first complex number
        {3.0, 4.0},  // second
        {5.0, 6.0}   // third
    };
    
    // Access is natural - everything is adjacent
    std::cout << "AoS: " << numbers[1].real << " + " 
              << numbers[1].imag << "i\n";
}
```

```
===================================================================
ARRAY OF STRUCTURES (AoS) - classic representation
===================================================================

Memory (bytes laid out consecutively):

  ┌─────────────────┬─────────────────┬─────────────────┐
  │  Complex[0]     │  Complex[1]     │  Complex[2]     │
  ├────────┬────────┼────────┬────────┼────────┬────────┤
  │ real:  │ imag:  │ real:  │ imag:  │ real:  │ imag:  │
  │  1.0   │  2.0   │  3.0   │  4.0   │  5.0   │  6.0   │
  └────────┴────────┴────────┴────────┴────────┴────────┘
  
  0x1000          0x1010          0x1020          0x1030
```

At the same time we must understand that although we logically think of an object's value as a contiguous sequence of zeros and ones, the physical resources where these bits are stored need not be contiguous: a classic example is a complex number as a pair of doubles (real and imaginary part), which may lie next to each other in memory or may be split across different structures or arrays—and the interpretation still assembles them into one logical whole. If we manually switched from an array of structures {re, im}[] to two separate arrays re[] and im[], then the fields of one logical object physically live in different regions of memory, being one object only by convention of interpretation.

```cpp
// ============================================
// Variant 2: Structure of Arrays (SoA)
// Data of one logical object spread across memory
// ============================================
struct ComplexArray_SoA {
    std::vector<double> real;  // all real parts consecutively
    std::vector<double> imag;  // all imaginary parts consecutively
    
    size_t size() const { return real.size(); }
    
    // Logical "object" at index i is the pair (real[i], imag[i])
    // but physically they live in different arrays
    struct ComplexRef {
        double& re;
        double& im;
        
        ComplexRef(double& r, double& i) : re(r), im(i) {}
        
        // Behaves like an object, but data is not adjacent!
        void print() const {
            std::cout << re << " + " << im << "i\n";
        }
    };
    
    ComplexRef operator[](size_t i) {
        return ComplexRef(real[i], imag[i]);
    }
};

void example_soa() {
    ComplexArray_SoA numbers;
    numbers.real = {1.0, 3.0, 5.0};
    numbers.imag = {2.0, 4.0, 6.0};
    
    // Access "object" at index 1
    // Physically fields in different places, logically one object
    std::cout << "SoA: ";
    numbers[1].print();
}
```

```
===================================================================
STRUCTURE OF ARRAYS (SoA) - split representation
===================================================================

Memory (arrays in different places):

  Array real[] (all real parts):
  ┌────────┬────────┬────────┐
  │  1.0   │  3.0   │  5.0   │  ← real[0], real[1], real[2]
  └────────┴────────┴────────┘
  0x2000  0x2008  0x2010
  
  
  Array imag[] (all imaginary parts):
  ┌────────┬────────┬────────┐
  │  2.0   │  4.0   │  6.0   │  ← imag[0], imag[1], imag[2]
  └────────┴────────┴────────┘
  0x3000  0x3008  0x3010

Logical object Complex[1] = {real[1], imag[1]}
                              = {3.0 from 0x2008, 4.0 from 0x3008}
```

In distributed systems, the resources of one logical object may end up in different kinds of memory—part in RAM, part on disk or in remote storage—but usually when we describe an object we assume one process, one address space, a unique starting address, and fixed offsets to all its parts.

### Object Type

An object type in this picture is the blueprint for how we store and change values in memory; we can say that for each object type there is a corresponding value type that describes all valid states of objects of that type.

If we agreed that our object is a 32-bit signed integer in two's complement format with little-endian byte order and 4-byte alignment, we have thereby defined a concrete object type: the compiler knows its size in bytes, where it can be placed in memory, which processor instructions to use for load and store, and the value type for this object is the set of all integers in the valid range.

Any concrete "int" object in the program belongs to this object type: its state is some concrete value, and its physical implementation is a set of bytes in memory that the compiler accesses at a known offset from the starting address. An important point to grasp when first writing code is that values and objects are complementary but play fundamentally different roles.

### Values vs Objects

Values as mathematical entities are immutable and do not depend on how we encoded them in memory; you can write the number 42 on paper, say it aloud, send it over the network as text or in binary format—the value itself does not change.

Objects, by contrast, are tied to a concrete machine and implementation: they have an address, a set of bytes, mechanics of mutation, and they can transition from one state to another during the program's lifetime.

Nevertheless, the state of any object at a given moment can always be described by a value: you can take an `std::string` object, look at its logical value as a sequence of characters, write it on paper or serialize to JSON—that will be a "snapshot" of the object's state.

When we discuss equality of objects, we usually mean equality of their states as values, abstracting from the fact that one `std::string` stores its data in one region of memory and another in another. This view is especially important in purely functional languages, where there are no mutable objects at all, only values and functions, but in C++ too it lets us separate "what" from "how" and not tie everything to concrete addresses.

### Why Do We Use Objects?

First, objects very naturally model mutable concrete entities of the real world: an employee record in a database, a character object in a game, a window state in a GUI—all of these are constructs that change as the program runs, and an object with mutable state describes them close to reality.

Second, even when we care only about purely abstract values—like the square root of a number or the solution of a linear system—we almost always implement these as algorithms that use mutable memory: iterative methods, accumulation of partial sums, etc., and in this sense objects are a powerful tool for implementing functions over values.

Third, at a fundamental level, the universal model of computation (Turing machine) implies a machine with memory, and a real computer is exactly such a machine, so any practical implementation of computation ultimately reduces to manipulating objects in memory, even if at the language level it is disguised as "pure functions".

### Properties of Object Types

Some properties of value types carry over to object types. If a value is well-formed—i.e. its binary representation is valid for the chosen type—then an object with that state is valid; if the value type is properly partial, representing only part of the possible abstract entities (like `int` relative to all integers), then the object type implementing this `int` will be partial; if the value type has unique representation, like two's complement for integers, then for objects of that type equality in meaning can be implemented by comparing bytes—which has historically been very important for optimization, because compilers like GCC and Clang can reduce comparison of POD-type objects to a simple `memcmp` when they know the representation is unique and there are no "holes" with uninitialized bits, or go further and replace `memcmp` with direct register comparison or vector instructions—but one should remember that a naive mapping of `operator==` → `memcmp` for structures with possible padding can lead to subtle bugs.

### The Notion of Identity

Object identity deserves separate discussion, because in the real world concrete entities have identity: Socrates remains Socrates regardless of whether he dyed his hair, changed address, or died, and a state remains the same state even if it changes its flag, constitution, or population size.

To reflect this in a program, objects that represent concrete entities need their own notion of identity, separate from current state. A convenient way to introduce such identity is an identity token—a unique value that expresses "who this is", not "what state they are in now". Such a token might be the object's address in memory, an index in the array where it is stored, or an employee ID in an HR system; by comparing identity tokens we effectively check object identity: the same object or different ones.

Over the program's lifetime a concrete object may change its identity tokens—it may be moved to another region of memory or from one container to another, or assigned a new identifier—but logical identity is preserved if we maintain a mapping between the "old" and "new" token.

### Equality vs Identity

Finally, we should draw a clear line between equality of objects and their identity. Two objects of the same type are equal if their states are equal as values: two `std::string`s both containing "hello" are equal even if they lie in different places in memory and have different internal buffers; two `std::vector<int>`s containing the same sequences are equal as sequences of values.

In that case it is natural to say that one object is a copy of the other, and any changes made to one do not affect the copy. Equality checking here rests on the value model, not on addresses; identity answers a different question—"is this the same object or just another object with the same state?".

In early C++ compilers and runtimes, the question of equality and identity was closely tied to how copying and moving were implemented, and old implementations could unexpectedly "share" internal buffers between objects—e.g. for `std::string` via copy-on-write—and one had to be careful about what "copy" really meant; but as the standard and implementations evolved, compilers and libraries became much stricter about equality by state and identity by token, which simplified reasoning about correctness.

```cpp
// GCC before C++11 with COW strings:
std::string s1 = "hello";
std::string s2 = s1;  // Buffer NOT copied!

// s1 and s2 point to ONE buffer in memory
// Internal reference count = 2
// On modification:
s2[0] = 'H';  // Only NOW is the buffer copied (Copy-On-Write)
```

COW string scheme:

```
After copy:
s1: [ptr] ──┐
            ├──→ [refcount:2][h][e][l][l][o][\0]
s2: [ptr] ──┘

After modifying s2:
s1: [ptr] ────→ [refcount:1][h][e][l][l][o][\0]

s2: [ptr] ────→ [refcount:1][H][e][l][l][o][\0]
                (new buffer)
```

### Evolution of Object Implementations

And different compilers implemented these ideas differently: in early cfront an object was effectively a C structure whose address played the role of identity token, and the distinction between value and object largely existed only in comments and documentation. In Borland C++ and early MSVC, object implementation was tightly bound to the memory model of the specific platform: alignment, byte order, structure layout conventions—and programmers had to explicitly account for how their "abstract" objects would live in concrete x86 memory.

```cpp
// C++ code:
class Point {
    int x, y;
public:
    void move(int dx, int dy);
};

// cfront translated roughly to:
struct Point {
    int x, y;
};
void Point_move(struct Point *this, int dx, int dy);

// Address = identity
Point p1, p2;
// &p1 and &p2 are identity tokens
```

Modern compilers like GCC, Clang, and MSVC have rich internal representations of types and values, often on top of an intermediate representation such as LLVM IR, where the idea of "object as source of mutable state" and "value as immutable entity" are clearly separated, and the optimizer can, for example, temporarily turn an object into a set of values in registers if it sees no "observers" between its updates, and then materialize it back into memory.

```cpp
// C++ code:
struct Point { int x, y; };

void foo() {
    Point p;
    p.x = 10;
    p.y = 20;
    use(p.x + p.y);
}

// Without optimizations (object in memory):
; %p = alloca Point
; store 10, p.x
; store 20, p.y  
; load p.x, load p.y

// After optimizations (object became values in registers):
; %x = 10
; %y = 20
; %sum = add %x, %y
; (no Point object in memory at all)
```

But for us it is useful to keep a simple picture in mind: memory is addresses and words, a value is the interpretation of a bit sequence, an object is a place in memory where that value lives and can change, and a type is the contract for how we store, read, modify, and compare those bits.

## Procedures

When we move from values and objects to program behavior and what makes a program a program, another fundamental category appears—procedures. In familiar C++ this is simply a function, but in a more general sense we can say: a procedure is a sequence of rules that changes the state of the objects it works with, and sometimes also creates new objects or destroys old ones.

Historically, compilers started with a very primitive notion of procedures: in early Fortran and the first versions of C, a procedure was just a named piece of code with a fixed set of arguments, and all the "magic" was to correctly generate entry and exit—prologue and epilogue—save registers, and manage the stack.

### Categories of Procedure Objects

As languages and compilers evolved, it became clear that the key question is not so much syntax as how exactly the procedure interacts with objects: which it reads, which it modifies, which live only during the call, and which outlive many calls or even the whole program. If we try to break this interaction into clear categories, we can distinguish four groups of objects that practically any procedure deals with.

First, **"input and output / in-out"**—objects that are passed into the procedure and out of it, either directly or indirectly. Directly is when you write `int f(int x)` and the compiler passes the value of x in registers or on the stack and then returns the result, say also in a register; indirectly is when you pass a pointer or reference, like `void increment(int* p)`, and the procedure works with the object not as a copy of a value but as the original in memory.

Second, **local state**: temporary objects created at the start of the procedure call, living on the stack or in registers, tailored to the algorithm's needs and destroyed when the procedure finishes. If in a function `void foo()` you declare `int tmp = 0; std::vector<int> v(10);`, then both tmp and v are part of the local state of this procedure: they are needed for its work, but nothing outside need know or should know about them.

And **global state**: objects that are accessible not only to this procedure but to others, across many calls—global variables, static fields, or objects in dynamic memory that are referenced from different parts of the program.

Finally, there is **procedure-owned state**: objects that are accessible only to the procedure itself (and perhaps closely related functions) but persist across calls. The simplest example is a static variable inside a function:

```cpp
void counter() { 
   static int n = 0;
   ++n; 
   std::cout << n;
}
```

Then n will live as long as the program, but only counter can use it; for everyone else it is as if it does not exist. This demonstrates the typical pattern of "hidden" procedure state that appeared in old C long before classes.

### Inputs, Outputs, and In-Out

To talk about procedure behavior more precisely, we need one more distinction: which objects count as its inputs, which as outputs, and which as both. If a procedure only reads an object's value and never changes it, that object is a pure input; if the procedure creates an object, writes data into it, or destroys it without looking at its previous contents, that object acts as an output and its initial state does not matter—only the new one.

The most interesting case is when an object is both read and modified: e.g. a counter passed by reference that the function increments, or an array element that is first checked against some condition and then updated. Such objects act as in-out for the procedure, and they are often the source of subtle effects and bugs if the programmer does not fully realize who in the program is allowed to change their state.

In early compilers, analysis of these categories was almost entirely the programmer's responsibility and the compiler simply accepted the code as given; but modern compiler optimizers actively analyze which variables are only read, which are written, and where side effects are possible, and on that basis decide which calls can be reordered (reordering, CSE, DCE), which transformations are allowed, and which would violate expected semantics.

## Computational Basis

Moving from procedures to the idea of a computational basis, we must understand what operations your type supports at all. For any value type we can define some minimal set of procedures from which, in principle, all other operations on it can be built; such a set is called the computational basis of the type.

For example, for unsigned k-bit integers we can take the operations "get zero", "check equality", and "go to next value", and in theory from these we can implement addition, multiplication, and comparison—just as very long sequences of "next" calls and checks.

### Efficiency of the Basis

But here the notion of efficiency comes in: a basis is considered efficient if any procedure built on it can be implemented no worse than on any other reasonable basis. In our example with the "next" operation things are bad: adding two k-bit numbers by repeatedly incrementing one of them requires on the order of 2^k steps in the worst case—i.e. will be inherently slow—whereas implementing addition at the level of machine instructions takes a fixed number of cycles independent of the argument values.

Historically, looking at the evolution of processor architectures and compilers, we see how the set of basic operations was gradually expanded precisely for efficiency: from early machines with a very limited instruction set, where complex operations had to be emulated by long sequences of elementary steps, to modern architectures with a rich set of arithmetic and logic instructions, for which compilers can generate specialized and vectorized code.

### Expressiveness of the Basis

But efficiency alone is not enough; a second important characteristic of a basis is its expressiveness. A basis is called expressive if with it we can define procedures over the type conveniently and compactly, especially those that occur regularly in practice.

We could say that subtracting two arbitrary numbers is redundant—it can be implemented via addition with negation, and negation via subtraction from zero—but if you try to write real code with only "+", "0", and "next", you will find that many definitions become bulky and unreadable. So when we design an "expressive" basis we add operations that are logically derivable from others but whose practical benefit from being explicit is much greater than the cost of enlarging the basis.

```cpp
// ============================================
// MINIMAL BASIS (theoretically sufficient)
// ============================================
namespace MinimalBase {
    using uint = unsigned int;
    
    // Three primitives:
    uint zero() { return 0; }
    bool equal(uint a, uint b) { return a == b; }
    uint next(uint a) { return a + 1; }  // increment
    
    // Build everything else from these three:
    
    // Addition via repeated increment - O(b) operations!
    uint add(uint a, uint b) {
        uint result = a;
        for (uint i = zero(); !equal(i, b); i = next(i)) {
            result = next(result);
        }
        return result;
    }
    
    // Multiplication via repeated addition - O(a*b) operations!
    uint multiply(uint a, uint b) {
        uint result = zero();
        for (uint i = zero(); !equal(i, a); i = next(i)) {
            result = add(result, b);
        }
        return result;
    }
    
    // Comparison via subtraction (via repeated decrement) 
    // O(min(a,b))
    bool less(uint a, uint b) {
        while (!equal(a, zero()) && !equal(b, zero())) {
            a = a - 1;  // here we need prev(), but it's not in the basis
            b = b - 1;
        }
        return !equal(b, zero());
    }
}
```

So too in the C++ standard: the language could formally be made very minimal, but in practice it needs operators like -, *, /, checks for <, >, standard functions over numbers so that the programmer can express ideas clearly and concisely and the compiler can translate them to the corresponding machine instructions.

### Evolution of the Computational Basis

```
═══════════════════════════════════════════════════════════════════════════
                    COMPUTATIONAL BASIS FOR UINT
═══════════════════════════════════════════════════════════════════════════

┌─────────────────────────────────────────────────────────────────────────┐
│  MINIMAL BASIS (theoretically complete, but impractical)                 │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   Primitives (3 operations):                                            │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐                              │
│   │  zero()  │  │ equal()  │  │  next()  │                              │
│   └──────────┘  └──────────┘  └──────────┘                              │
│        │             │              │                                   │
│        └─────────────┴──────────────┘                                   │
│                      │                                                  │
│         ┌────────────▼────────────┐                                     │
│         │  FROM THESE THREE OPS   │                                     │
│         │   WE CAN BUILD EVERYTHING│                                    │
│         └────────────┬────────────┘                                     │
│                      │                                                  │
│         ┌────────────▼────────────┐                                     │
│         │   add(a, b) = O(b)      │  ← Inefficient!                     │
│         │   for i=0; i<b; i++     │    2^32 iterations in worst case    │
│         │     a = next(a)         │                                     │
│         └─────────────────────────┘                                     │
│                                                                         │
│   Problem: Complexity of operations DEPENDS on argument values           │
│   multiply(1000, 2000) = 2 million next() calls!                        │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                    ▼
                            [Add efficiency]
                                    ▼
┌─────────────────────────────────────────────────────────────────────────┐
│  EFFICIENT BASIS (add operations for performance)                       │
├─────────────────────────────────────────────────────────────────────────┤
│   Primitives (7 operations):                                            │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐                │
│   │  zero()  │  │ equal()  │  │  next()  │  │  add()   │   O(1)         │
│   └──────────┘  └──────────┘  └──────────┘  └──────────┘                │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐                              │
│   │subtract()│  │multiply()│  │  less()  │                O(1)          │
│   └──────────┘  └──────────┘  └──────────┘                              │
│   Now all basic operations - O(1), constant time!                       │
│                                                                         │
│   How implemented:                                                      │
│   add(a, b)      → 1 CPU instruction:  ADD eax, ebx                     │
│   multiply(a, b) → 1-3 instructions:  IMUL eax, ebx                    │
│   less(a, b)     → 1-2 instructions:  CMP eax, ebx; SETL cl             │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
                                    ▼
                            [Add expressiveness]
                                    ▼
┌─────────────────────────────────────────────────────────────────────────┐
│  EXPRESSIVE BASIS (add operations for code readability)                 │
├─────────────────────────────────────────────────────────────────────────┤
│   All efficient operations +                                            │
│   Logically derivable but convenient:                                    │
│   ┌────────────┐  ┌────────────┐  ┌────────────┐  ┌────────────┐        │
│   │not_equal() │  │less_equal()│  │ greater()  │  │greater_eq()│        │
│   └────────────┘  └────────────┘  └────────────┘  └────────────┘        │
│         │                │                │                │            │
│         └────────────────┴────────────────┴────────────────┘            │
│                                   │                                     │
│                    Can express via equal() and less():                   │
│                    not_equal(a,b) = !equal(a,b)                         │
│                    greater(a,b)   = less(b,a)                           │
│                                                                         │
│   ┌────────────┐  ┌────────────┐  ┌────────────┐  ┌────────────┐        │
│   │ divide()   │  │ modulo()   │  │   max()    │  │   min()    │        │
│   └────────────┘  └────────────┘  └────────────┘  └────────────┘        │
│         │                │                │                │            │
│         └────────────────┴────────────────┴────────────────┘            │
│                                   │                                     │
│                    Can express via basic operations:                     │
│                    max(a,b) = greater(a,b) ? a : b                     │
│                                                                         │
│   ┌──────────────────────────────────────────────────────┐              │
│   │  ADVANTAGE: Code becomes readable and compact         │              │
│   │                                                      │              │
│   │  Before:                                              │              │
│   │    if (!equal(a, b) && (less(a, b) || equal(a, b)))   │              │
│   │                                                      │              │
│   │  After:                                               │              │
│   │    if (not_equal(a, b) && less_or_equal(a, b))       │              │
│   │                                                      │              │
│   │  Or simply:                                           │              │
│   │    if (a != b && a <= b)  // overloaded operators    │              │
│   └──────────────────────────────────────────────────────┘              │
└─────────────────────────────────────────────────────────────────────────┘
```

Compilers and libraries gradually enriched the basis of available operations: early versions of standard libraries had fewer algorithms and fewer "built-in" operations on types, and programmers had to implement what today is taken for granted—from elementary numeric functions to complex sorting and search algorithms.

Modern compilers already live in a world where the computational basis for fundamental types is rich enough and well optimized, and their main task is to use this basis as effectively as possible without breaking language semantics. When you write a + b for integers, the compiler knows which machine instruction to use, how to account for overflow, which flags will be set in the processor, and can build optimal instruction sequences on that basis.

If you create your own type—say BigInt or Rational—and define for it a minimal set of operations that is technically complete but not very expressive or efficient, then all functions you write on top of that basis will inherit its shortcomings: addition implemented as repeated increment will remain exponential no matter how you optimize. So when designing your own types in C++, you are in effect retracing the path of machine architects and compiler authors: choosing which operations to make basic so that both expressing algorithms is convenient and the compiler can generate efficient code—and it is here that the ideas of regular types and a careful choice of computational basis become not theory but practical guidance for designing good, predictable, and fast abstractions.

## Regularity

Modern C++ would not exist without regularity, which at first glance seems redundant: we are used to writing code and relying on comparison, copying, and assignment "somehow working at the level of the language and compiler".

Behind this simple behavior is an idea that lets the compiler safely optimize code and lets us reason about it in a mathematically correct way—and this idea goes back to the fundamental work of Alexander Stepanov in the early 1990s.

### Regular Type

A regular type in the modern sense is a type that behaves like primitives such as `int` or `double` and supports correct equality, copying, assignment, and default construction so that equal values remain equal after any operations and copies remain fully independent of the original—and this predictability lets the compiler replace expressions with equal ones in meaning without losing program correctness.

Regular functions, in turn, return equal results when applied to equal arguments, and this property—called regularity—is the foundation of all algorithms in the standard library, from sorting and search to sequence transformations. The whole STL was built on the assumption that types in containers and algorithms behave regularly: equality is reflexive, symmetric, and transitive; copying creates an independent copy; assignment does not change the original—and that is what made it possible to build a library where the same algorithm `std::sort` works correctly with `int`, with `std::string`, and with our user-defined types if they are designed correctly.

### Complexity of Operations

When we work with built-in types like `int`, `double`, or `bool`, we usually assume that equality, copy, and assignment take constant time, because they reduce to one or two machine instructions for comparison or moving registers—but as soon as we move to composite objects, the picture becomes more complex.

We expect that equality check will take time proportional to the total amount of data, including nested fields; however in practice this linear complexity is not always guaranteed, and compilers use tricks to speed up such operations.

If we take a multiset—an unordered collection of elements with possible duplicates, represented as an unsorted dynamic sequence (e.g. `std::vector` with duplicates)—insertion of a new element is constant time because we append. But to check equality of two such multisets we must either sort both and compare lexicographically—O(n log n)—or for each element of the first check its presence in the second with multiplicity—O(n²) in a naive implementation. The standard `std::unordered_multiset` does better: its `operator==` works in O(n) on average but can degrade to O(n²) with many hash collisions—which the standard explicitly allows. If you start using such multisets as elements of another container where equality is needed often for search or hashing, performance will drop sharply compared to types whose `operator==` is O(1).

In extreme cases equality can be NP-hard—e.g. checking graph isomorphism for data structures representing graphs—and in such situations the programmer is forced either to give up full equality in meaning or to settle for representational equality—direct bit comparison—which is fast but can give false negatives for values equal in meaning but encoded differently.

### Representational Equality

That is why when behavioral equality is too expensive or impossible to implement, we fall back to representational equality: two values are equal if their bit pattern is identical, and for composite objects this is often implemented via recursive field comparison or via `memcmp` on raw bytes.

Representational equality always implies behavioral equality, because if the bits match, the interpretation matches—the converse is not always true. But in practice it is very convenient for copy constructors and assignment: if you implement them via bytewise copy, you guarantee that the copy will be equal to the original in representation, and if the type is designed correctly, in behavior too.

Similarly, when full equality in meaning is too expensive, we can use structural ordering: lexicographic for sequences or by first differing field for structures, which allows efficient sorting and search even when true semantic order is unavailable. However, not all objects admit copying or equality at all: if an object owns a unique resource like an open file, network connection, or GPU buffer, then neither copying nor assigning to it makes sense, and such types we classify as "non-regular" and use in special contexts where we explicitly specify their semantics.

### Regularity in Modern C++

In modern C++ the ideas of regularity have finally received official expression in the form of concepts, which literally embody in the language what Stepanov was talking about in the 1990s—and these decisions directly influence the design of containers, where good types always strive for regularity.

`std::string_view`, `std::span`, `std::optional` are designed so that equality reflects meaning, not implementation details; copying means logical duplication, not shared ownership of resources. If a type is inherently non-regular—e.g. it owns a unique socket, GPU buffer, or large file—then it is explicitly classified as an "object" type and not put into value-oriented containers, or is given a weak equality model with clear documentation of what exactly it checks.

### Regularity and Optimizations

For the compiler optimizer, regularity is permission to reorder operations, replace a + b with b + a, or even cache results—without side effects and without breaking the program's observable behavior. Compilers have come a long way to understanding these ideas: in early C the notion of regularity did not exist at all, and the programmer simply relied on primitive types behaving predictably and composite structures behaving as written. But today we can write `template<std::regular T> void process(T value);` and the compiler can check at compile time that the type meets all requirements, including correct equality and copying—which radically simplifies debugging and makes code more reliable than in the era when all of this rested on trust.
