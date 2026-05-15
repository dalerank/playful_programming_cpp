# Playful Programming. Concept Hierarchy

In the previous articles I covered how overloads work and how the compiler finds the right functions in associated namespaces, but what happens when the compiler finds not one but several matching overloads at once? This question becomes especially relevant when working with templates and concepts, because the same type can satisfy the requirements of several functions simultaneously — and this is where the mechanism for selecting the most appropriate overload comes into play. Without this selection, the entire system of requires and concepts would not work.

How does the compiler choose the best overload when there are several suitable candidates? Intuitively we expect that a more "precise" function should take priority over a more general one, and we often encode this expectation into compiler rules when writing templates and constraints. The general idea is as follows: overloads can not only be enumerated, but arranged into a hierarchy by degree of specificity — some functions describe a broad class of types, others describe a subset of it, and when the argument type is known, the compiler should choose the function whose requirements most precisely match that type. This logic is built directly into the C++ standard and is called *partial ordering*, because not all overloads are necessarily comparable with each other.

---

At the heart of partial ordering lies the concept of *subsumption* — or, informally, one set of requirements subsumes another if every type that satisfies the stricter requirements automatically satisfies the weaker ones as well. In that case, the stricter variant is considered more specialized and wins during overload selection.

Let us return to the resolve() example from the previous article, where we had two versions of a function:

```
template<typename T>
void resolve(T x) requires std::integral<T>;

template<typename T>
void resolve(T x) requires std::integral<T> && (sizeof(T) > 4);
```

The first function accepts any integer type, while the second accepts only those integer types whose size is greater than four bytes. It is obvious that the set of types for the second function is a subset of the set of types for the first — any type that fits the second version automatically fits the first, but not vice versa.

If we call resolve(10ull), both overloads are formally applicable: long long is an integer type, and on most platforms its size is greater than four bytes. But the compiler does not get stuck, because it compares the constraints and sees that the second version is stricter — that is, more specialized — and it selects exactly that one, without any ambiguity and without needing any additional hints from the programmer.

This is a very important difference from the old SFINAE-based techniques, where similar behavior had to be ensured manually: adding dummy parameters, complicating enable_if conditions, keeping track of the overload order, and hoping that the compiler would interpret them as intended.

With concepts, this hierarchy becomes part of the formal model of the language rather than a side effect of tricks. But it is worth keeping in mind that partial ordering is also not a cure-all — there are situations where two sets of requirements do not subsume each other. For example, one concept might constrain a type by its size, and another by the presence of a particular operation, and neither of them is strictly stronger in the general case. In such situations the compiler will honestly report an ambiguity, because from the perspective of formal logic it genuinely has no grounds to prefer one variant over the other.

```
#include <concepts>
#include <iostream>

// Concept: type has size greater than 4 bytes
template<typename T>
concept LargeType = sizeof(T) > 4;

// Concept: type supports the increment operation
template<typename T>
concept Incrementable = requires(T t) {
    { ++t } -> std::same_as<T&>;
};

// First overload: requires large size
template<LargeType T>
void process(T value) {
    std::cout << "Large type version\n";
}

// Second overload: requires increment
template<Incrementable T>
void process(T value) {
    std::cout << "Incrementable version\n";
}

int main() {
    long long x = 42;  // Satisfies both concepts!
    
    process(x);  // Compilation error: ambiguity!
                    // long long is both LargeType and Incrementable,
                    // but neither concept is stricter than the other
    
    return 0;
}

<source>:31:5: error: call to 'process' is ambiguous
   31 |     process(x);  // Compilation error: ambiguity!
      |     ^~~~~~~
<source>:18:6: note: candidate function [with T = long long]
   18 | void process(T value) {
      |      ^
<source>:24:6: note: candidate function [with T = long long]
   24 | void process(T value) {
      |      ^
```

The concept hierarchy and the partial ordering mechanism together allow overloads to be built as a system of refinement contracts, where you can start with the general case and gradually add more and more specialized versions without fear of conflicts or unexpected behavior. The compiler takes these refinements, builds a table of strict comparisons from them, and predictably selects the most appropriate function, guided by exactly the requirements we explicitly formulated in the code.

### What "stricter" means

Now let us figure out exactly what the word "stricter" means in the context of requires and concepts. It should be noted right away that this is not about the programmer's subjective impression (an informal agreement, as it was with SFINAE), but about a formal relationship between conditions that the compiler knows how to analyze — namely, constraints.

The intuitive explanation is fairly simple: if the fulfillment of condition A *automatically implies* the fulfillment of condition B, then A is considered more specialized and B more general. Or put another way: the set of types satisfying A is a subset of the set of types satisfying B. Now, when the compiler has to choose between overloads, it prefers the one whose requirements are stricter, because it describes a narrower and more precise case.

For example, the condition std::integral\<T\> && sizeof(T) > 4 is stricter than simply std::integral\<T\>, because any type that satisfies the first condition is guaranteed to satisfy the second, but not the other way around. Similarly, requires (sizeof(T) == 4) is stricter than requires (sizeof(T) >= 1): being exactly four bytes automatically means the size is not less than one byte, but that is merely a special case of the more general condition.

```
// General condition: any integer type
template<typename T>
requires std::integral<T>
void process(T value) {
    std::cout << "General integral version\n";
}

// Stricter condition: integer type with size greater than 4 bytes
template<typename T>
requires std::integral<T> && (sizeof(T) > 4)
void process(T value) {
    std::cout << "Large integral version (size > 4)\n";
}

// Another example with different levels of specificity
template<typename T>
requires (sizeof(T) >= 1)  // Very general condition
void display(T value) {
    std::cout << "Any type with size >= 1\n";
}

template<typename T>
requires (sizeof(T) == 4)  // Stricter condition
void display(T value) {
    std::cout << "Exactly 4 bytes\n";
}

int main() {
    int x = 42;           // 4 bytes on most platforms
    long long y = 100;    // 8 bytes on most platforms
    short z = 5;          // 2 bytes
    
    process(x);   // Calls first version (int usually 4 bytes, not > 4)
    process(y);   // Calls second version (long long > 4 bytes)
    process(z);   // Calls first version (short < 4 bytes)
    
    std::cout << "\n";
    
    display(y);   // Calls first version (sizeof(long long) >= 1, but != 4)
    display(z);   // Calls first version (sizeof(short) >= 1, but != 4)
    
    return 0;
}

Program stdout
>> General integral version
>> Large integral version (size > 4)
>> General integral version

>> Any type with size >= 1
>> Any type with size >= 1
```

The compiler is not a programmer and does not reason "intuitively" as shown above, which is why C++ strictly formalizes these relationships through the concept of *subsumption* — one set of constraints subsuming another. One set of constraints subsumes another if every disjunctive element of the first includes all the conjunctive elements of the second. The formulation sounds heavy, but there is a very concrete logic behind it.

In order to be able to compare constraints, the compiler first reduces them to a structured form, breaking down the requires expression into elementary components and treating the logical connectives && and || not as arbitrary expressions, but as logical operations on sets of requirements. After that, these elementary conditions are compared pairwise.

Let us consider a simple example with logical OR:

```
requires (std::integral<T> || std::floating_point<T>)
```

This means the type must be either an integer or a floating-point type: two alternative paths for satisfying the condition are permitted, and only if neither of them is met does the constraint fail. This is a disjunction — a set of alternative requirements. Now let us look at an example with logical AND:

```
requires (std::copyable<T> && std::movable<T>)
```

Here the situation is the opposite: both checks must be true simultaneously. The type must be both copyable and movable. This is a conjunction — a set of requirements each of which is mandatory.

The compiler collects requires constraints into logical formulas built from atomic predicates connected by && and ||. It does not prove (because the Great Programmer wrote it so) their truth in the general logical sense, but compares their structure and checks whether one constraint follows from another at the level of syntactic form.

And here we come to a very important and often surprising detail: two constraints are considered identical only if they are syntactically identical. Logical equivalence plays no role in this case, because the compiler does not perform logical operations and does not reduce expressions to a canonical form — simply because that would require introducing a mathematical apparatus for such proofs into the compiler.

```
template<typename T>
concept TrueConcept = true;

template<typename T>
concept AlsoTrueConcept = (C<T> && true);
```

From the perspective of logic and common sense, the concepts *TrueConcept* and *AlsoTrueConcept* are equivalent — this is usually taught to first-year students in introductory logic lectures. Both are always true, but for the compiler these are different expressions, because their syntactic structure differs, and the standard explicitly states that such constraints are not considered equivalent.

The consequences of such "clever" constraints on the programmer's side can be quite unpleasant. If we overload functions with requires TrueConcept\<T\> and requires AlsoTrueConcept\<T\>, the compiler ends up in a situation where neither overload subsumes the other, because they are logically equal but formally incomparable. The result is simply an overload ambiguity, and the standard permits the compiler to issue an error of the form *ambiguous overload, no diagnostics required* — meaning there is not even a guarantee of an intelligible message.

> "Two atomic constraints are considered identical if they are formed from the same expression at the source level and their parameter mappings are equivalent."

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/172/fa7/09b/172fa709b50afddaae4026a3db7a1a8e.png)

And this moment illustrates well the general philosophy of constraints and concepts, which make template code more strict and formal but in return demand discipline from the programmer. If you want the compiler to see a hierarchy and specialization, express your constraints explicitly, simply, and consistently — but do not rely on logical identities and "obvious" transformations. The compiler will not perform them, because it lacks the mathematical apparatus to do so.

### How it works in practice

Now let us move from the formal model provided by the standard to the practical consequences that a developer encounters in real code. At this stage many developers begin to feel a slight discrepancy between "*how the system seems like it should work*" and "*how it actually works*."

A common situation arises where, by intent, one function clearly should "subsume" another, but the compiler does not "see" this — and the reason is almost always the same: the constraints are formulated slightly differently. To a human, these formulations look equivalent (a logical straight line) or obviously related (a logical curve), but to the compiler they are simply different syntactic constructs with no formal relationship between them.

The typical symptoms are an unexpected overload ambiguity or the selection of a different function version than expected. In such cases the problem is almost never in the requires mechanism itself, but in precisely how the constraints were expressed. Therefore the general and proven approach is: do not duplicate compound constraints manually, but extract them into a separate concept. For example, instead of writing in multiple places:

```
requires (Resolvable<T> && IsPath<T>)
```

it is far more reliable to define a single concept:

```
template<typename T>
concept ResolvablePath = Resolvable<T> && IsPath<T>;
```

and then use it directly:

```
requires ResolvablePath<T>
```

Because the moment you slightly change one of the five places where that expression appeared, the remaining four will automatically differ. And it can get downright comedic.

```
// Attempt to create a general and specialized version
template<typename T>
requires (Resolvable<T> && IsPath<T>)
void unified_process(T item) {
    std::cout << "General version\n";
}

// Specialization with IsAbsolute
template<typename T>
requires (Resolvable<T> && IsPath<T> && IsAbsolute<T>)
void unified_process(T item) {
    std::cout << "Specialized for absolute paths\n";
}

// Another developer adds their version, but writes in a different order
template<typename T>
requires (IsPath<T> && Resolvable<T> && IsAbsolute<T>)  // Different order!
void unified_process(T item) {
    std::cout << "Another specialized version\n";
}

// unified_process(fp);  // ERROR: ambiguous
                         // Compiler sees TWO specializations with IsAbsolute,
                         // but they are syntactically different and it cannot
                         // determine which one is "more specialized"
```

From a human perspective, the solution with a single entry point — i.e., one concept with constraints — changes nothing. But from the compiler's perspective the situation becomes fundamentally better: the constraints now have a common name and a single syntactic form, which allows the hierarchy to be correctly constructed, overloads to be compared, and partial ordering rules to be applied without surprises. In essence, you are helping the compiler see the very structure you already had in mind.

A good example of a consciously designed concept hierarchy is the iterator concepts of the standard library. In the STL they are arranged not as a set of independent checks, but as a strict ladder of abstractions, where input_iterator sits at the bottom, followed by forward_iterator, bidirectional_iterator, and finally random_access_iterator — and each next concept includes the previous one and adds new requirements.

```
┌─────────────────────────┐
                    │   input_iterator        │
                    │  (base level)           │
                    │  - read elements        │
                    │  - move forward         │
                    └───────────┬─────────────┘
                                │
                                │ includes + adds
                                │ multi-pass guarantees
                                ▼
                    ┌─────────────────────────┐
                    │  forward_iterator       │
                    │  - everything from input│
                    │  + multi-pass traversal │
                    └───────────┬─────────────┘
                                │
                                │ includes + adds
                                │ backward movement
                                ▼
                    ┌─────────────────────────┐
                    │ bidirectional_iterator  │
                    │  - everything from fwd  │
                    │  + operator --          │
                    └───────────┬─────────────┘
                                │
                                │ includes + adds
                                │ random access
                                ▼
                    ┌─────────────────────────┐
                    │ random_access_iterator  │
                    │  - everything from bidir│
                    │  + operator []          │
                    │  + arithmetic           │
                    │  + O(1) movement        │
                    └─────────────────────────┘
```

The idea here is simple and elegant: any random_access_iterator is a bidirectional_iterator, any bidirectional_iterator is a forward_iterator, and so on. This is expressed directly in the concept definitions. For example, forward_iterator is formulated in terms of input_iterator plus additional requirements on the iterator category. As a result, a clear subsumption relationship exists between these concepts, one that the compiler can analyze automatically.

The practical consequence of this hierarchy is readily visible during overload selection. Imagine two versions of a distance() function:

```
template<std::input_iterator It>
void distance(It it, It e);

template<std::random_access_iterator It>
void distance(It it, It e);
```

If we call *distance()* with a *std::vector\<int\>* iterator, the compiler will without hesitation choose the second version, because the vector's iterator satisfies both concepts, but *std::random_access_iterator* is stricter — and therefore more specialized. This is exactly what the programmer expects: for a more powerful iterator, a more efficient implementation is used.

But even with all the formal strictness of concepts, complex and deep hierarchies can lead to unexpected collisions — something Alexandrescu has pointed out repeatedly in his talks, where his position boils down to a simple but mature idea: not every hierarchy is worth encoding through overloads and partial ordering. In real code it sometimes turns out to be simpler and more reliable to express the selection logic explicitly rather than relying on an automatic mechanism, and instead of many overloads you can write a single function and use if constexpr inside it:

```
if constexpr (std::contiguous_iterator<It>) {
    // the fastest version
} else if constexpr (std::random_access_iterator<It>) {
    // version for random access
}
```

Such code is often easier to read, easier to debug, and easier to extend. It explicitly shows the priority order and does not require the reader to understand the subtleties of subsumption between a dozen concepts. Both Herb Sutter and Alexandrescu have noted that it is not always worth chasing maximum generality at the cost of readability and predictability. Concepts are a tool, not an opportunity to show off one's cleverness — they are well suited for formalizing interfaces and expressing contracts, but they are not obligated to solve all your architectural problems for you. Nor are overly strict constraints on every little thing:

```
template<typename T>
concept Readable = requires(T t) {
    { *t };
};

template<typename T>
concept Incrementable = Readable<T> && requires(T t) {
    { ++t } -> std::same_as<T&>;
};

template<typename T>
concept Decrementable = Incrementable<T> && requires(T t) {
    { --t } -> std::same_as<T&>;
};

template<typename T>
concept Comparable = Decrementable<T> && requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
};

template<typename T>
concept Arithmetic = Comparable<T> && requires(T a, typename std::iterator_traits<T>::difference_type n) {
    { a + n } -> std::same_as<T>;
    { a - n } -> std::same_as<T>;
};

template<typename T>
concept IndexAccessible = Arithmetic<T> && requires(T a, typename std::iterator_traits<T>::difference_type n) {
    { a[n] };
};

template<typename T>
concept FastDistanceMeasurable = IndexAccessible<T> && requires(T a, T b) {
    { a - b } -> std::convertible_to<typename std::iterator_traits<T>::difference_type>;
};

template<typename T>
concept Contiguous = FastDistanceMeasurable<T> && std::contiguous_iterator<T>;
```

To summarize, the picture that emerges is as follows: the compiler selects an overload based on the strictness of requires constraints and the formal relationships between them, which allows concept hierarchies to be built and expressive, type-safe interfaces to be written — but syntactic identity of constraints matters more than their logical equivalence, and overly complex hierarchies can harm code clarity.

As at the end of the previous article, where I spoke about the need for moderation in constraints, so here I repeat the thought that good design combines carefully isolated concepts with a sound balance between compiler automation and explicit logic in the code — all of this helps create code that not only works correctly but remains comprehensible to other developers. Nevertheless, for all its power, the concept system has limits of applicability, and it is important to understand where those limits lie — and that will be the subject of the next article...
