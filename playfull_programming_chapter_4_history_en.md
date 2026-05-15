# Playful Programming. History of Concepts

The history of concepts in C++ is one of the most illustrative examples of how a language evolves not linearly, but through decades of experiments, rollbacks, and rethinking. The first ideas that we today call concepts appeared back in the late 1990s, when it became clear that C++ templates have colossal expressive power but provide almost no means to describe the programmer's intent. A template could be instantiated with almost any type, but errors manifested either as kilometers of compiler messages or as unexpected runtime behavior. Even then, Stroustrup formulated the problem as "the absence of contracts for templates" — the programmer knows that a type requires operator+ or ==, but the language cannot express this.

Early concept proposals were extremely ambitious and sought to describe not just syntax, but semantics as well. For example, the concept EqualityComparable was supposed to mean not just the presence of operator==, but the fulfillment of the mathematical properties of equivalence: reflexivity, symmetry, and transitivity. Similarly, concepts for ordered types implied strict weak ordering, and for iterators — correct multi-pass behavior. This reflected an academic view of generic programming, strongly inspired by functional languages and the work of Stepanov.

---

If you were shown a description of one of the early concept proposals, you might think it was some kind of evil Python (and yes, | is used here as a logical AND that adds conditions):

```
concept Numeric {
    @abstract operator+(const T&, const T&)
    |
    @abstract operator-(const T&, const T&)
    |
    @abstract operator*(const T&, const T&)
    |
    @abstract operator/(const T&, const T&)
    |
    @delete operator<(const T&, const T&) //cannot compare
    |
    @allow T(0)  // can be created from 0
};
```

But it quickly became clear that such concepts were impossible to implement in a real compiler due to their complexity, because semantic properties cannot be verified automatically. The compiler cannot prove the correctness of operator== for all cases of its use — not unless C++ is turned into a language of formal proofs. Moreover, even formalizing such requirements in the standard specification turned out to be extremely difficult: where exactly does the boundary lie between "the programmer's promise" and "the language's guarantee"? Here is another variant of a separate concept language, which relied on a mathematical apparatus of expressions for specifying constraints:

```
concept Container<typename> {
    typename value_type
    typename iterator
    
    comparable ->
      T == T -> bool
      and T != T -> bool
      and T < T -> bool
      and T <= T -> bool
      and T > T -> bool
      and T >= T -> bool
};
```

Despite all the difficulties, work continued, and by the mid-2000s concepts had become one of the key directions in C++ development — they were actively discussed in the committee, compiler prototypes were being developed, and at some point concepts almost made it into the C++0x standard (the future C++11). However, this is precisely where the first major collision between theory and practice occurred: the proposed system turned out to be too complex to implement and too incomprehensible for the average programmer. As a result, concepts were removed from C++11 at the last moment, which was of course perceived by many in the community as a serious defeat.

However, after this came an important period of rethinking. Instead of trying to solve all the problems of concepts at once, the decision was made to simplify the model of application. This is how the idea of Concepts Lite was born as a lightweight variant of concepts — one that deliberately abandoned semantic requirements and focused exclusively on what the compiler can reliably and efficiently verify from the programmer-proposed conditions. A key role in this was played by Andrew Sutton, who proposed a simplified constraint model based on logical expressions, atomic constraints, and strict, formalized rules of partial ordering. Here is roughly how concepts would have looked if accepted in 2009:

```
// Concept with axioms to describe semantics
concept TotalOrder<typename T, typename Op> {
    requires Predicate<Op, T, T>;
    
    // Axioms describe semantic properties
    axiom Reflexivity(Op op, T x) {
        op(x, x) <=> false;  // element is not less than itself
    }    
    axiom Antisymmetry(Op op, T x, T y) {
        if (op(x, y) && op(y, x))
            x <=> y;  // if x < y and y < x, then x == y
    }
    axiom Transitivity(Op op, T x, T y, T z) {
        if (op(x, y) && op(y, z))
            op(x, z) <=> true;  // transitivity
    }
}
```

Concepts Lite were not a "behavioral contract" but became a language mechanism for specifying template constraints. In this model, a concept essentially became a named logical expression over types — an expression verifiable at compile time. A concept now merely says that "this type supports exactly the operations listed here."

This pragmatic approach turned out to be more suitable for the community, and the implementation itself was simple enough not to break existing code while complementing already existing mechanisms like SFINAE. Constraints also made template behavior more predictable and became part of the overload resolution system rather than a side effect of substitution failures — and it was precisely this version of concepts that was adopted into the C++20 standard.

Modern concepts are the result of a compromise between expressiveness and implementability. Although they are not based on verifying the semantics and correctness of algorithms, they do allow explicit expression of type requirements and significantly improve interface readability, giving the compiler enough information to select specialized overloads.

The history of concepts illustrates the C++ development philosophy well: the language does not strive to be theoretically pure, but neither does it abandon powerful ideas if they can be adapted to reality. Modern concepts are far from the academic ideal of the late 2000s and are more of an engineering tool ready for use — and that is precisely why they turned out to be viable.

### Non-empty Comments

In the very first proposals, a concept was considered a set of requirements plus a semantic contract, and the syntax from Stepanov resembled a separate mini-language.

An example of an equality concept:

```
concept EqualityComparable<typename T> {
    bool operator==(T, T);
    bool operator!=(T, T);

    /// semantic requirements:
    /// == is reflexive
    /// == is symmetric
    /// == is transitive
};
```

What is critically important here is that the last lines were not comments in the spirit of "for documentation" — they were intended to be part of the formal definition of the concept. The idea came from the work of Gries/Dijkstra and the academic school of generic programming: an algorithm is correct only when the type satisfies mathematical properties.

### Axioms That Never Were

In later versions of the early concepts, the emphasis had shifted toward the STL, and concepts were used to constrain algorithms directly:

```
template <typename Iter>
concept RandomAccessIterator
    : BidirectionalIterator<Iter> -> axiom {
        i + n;
        i - n;
        i[n];
    };

// And further:

template <RandomAccessIterator Iter>
void sort(Iter first, Iter last);
```

It is important to note here that semantic inheritance was still implied: RandomAccessIterator did not just "have operator+" but guaranteed O(1) operation complexity, correct indexing, reference stability, and so on. Formally, the language did not verify this, but the standard *promised* that if a type was declared as satisfying a concept, it was obligated to behave correctly. In any case, no existing compiler could perform such checks — it was an "honor system" contract — and so nothing progressed beyond prototypes.

## The Heavy Aughts

In the versions of concepts that almost made it into C++11, the syntax had become significantly closer to ordinary code and more deeply integrated into the template system.

```
template <typename T>
concept LessThanComparable -> requires(T a, T b) {
    { a < b } -> bool;
};
```

And usage:

```
template <LessThanComparable T>
T min(T a, T b) {
    return b < a ? b : a;
}

// But additional mechanisms existed:
template <typename T>
  requires LessThanComparable<T>
    void foo(T);
```

But the problem was that concepts:

- affected type deduction;
- affected the partial ordering of templates;
- introduced new specialization rules;
- made compiler errors even harder to understand.

Even Stroustrup acknowledged that this version was too complex and still too incomprehensible for the average programmer. The transition to Concepts Lite as we see it today is often described as a deliberate simplification, but in fact it was not a step backward — it was more of a step toward the reality of both existing projects and new codebases. After the failed attempt to include "heavy" concepts in C++11, it became clear that the language had gone too far in trying to formalize program correctness: a program may be formally correct, but that will not make it work better or be more comprehensible. It was at this moment that the idea emerged to simplify the model and keep only what the compiler is actually capable of handling.

## Lightweight Concepts

At the core of Concepts Lite lies a simple but very important idea: a concept is not a philosophical statement from Stepanov's early works about the properties of a type, but merely a technical constraint on the permissible substitutions of template parameters. In this case, a concept answers not the question "is this the right type?" but the question "can code be generated with this type?". Therefore, the modern C++20-style EqualityComparable concept requires only the existence of the expressions a == b and a != b and that their result is convertible to bool — and all the compiler needs to check is exclusively the form of the expression and its type, without attempting to draw conclusions about whether the equality operation behaves correctly from a mathematical standpoint.

```
// Concept checks only syntax, not behavior
template<typename T>
concept EqualityComparable = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
};
```

This means that properties such as reflexivity, symmetry, or transitivity — which were present in the first concept proposals — fall entirely outside the language's area of responsibility, and consequently outside the compiler's. A type can formally satisfy EqualityComparable while having an absurd implementation of operator== that returns random values or depends on global state, and from the compiler's perspective this is completely fine, because its job is not to prove the correctness of a program but to verify that the generated code is even possible.

If we compare old and new concepts, in the early, mathematically complete approach the statement "the type satisfies EqualityComparable" meant far more than just the presence of operators. Algorithms could rely on equality behaving as an "equivalence relation," and the entire correctness of the STL was built around this assumption — which was of course beautiful in theory, but that theory rested on an unspoken agreement between the type's author and the standard library.

An equivalence relation is a proof that certain elements of a set are "the same" from a particular point of view. For a relation to be an equivalence relation, it must behave as naturally as ordinary equality. Imagine we have some sign ∼ that means "equivalent," and we want it to possess the same intuitive properties as the equality sign.

For this, the relation must satisfy three axioms. Reflexivity: any element is equivalent to itself (a ∼ a), which sounds obvious but is important to state explicitly. Symmetry: if a is equivalent to b, then b is equivalent to a (if a ∼ b, then b ∼ a) — order does not matter. And finally, transitivity: if a is equivalent to b, and b is equivalent to c, then a must be equivalent to c (if a ∼ b and b ∼ c, then a ∼ c). Here is a very simple description of equivalence:

```
A relation ∼ on a set S is called an equivalence relation
if for all a, b, c ∈ S the following three axioms hold:

┌─────────────────────────────────────────────────────────────────┐
│ REFLEXIVITY                                                     │
│                                                                 │
│   ∀a ∈ S: a ∼ a                                                │
│                                                                 │
│   "Every element is equivalent to itself"                       │
│                                                                 │
│   Example: 42 = 42, a person equals themselves                  │
└─────────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────────┐
│ SYMMETRY                                                        │
│                                                                 │
│   ∀a, b ∈ S: a ∼ b  ⟹  b ∼ a                                  │
│                                                                 │
│   "If a is equivalent to b, then b is equivalent to a"          │
│                                                                 │
│   Example: if 6/2 = 3, then 3 = 6/2                             │
└─────────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────────┐
│ TRANSITIVITY                                                    │
│                                                                 │
│   ∀a, b, c ∈ S: (a ∼ b) ∧ (b ∼ c)  ⟹  a ∼ c                  │
│                                                                 │
│   "If a∼b and b∼c, then a∼c"                                   │
│                                                                 │
│   Example: if a = b and b = c, then a = c                       │
└─────────────────────────────────────────────────────────────────┘
```

These three properties together guarantee that our relation behaves reasonably and allows the entire set to be partitioned into disjoint groups of equivalent elements — that is, they form an "equivalence relation." So what do you think of the idea of such a mathematical apparatus in concepts? And another question: how would you even describe it?

The modern approach is far more down-to-earth: if a type satisfies EqualityComparable, this means exactly one thing — code that uses == and != in the appropriate places will compile. But the language provides no guarantees of algorithmic correctness. The responsibility for ensuring that operator== actually has meaningful semantics lies entirely with the programmer, while the compiler translates this into a form accessible to the processor.

## What Survived in the Standard

The old concepts did not survive because semantic requirements cannot be verified automatically by the compiler — meaning they inevitably turn into either comments or a source of problems. Formalizing such requirements in the standard turned out to be extremely difficult, and there is no clear boundary between what the language is obligated to enforce and what belongs to the developer's code. Implementing heavy concepts overloaded compilers and made the already complex template rules even more complex, instead of simplifying the programming process. Perhaps it is for the best that the committee failed to deliver a mathematically complete implementation of concepts at the time — otherwise we would now be drowning in drawing arrows and proving axioms. But concepts did not become a silver bullet either, adding a certain level of complexity to an already far-from-simple language. As for why "lightweight" concepts nevertheless led to "heavy" hierarchies — that we will discuss in the next article...
