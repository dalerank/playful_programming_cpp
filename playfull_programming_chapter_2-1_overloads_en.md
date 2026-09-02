# Overloads

So, overloading in `C++`... they don't teach it at university, and they don't give it to you in books…

Let us start with a story. A power function lives in a project, written by someone long ago, time-tested and never letting anyone down because all this time it computed damage multipliers in `int` and `float`. Then the project migrates to deterministic fixed-point arithmetic, to that exact `Fixed` from the previous chapter, and with a clear conscience the team applies the function to the new type, since it is a template, and therefore generic.

```cpp
struct Fixed { int32_t raw; };            // 16.16, that exact type from the previous chapter

Fixed operator*(Fixed a, Fixed b) {       // multiplied and put the point back
    return Fixed{(int32_t)(((int64_t)a.raw * b.raw) >> 16)};
}

template <typename T>
T pow_naive(T x, unsigned n) {
    T result = T{1};                      // <- what happens here now?
    for (unsigned i = 0; i < n; ++i)
        result = result * x;
    return result;
}

void damage_bonus() {
    Fixed per_stack{(int32_t)(1.1 * 65536)};     // +10% per stack, raw == 72089
    Fixed mult = pow_naive(per_stack, 13);       // expecting a multiplier around 3.45
    printf("raw=%d\n", mult.raw);                // raw=1, meaning 0.0000153
}
```

It will compile, and even run in some fashion, but produce a damage multiplier of roughly zero. The issue is that `T{1}` for `Fixed` is not one at all; it is `raw == 1`, simply the smallest positive number this type can represent. Multiplying by such a number in 16.16 format shifts the result sixteen bits to the right, so the accumulator starts with one unit in the least significant bit and stays there for all thirteen iterations.

Neither `Fixed`, nor the algorithm, nor even the programmer who brought them together is to blame here; the culprit is our assumption that a foreign type will behave like a plain integer, but, as Jack Reacher said, assumptions kill. We will return to this assumption at the end of the chapter, and for now let us unpack where an algorithm gets the right to demand anything of a type in the first place, and why overloads in the language are not about pretty names, and in fact not about names at all.

## Algorithm first, types second

The conversation about overloads should begin not with syntax or templates, but with the idea of building an interface that extends to new data types while remaining clear and functional. The authors of the standard library themselves formulated the key thought this way: first you must properly compose (design) the algorithm, and only then understand which types it works for. You might have heard this phrase at conferences or read it in books, but remembering book examples is a distinct skill, and most examples are read and promptly forgotten... In practice, this idea is violated constantly, and `pow_naive` from the opening of the chapter violated it as well, because the algorithm was written for the concrete types `int` and `float`, and nobody wrote down the requirements on the generic type when creating it. Let us write them down; it does not take long, you just have to read the function body line by line and ask at every line what it wants from `T`.

```cpp
template <typename T>
T pow_naive(T x, unsigned n) {
    T result = T{1};              // 1. type has a multiplicative identity,
                                  //    written as T{1}
    for (unsigned i = 0; i < n; ++i)
        result = result * x;      // 2. has operator*, producing T from two T values
                                  // 3. result can be assigned back
    return result;                // 4. value is copied out
}
```

That gives four requirements from four lines, and `Fixed` satisfies three of them: we wrote multiplication for it, while assignment and copying came from the underlying `int32_t`. Only the first is not satisfied, and that is a fairly common occurrence: specific initialization is easy to forget because for `int` and `double` it produces no errors.

Look at those comments once more, do you see why this idea is so frequently broken? Making an algorithm is not hard, but doing it off the top of your head... it is very easy to start with types, classes like "Weapon" and "Human", hierarchies, and other baseline concepts familiar to a programmer, and only then try to force the algorithm onto the data. But generic programming implies the opposite path.

Take a moment to inspect the code of algorithms in the standard library: you will see that they are generalized as much as possible, and a single algorithm serves plain integers, strings, and complex classes alike, *if* the type provides the necessary operations **with the expected semantics** (regularity, neutral element, sensible comparison, and so on; see the previous chapters). That is, overloaded operators alone will be enough for code to compile, but not enough to keep `sort` and `accumulate` from calculating nonsense.

Therefore, when tackling an algorithm, you must abstract away from concrete data types and ask yourself not "what is this object," but "what operations over it are needed and what must they guarantee." Requirements then become a consequence of the algorithm, rather than a property of whichever type happened to be at hand.

## Type matches syntactically

`float` for speed and many other things matches "syntactically" because it is a number and you can add and multiply it, but semantically it is incorrect because with the type `float` we lose information about physical meaning.

```cpp
void naive_physics() {
    float acceleration = 9.8f;   // m/s²
    float velocity = 100.0f;     // m/s
    float mass = 50.0f;          // kg
    float dt = 2.0f;             // s

    // The compiler allows writing nonsense
    float nonsense1 = acceleration + mass;      // 9.8 + 50 = ???
    // adding m/s² to kg, physically absurd

    float nonsense2 = velocity * mass;          // 100 * 50 = ???
    // got kg·m/s, which is momentum, not acceleration

    float nonsense3 = acceleration / velocity;  // 9.8 / 100 = ???
    // got s⁻¹, frequency, unrelated to the task

    float nonsense4 = velocity + dt;            // 100 + 2 = ???
    // and here we added velocity and time, and it all compiles again

    // Everything compiles with zero error signals, leaving
    // just wrong numbers with no asserts or compiler diagnostics
    printf("%.1f %.1f %.1f %.1f\n", nonsense1, nonsense2, nonsense3, nonsense4);
}
```

This is cured by "strong types": you have to introduce types that preserve meaning, and write overloads that enforce the rules, even if that makes the code somewhat bloated. Notice that the overload below does not merely "give different things the same name"; it returns different types, meaning it carries semantic intent rather than just superficial form.

```cpp
struct Seconds { float v; };
struct Metres  { float v; };
struct Speed   { float v; };     // m/s
struct Accel   { float v; };     // m/s²

Speed  operator*(Accel a, Seconds t) { return Speed{a.v * t.v}; }
Metres operator*(Speed s, Seconds t) { return Metres{s.v * t.v}; }

void typed_physics() {
    Accel g{9.8f};
    Seconds t{2.0f};

    Speed v = g * t;         // 19.6 m/s, type changed along with meaning
    Metres path = v * t;     // 39.2 m of travel at constant speed

    // Accel oops = g * g;   // no overload for this, which is good news
    // Metres bad = v;       // will not compile either, meters are not speed

    printf("%.1f %.1f\n", v.v, path.v);
}
```

## The classic textbook overloading example

Now let us look at how overloading is usually presented in textbooks, thinking strictly in terms of types: "we have `int`, `double`, `string`, let us write a function for each."

```cpp
// three functions, one name, and not a single algorithm
int add(int a, int b) { return a + b; }
double add(double a, double b) { return a + b; }
string add(const string& a, const string& b) { return a + b; }
```

The resulting overload set becomes a way to give similar things the same name so the programmer does not have to remember `add_int()`, `add_double()`, and `add_string()`. It is more convenient than three names, no dispute there, but this is a superficial understanding that never moves the programmer past procedural thinking, and it never becomes the generic programming you see in `std` when you look into the algorithms. It is merely syntactic sugar masking a collection of separate functions under a single name, and for a fourth type you would have to write a fourth copy of `a + b`.

## Generalizing the algorithm

In `Notes on Programming`, Stepanov writes that the structured programming school (Dijkstra, Wirth, Hoare, Dahl) fascinated him, but following their advice in assembly was difficult until he realized that the ideas of structured programming are not tied to any particular language. Stepanov examines the classic teaching example of `x^n` in the spirit of Gries and Dijkstra, though the algorithm of exponentiation by squaring itself is older and broader than the research of those authors.

The first solution that typically comes to mind is to keep an accumulator variable and multiply it by x in a loop n times. That is `pow_naive` from the opening of the chapter; it is easy to explain, easy to write, easy to test, and often asked in interviews. At one point the primary question was "can we do it faster," since practically everyone writes the naive solution. We can, and have been able to for a very long time:

> **Around 200 BC.** The ancient Indian scholar Pingala, in his treatise on prosody ("Chandas Shastra", also known as "Chandas Sutra"), investigated the combinatorics of poetic meters using short (*laghu*) and long (*guru*) syllables. Positional binary arithmetic with zero was not yet present there, but listing combinations of syllables already amounts to binary places, and the rules for converting between combination numbers and the combinations themselves are known primarily from Halayudha's commentary in the 10th century. Knuth reviews this history in `The Art of Computer Programming`, Volume 2, Section 4.6.3, and traces exponentiation by squaring directly to that source.

> **1976 and 1981.** Dijkstra, `A Discipline of Programming`, and Gries, `The Science of Programming`. The structured programming school puts the program and its proof side by side, and in Gries fast exponentiation is analyzed as a textbook exercise on loop invariants, because the interest is no longer in "how to write it" but in "how to prove what is written is correct."

> **2009.** Stepanov and McJones, `Elements of Programming`. Exponentiation is derived not for numbers, but for any associative operation, and that is precisely the word missing from textbook formulations.

The algorithm known as exponentiation by squaring relies on the binary representation of the exponent and solves the problem in logarithmic time. Instead of performing n multiplications, we halve the exponent and square the base, and if the current exponent is odd, we additionally multiply the result by the current value of the base.

```cpp
template <typename T>
T pow_fast(T x, unsigned n) {
   T result = T{1};

   while (n > 0) {
       if (n & 1)
           result = result * x;

       x = x * x;
       n >>= 1;
   }

   return result;
}
```

In programming, any integer can be represented in binary, which means any power can be decomposed into a product of powers of two. For instance, x to the 13th power is x to the eighth multiplied by x to the fourth and x to the first. The algorithm sequentially iterates through the bits of the exponent starting from the least significant, deciding at each step whether the current base contributes to the final result.

```text
x^13 = x * x * x * x * x * x * x * x * x * x * x * x * x   (13 factors)

13₁₀ = 1101₂ = 8 + 4 + 1,  meaning  x^13 = x^8 * x^4 * x^1

x^1 = x                    (0 squarings)
x^2 = x^1 * x^1 = x²       (1 squaring)
x^4 = x^2 * x^2 = x⁴       (2 squarings)
x^8 = x^4 * x^4 = x⁸       (3 squarings)

loop iterates across bits from lowest to highest:

   bit position: 0     1     2     3
   bits of 13:   1     0     1     1
   square:      x^1   x^2   x^4   x^8
   included:    yes   no    yes   yes
                 ↓           ↓     ↓
        R  =    x^1    *    x^4 * x^8  =  x^13

   3 squarings + 3 multiplications into accumulator = 6 operations
   versus 13 in the naive loop (the code above does one extra
   squaring on top of this, which we will return to separately)
```

Now the number of squarings needed is equal to the number of significant bits in the exponent minus one (in the code above there is still one extra, which we will remove shortly), and multiplications into the accumulator equal the number of set bits. For n = 13 that is 13 multiplications versus 6, for n = 100 it is one hundred versus nine, for n = 1000 one thousand versus fifteen, and for one million one million versus twenty-six.

## What an algorithm needs from a type

As you have already gathered, to build a sound, convenient, and correct algorithm, you need to look not so much at the algorithm itself as at what it requires from the data type. The algorithm does not care whether `x` is an integer, a floating-point number, a very small or very large integer (`int8_t` or `__int128`), a matrix, or something else. It needs a multiplication operation to exist, a neutral element for that multiplication to be present, and values to be copyable. A couple of items are missing here, and those are the ones forgotten most often.

The first item is associativity. The fast algorithm does not just multiply in a different order, it multiplies different pairs, because instead of x·x·x·… it now calculates squares and assembles the answer from them. The second item in the list seems to be there already, namely the neutral element, but it is written too implicitly as `T{1}` and off we go. A neutral element belongs to an operation, not a type, and writing it as a literal works only for built-in numbers. For `Fixed`, one is `raw == 65536`, whereas `Fixed{1}` is 2⁻¹⁶; for a 2×2 rotation matrix, the identity is `{1, 0, 0, 1}`, while `Mat2{1}` is `{1, 0, 0, 0}`, a projector that discards half the matrix.

This is verifiable in a couple lines of code: a 90-degree rotation to the fourth power must yield the identity matrix, and with a true identity it does, while with `T{1}` it returns `{1, 0, 0, 0}`. What to do about this we will examine later, and then it will become clear why some algorithms have specialized requirements on their input arguments.

The third item (well, I did promise two) is usually omitted from requirements lists, but it is there. Look at `pow_fast` again: the line `x = x * x` sits before the shift `n >>= 1` and runs every time, including when the last significant bit has already been processed and the result is ready. That is, the algorithm requires the type to store a value that never appears in the answer. For `Fixed` 16.16, a multiplier of 1.5 to the seventeenth power is 985.26, which fits comfortably, but the superfluous square computes 1.5 to the thirty-second, which is 28,274,844,190 in `raw` units, overflowing `int32_t`, whereas the naive loop on the exact same data never overflows.

> An attentive reader might also recall commutativity, but let us leave that as homework. A brief note to spark interest: commutativity is not needed here because the factors all come from the same `x` and the algorithm does not scramble their order, so the matrix from the end of the chapter works fine even though `A * B` and `B * A` differ for matrices. For integers, reordering pairs changes nothing, but for `float` it does, because floating-point multiplication is associative only in textbooks. On my machine I ran 12,600 pairs of (base, exponent), where the base runs from 1.001 to 1.2 in steps of 0.001 and the exponent from 2 to 64; `pow_naive` and `pow_fast` diverged in their bit patterns on 10,941 pairs, which is 86.8% of cases.
>
> For `1.1f` to the 100th power, the naive loop produces 13780.6357, while the fast one gives 13780.6475; the error may seem small, but it exists, and if a client computes a multiplier one way while the server computes it another, you get the desync from the previous chapter with no obvious cause. With `Fixed` the same thing happens, because truncation on the right shift is not associative either, and for 1.1 to the 13th power the naive version yields `raw == 226210` while the fast version yields `raw == 226213`.

Or take integers: `pow_fast(2, 16)` for `int` returns the correct 65536, but along the way it computes 65536 * 65536, which overflows signed integer arithmetic and formally constitutes undefined behavior. Yes, the compiler will likely optimize away this dead multiplication under `/O2` if the exponent is known at compile time, but when the exponent arrives from runtime, the multiplication remains in the loop as is, and what happens under that UB or how it affects our code is unpredictable. Relying on such an assumption is unwise, especially when the fix takes a single reordering.

```cpp
template <typename T>
T pow_fast_careful(T x, unsigned n) {
   T result = T{1};                // we will deal with this line in the next chapter

   while (n > 0) {
       if (n & 1)
           result = result * x;

       n >>= 1;

       if (n)                      // squaring is needed only if more bits follow
           x = x * x;
   }

   return result;
}
```

The difference is a single reordering, yet the requirements on the type have already changed. You should never think "the algorithm works with any type that has `*`," but rather "here is the list of operations the algorithm performs on the type; verify that your type qualifies."

## Why this is what overloads actually are

And here we connect directly to overloads and generic programming. We have a single algorithm, in the code above represented by the template `pow_fast`, while concrete multiplication is selected based on type: for `int` and `double` that is the built-in `*`, and for `Fixed` or a matrix it is the overloaded `operator*` we wrote by hand. We could have written an overload set `pow(int, …)`, `pow(Matrix, …)` instead of a template, but the essence remains the same: one name and one algorithm across differing implementations of an operation. The language binds an abstract algorithm to concrete types precisely through operator (and function) overloading combined with templates.

```cpp
struct Mat2 { int a, b, c, d; };           // rotations and reflections on a grid

Mat2 operator*(Mat2 m, Mat2 n) {           // that very "overloaded operator"
    return Mat2{m.a * n.a + m.b * n.c, m.a * n.b + m.b * n.d,
                m.c * n.a + m.d * n.c, m.c * n.b + m.d * n.d};
}

template <typename T>
T pow_fast_careful(T x, unsigned n) {      // algorithm from mid-chapter, word for word
   T result = T{1};

   while (n > 0) {
       if (n & 1)
           result = result * x;

       n >>= 1;

       if (n)
           x = x * x;
   }

   return result;
}

void one_algorithm() {
    printf("%d\n", pow_fast_careful(3, 5));            // 243, here T{1} guessed right

    Mat2 rot{0, -1, 1, 0};                             // 90-degree rotation
    Mat2 r4 = pow_fast_careful(rot, 4);                // four rotations, expecting identity
    printf("{%d %d %d %d}\n", r4.a, r4.b, r4.c, r4.d); // {1 0 0 0}, not {1 0 0 1}
}
```

Multiplication slotted in correctly, which is what operator overloading was invented for, but identity did not. Formally we could supply that too by adding an `int` constructor to `Fixed` that shifts the value sixteen bits, making `T{1}` mean unity. Yet that is once again a convention rather than a requirement, unchecked by the compiler, and there is no `operator 1` in the language.

For an aggregate like `Mat2`, you would have to write the constructor by hand, because `Mat2{1}` initializes only the first field. For a matrix whose dimension is known only at runtime, even such a constructor will not help, because the identity matrix depends on operand dimensions, meaning value rather than type, and constructing it inside the algorithm is impossible in principle.

This is where generic code breaks most often, and why `std::accumulate` receives its initial value from the caller. By the way, if you look at how the standard library handles powers, `std::pow` has no integer overload; integer arguments are converted to `double`, computed there, and returned as `double`. So `pow(3, 38)` is not thirty-eight integer multiplications, but a journey into floating-point math where beyond 2⁵³ exact answers are no longer guaranteed, since not every integer is representable there; on my machine it returns 1350851717672992000 instead of 1350851717672992089, missing by 89. The standard library is rich in overloads, but it is not obligated to have everything you need.

It turns out that overloads (in the broad sense, covering both functions and operators) become not just a way to avoid different names, but the underlying mechanism for "one algorithm for different types, provided the types satisfy the requirements." This is no longer a side convenience, but a core tool of generic programming alongside templates and (later) concepts.

## What this means for your algorithms

- Before calling a template generic, write down the list of operations it performs on the type. Not "needs `*`," but "needs associative `*`, needs a neutral element, needs headroom for an extra square," and so on.
- The neutral element `T{1}` works only for built-in numbers; for custom types it returns garbage. Agreeing that a literal represents one is possible, but that convention cannot be enforced by the compiler.
- If the same quantity is calculated in two places, compute it with the exact same code. In the examples above, 86.8% of pairs diverged in bit patterns between two correct implementations of the same function.
- Operator overloading is a way to delegate a type its share of the work while leaving the algorithm its structure, which is why overloads live alongside templates and concepts rather than in a "syntactic sugar" section.

Keeping these simple, genuinely simple constraints in mind changes how you design interfaces and algorithms. You start thinking not about classes and hierarchies, but about the semantics of operations and which properties of types make an algorithm correct and efficient. That is, you start thinking in the style of the standard library and generic programming, as the language designers intended. Meanwhile, the function from the opening of the chapter remains broken in one place, where we assumed all types have a unity value. In the next chapter we will look at how to fix that.
