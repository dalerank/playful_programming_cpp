# Overloads

## Table of Contents

- [Introduction](#introduction)
- [Generalizing the Algorithm](#generalizing-the-algorithm)
- [Generalizing the Constraints](#generalizing-the-constraints)
- [Generalizing Overloads](#generalizing-overloads)
- [Good Overloads](#good-overloads)
- [Bad Overloads](#bad-overloads)
- [Another Bad Example](#another-bad-example)
- [How to Do It Better](#how-to-do-it-better)
- [Conclusion](#conclusion)

---

## Introduction

So, overloading in C++ — not the way they teach it at university or present it in books…

Let's talk about overloads and generic programming, but let's start not with syntax and not with templates, as most textbooks and technical literature do, but with the idea. In C++, function and method overloading is often perceived as a convenience, or a way to "give different things the same pretty name," but in reality it's a much more powerful tool. Used well, it lets you build universal interfaces — interfaces that can be extended to new data types while remaining understandable and safe.

To grasp this, we need to recall what actually lies at the foundation of generic programming and its particular application in the form of overloads. The founders of the STL put the key idea of the standard library in the simplest possible terms: first you need to properly define the algorithm, and only then figure out which types it works for. This phrase often comes up at conferences and in the books of smart people, but in practice it gets violated constantly. Not because it's hard to follow, but because it's very easy to start with types, classes, hierarchies, and other programmer-familiar building blocks, and only then try to "fit" an algorithm onto them. Generic programming proposes the opposite approach, and if you look at the algorithms in the standard library, they are maximally generalized — in most cases they work just as well for plain integers as for a complex class with overloaded operators.

Starting from the problem the algorithm solves, you should always abstract away from concrete data types and ask yourself not "what is this object," but "what operations does it need to support." Does it need to support multiplication, comparison, copying, creation of a neutral element? The answers to these questions naturally reveal the requirements on the types, and those requirements don't come out of nowhere — for the acceleration (a physical quantity) of some entity, we won't suddenly reach for floats (yes, they work syntactically, but semantically a float has no business being there), because the requirements become a consequence of the algorithm.

float works syntactically — it's a number, you can add and multiply it — but it's semantically wrong because we lose information about physical meaning.

```cpp
// "Naive" approach — start from the type:
float acceleration = 9.8;  // m/s²
float velocity = 100.0;     // m/s
float mass = 50.0;          // kg
float time = 2.0;           // s

// The compiler happily lets you write nonsense:
float nonsense1 = acceleration + mass;        // 9.8 + 50 = ???
// Adding m/s² to kg — physically absurd

float nonsense2 = velocity * mass;            // 100 * 50 = ???
// Got kg·m/s — that's momentum, not acceleration

float nonsense3 = acceleration / velocity;    // 9.8 / 100 = ???
// Got s⁻¹ — a frequency, completely unrelated to the task

// Everything compiles, but errors only show up at runtime, as wrong results
```

The classic textbook example of overloading:

```cpp
// "Function overloading allows using one name
//  for functions with different parameter types"

// Add two integers
int add(int a, int b) {
    return a + b;
}

// Add two floating-point numbers
double add(double a, double b) {
    return a + b;
}

// Add three integers
int add(int a, int b, int c) {
    return a + b + c;
}

// Concatenate strings
std::string add(const std::string& a, const std::string& b) {
    return a + b;
}


// Pavlovskaya-style
// Area of a circle
double area(double radius) {
   return 3.14159 * radius * radius;
}

// Area of a rectangle
double area(double width, double height) {
   return width * height;
}

// Area of a triangle
double area(double base, double height, bool is_triangle) {
   return 0.5 * base * height;
}

int main() {
   std::cout << area(5.0) << "\n";            // circle
   std::cout << area(4.0, 6.0) << "\n";       // rectangle
   std::cout << area(3.0, 4.0, true) << "\n"; // triangle
}

// Stroustrup-style
// Complex numbers
class Complex {
   double re, im;
public:
   // Constructor from two numbers
   Complex(double r, double i) : re(r), im(i) {}

   // Constructor from one number (imaginary part = 0)
   Complex(double r) : re(r), im(0) {}

   // Default constructor
   Complex() : re(0), im(0) {}

   void print() const {
       std::cout << re << " + " << im << "i\n";
   }
};
```

All the examples above are syntactic sugar. Thinking in terms of types — "we have int, double, string, let's write a function for each" — you miss the general solution. The resulting set of overloads becomes a way to give similar things the same name so that the programmer doesn't have to remember add_int(), add_double(), add_string(). But that's a shallow understanding, one that never takes the programmer beyond procedural thinking. It's not the generic programming you see in std when you dig into the algorithms. It's just syntactic sugar masking a set of separate functions under a single name.

---

## Generalizing the Algorithm

In "Notes on Programming," Stepanov writes: "Structured Programming School: Dijkstra, Wirth, Hoare, Dahl. By 1975 I became a fanatical disciple. I read every book and paper authored by the giants. I was, however, saddened by the fact that I could not follow their advice. I had to write my code in assembly language and had to use goto statement. I was so ashamed. And then in the beginning of 1976 I had my first revelation: the ideas of the Structured Programming had nothing to do with the language."
https://studylib.net/doc/25271030/alexander-stepanov----notes-on-programming--2018-

I've read both Stepanov's book and Dijkstra's books, and I'm more drawn to another approach that Stepanov borrowed from Gries/Dijkstra (David Gries/Edsger Dijkstra) — also nearly a textbook example — raising a number to a power, computing x to the n. The first solution that comes to mind is trivial: we introduce an accumulator variable and multiply it by x exactly n times in a loop. This solution is easy to explain, easy to write, and easy to verify, and it's often proposed in job interviews. At some point the main question wasn't "how to do this" (almost everyone writes the solution), but can we do it faster?

```cpp
template <typename T>
T pow_naive(T x, unsigned n) {
   T result = T{1};

   for (unsigned i = 0; i < n; ++i)
       result = result * x;

   return result;
}
```

It turns out we can, and significantly so. There is an algorithm called exponentiation by squaring. It relies on the binary representation of the exponent and solves the problem in logarithmic time. Instead of doing n multiplications, we repeatedly halve the exponent and square the base. If the current exponent is odd, we additionally multiply the result by the current value of the base.

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

Intuitively the algorithm works like this: any integer can be represented in binary, and therefore any power can be decomposed into a product of powers of two. For example, x to the 13th is x to the 8th multiplied by x to the 4th and by x to the 1st. The algorithm sequentially "walks" the bits of the exponent, starting from the least significant, and each time decides whether the current base participates in the final result.

```
x^13 = x * x * x * x * x * x * x * x * x * x * x * x * x

13₁₀ = 1101₂ = 8 + 4 + 1

x^13 = x^8 * x^4 * x^1

x^1 = x                    (0 multiplications)
x^2 = x^1 * x^1 = x²       (1 multiplication)
x^4 = x^2 * x^2 = x⁴       (2 multiplications)
x^8 = x^4 * x^4 = x⁸       (3 multiplications)

Bits of 13: 1     1    0    1
            ↓     ↓    ↓    ↓
           x^8   x^4  x^0  x^1

   R =  x^8 * x^4 * 1 * x^1
```

What's important to note is not so much the algorithm itself, but what it actually requires from the data type. The algorithm doesn't care whether x is an integer, a floating-point number, a very large or very small integer (int6_t/int128_t), a matrix, or anything else. All it needs is for a multiplication operation to exist, for a neutral element to exist, and for values to be copyable. Everything else is an implementation detail.

And here is where the connection to overloads and generic programming emerges. We have one and the same algorithm, described in abstract terms, and the concrete multiplication operation is selected depending on the type. For int it will be one multiplication, for double another, and for a matrix a third — and it is precisely through overloads and generic interfaces that the language lets you connect a single algorithm to many concrete implementations.

It turns out that overloads stop being merely a way to avoid different function names, and instead become a mechanism for expressing the idea: "one and the same algorithm, applicable to different types as long as they satisfy certain requirements." Overloading is now an integral part of generic programming, not a convenient side effect of it.

If you keep this thought in mind, designing interfaces and algorithms starts to look different — when we begin thinking not about classes and hierarchies, but about the meaning of operations and about what properties of types make an algorithm correct and efficient. That's where truly beautiful and generic code begins.

---

## Generalizing the Constraints

Now that we have the algorithm, it's natural to ask the next question: how do we make it generic? How do we turn a solution for integers into an algorithm that works not just with unsigned, but with other data types too?

The first impulse is usually very straightforward — you want to simply replace the concrete type with a template parameter T, and consider the job done. Formally the code will indeed become templated, but almost immediately you see that this is only a surface-level generalization, not a real one.

Take, for example, the check x < 0. It only makes sense for signed numeric types. If T is an unsigned type, the expression is meaningless. If T is a matrix, vector, or user-defined type, the operation may not exist at all. This is the first signal that the algorithm in its current form is actually tightly coupled to specific data types.

**The neutral element problem**

Another, even more fundamental problem is related to result initialization. If in the simplest version of the algorithm we start with one, which feels natural for integers and floating-point numbers, the moment you step outside scalar arithmetic the question stops being trivial. For matrices, the "one" is the identity matrix? And for complex numbers? And for user-defined types? The algorithm itself should not and cannot know what value plays the role of the neutral element for multiplication.

```cpp
template <typename T>
T power(T x, unsigned n) {
   if (x < 0)   // problem — partial solution
       x = -x;

   T result = 1; // problem — 1 doesn't work for all types

   while (n--)
       result *= x;

   return result;
}
```

You could try to solve this by introducing some global function like identity(T) that returns the neutral element for type T, and at first glance this looks elegant and will even work (for a while), but in practice this solution is too rigid. It imposes a global convention that may not exist and requires every type to support a non-standard interface, which immediately reduces the universality of the algorithm and makes it harder to integrate into existing code.

A much more natural and "STL-idiomatic" approach is to not try to guess the neutral element inside the algorithm, but to pass the initial accumulator value from the outside. In that case the algorithm receives an already correctly initialized state and simply does its job, without making assumptions about the data type or implementing logic unrelated to the actual power computation. Now do you understand why STL algorithms often require an initial element?

This style is fully consistent with the philosophy of the C++ standard library. Algorithms in STL are a kind of "clean rooms" — they don't validate input for correctness, they don't create objects, and they don't make architectural decisions on the programmer's behalf. Their job is to correctly and efficiently execute a given sequence of operations, while the responsibility for preparing the arguments, choosing initial values, and satisfying preconditions lies with us and the external logic that calls the algorithm.

```cpp
template <typename T>
T power(T base, unsigned n, T result) {
   while (n--)
       result *= base;

   return result;
}
```

Only this way do we get a truly generic algorithm that works with any type for which the necessary operations are defined, while keeping it simple, transparent, and easy to verify. This is how generalization and overloads start to work not as a language trick with same-named functions, but as a design tool.

---

## Generalizing Overloads

In practice we almost never deal with a single "universal" function — instead we work with a set of functions united by one property (not necessarily the name). It's not one abstract power/pow, but a whole collection of overloaded functions, constructors, operators, and methods that exist under the same name and together form a single, flexible interface. The programmer sees one name and one concept, while the compiler sees dozens of different implementations you may not even be aware of, and picks the most suitable one for the given type. It is precisely this combination of overloads that lets you write code that is simultaneously convenient, expressive, and yet doesn't sacrifice performance.

```cpp
template <typename T, typename Op>
T power(T base, unsigned n, T result, Op op) {
   while (n--)
       result = op(result, base);

   return result;
}
```

It's worth emphasizing that a zoo of overloads doesn't compete with generic programming — it complements it. The templated algorithm defines the general shape of the solution, while the overloaded operations and helper functions provide optimal behavior for specific cases. We go not from data types to an algorithm, but from the capabilities of an algorithm to the permitted types, and as a result we get a generic interface that appears simple but internally accounts for the differences between types.

To see this in an example from the standard library itself, you don't have to dig into the implementation of pow — you can just look at the simple equality operator for strings. At first glance the task seems trivial: we have std::string, we have operator ==, and we could write it in the most straightforward way — take two strings by reference and compare them via compare. Such code would be correct and readable for most programs and developers, but it contains what's called the "single window problem," or an insufficient overload set. This is more of a descriptive phrase than an established term — the "single window" framing is intuitively clear in the Russian-speaking community and accurately describes the situation, but in the C++ community it's discussed as part of the broader topic of designing overload sets.

The problems start the moment we step outside the ideal case, because in real code strings are compared to all sorts of things, and very rarely to another std::string specifically. Very often one of the operands will be a string literal — on either side — like "hello", or a const char* pointer coming from a C interface. If we only have one version of the operator that takes two std::strings, the compiler will be forced to perform an implicit conversion. The literal becomes a temporary string object, which requires a memory allocation, a data copy, and then almost immediately destruction of the whole thing.

Semantically the program remains correct in all cases, but in terms of efficiency we're doing unnecessary work — and the worst part is that this inefficiency is hidden from the developer behind a "convenient" call. The call truly looks harmless, even though dynamic memory allocation is happening under the hood. And this is where the power of multiple overloads comes in — instead of one universal operator, the standard library provides several variants: string vs string, string vs C-string, C-string vs string. All of them mean the same thing logically — an equality check — but they're implemented so that in each specific case unnecessary conversions and temporary objects are avoided.

```
string == string

const char* == string

string == const char*
```

For the developer the interface stays the same: we simply write a == "hello" and only think about implementation details in edge cases, or when we get a compile error, or notice a performance issue. For the compiler the overload set becomes a set of clearly formulated rules from which it picks the best match. This is the well-designed interface that STL presents: a single name, a single meaning, but different implementations, each optimal for its own set of types.

From this simple example, which you almost certainly use every day, follows an important rule of good design — overloads should not change the meaning of an operation, but only refine it for different forms of input. If different overloads "do different things," the interface becomes confusing and treacherous, and the conditions of use become slippery. This is exactly how the standard library achieves its balance of universality and convenience, without requiring the programmer to write specialized code by hand.

---

## Good Overloads

Real life, of course, gives us imperfect sets of programming interfaces, and the question almost inevitably arises — "are they actually well-designed?" Formally the code may be correct, may compile, may even run fast, but still be uncomfortable and treacherous to use. The problem is that overloads directly interact with the type system and overload resolution rules in C++, which is one of the most complex parts of the language — and with each new standard it gets no simpler, if anything more so.

```cpp
void log(const char* msg);
void log(const std::string& msg);
void log(const std::string_view& msg);

log("hello");              // which overload?
std::string s = "world";
log(s);                    // obvious here
log(s.substr(0, 3));       // but here?
```

There's a common joke that a good overload set is the one you wrote yourself (because you understand what you did and how it works), and a bad one is the one someone else wrote — because you have to figure out why the compiler chose this particular overload and not another. Like most jokes, there's a kernel of truth in it, but in practice there are quite concrete quality criteria.

These criteria were well articulated by Tim Sweeney (yes, the author of Unreal Engine — and before him by Titus Winters, though more broadly, and I learned about Titus much later than about Sweeney) many years ago. What makes them valuable is that they don't come from abstract "beauty," but from real experience maintaining and evolving one of the most complex systems in game development.

The first and arguably most important principle is: the developer should not have to solve an overload puzzle in their head. If correctly calling a function requires a deep knowledge of overload resolution rules in the engine, understanding template priorities, implicit conversions, and the pitfalls of references and values, that almost always signals bad interface design. A good overload set works intuitively — I look at the function name or the code and immediately understand what will happen and how, without consulting the documentation.

```cpp
void log(std::string_view msg) {
   write_to_log(msg);
}

log("hello");
log(s);
log(s.substr(0, 3));
```

The second principle is closely related to the first — all overloads in a set should have one clear purpose that can be expressed in a single short phrase. For example: "compare two strings for equality" or "raise a value to a power," as in the examples above. If explaining each overload requires caveats, comments, or extra ceremony, it means different operations are hiding under the same name and the interface has started to mislead, losing its coherence.

The third principle was formulated much later: every overload should do approximately the same thing. The behavior should be roughly the same regardless of which version the compiler selects. Differences between overloads are acceptable only at the level of performance, forms of data representation, or argument passing — but not at the level of meaning. If one overload logically does one thing and another does something noticeably different, that almost guarantees bugs and surprising behavior.

```cpp
// interface proliferation problem
void open(File& f);
void open(const char* path);
void open(const char* path, bool create_if_missing);
```

Importantly, the first two principles actually follow from the third, or you could say the third is their generalization. If all overloads truly do the same thing, the user doesn't need to puzzle over which one to pick, and their purpose is easy to state in one phrase. But the moment that unity is broken, the interface starts to "proliferate," and overloads transform from a tool of expressiveness into a source of complexity.

---

## Bad Overloads

To better feel the boundary between good and bad design, it's useful to look at a deliberately poor example of an overload set. Imagine a function with a single name — say, resolve — that has two overloads. In one case it takes an integer and, for example, normalizes it by clamping it to some valid range. In the other case the same function with the same name takes a string and... parses it as a file path, checking whether the file exists on the filesystem.

Formally, from the language's point of view, everything looks syntactically legal. It's a valid overload set, and the compiler has no trouble selecting the right version based on the argument type. But from a semantic standpoint, problems begin. Under one name hide two operations that have almost nothing in common — they solve different tasks, operate in different domains, and require different contexts.

Try reading that code without comments: resolve(x) can still be interpreted somehow if you know the type of x. But on its own it says nothing about the author's intent. And resolve("data/config.json") looks even worse: it's unclear what exactly will happen — string parsing, file checking, data loading, or something else. To understand the behavior you either have to dig into the documentation or memorize all the overloads.

```cpp
int resolve(int x)
{
   // normalize — find the nearest valid value, etc.
   if (x < 0)
       return 0;
   if (x > 100)
       return 100;
   return x;
}

int resolve(const std::string& path)
{
   // check existence and read config
   std::ifstream file(path);
   if (!file)
       return -1;

   int value;
   file >> value;
   return value;
}
```

At this point the comment on the function stops being an explanation and becomes a load-bearing part of the code — and that's a sure sign that two different pieces of functionality are hiding under one name, given the same label for some reason.

```cpp
// top secret blueprints here
auto a = resolve(x);

// and here we used to wrap fish
auto b = resolve("data/config.json");
```

"Broken" overload sets are dangerous because they mask problems of visibility and responsibility. Even if the code compiles, looks clean, and passes review, that doesn't mean you won't have to examine every overload individually when making a change. The moment the interface is used at a larger scope or lands in another developer's hands, it becomes a source of problems.

The rule of thumb here is simple: if you can rename an overload and the code doesn't get worse, the overload was a bad idea. You should overload only when a single concept stands behind a single name. Sure, names feel "scarce" and people prefer to economize at the expense of meaning — but that kind of economy stops being expressive and starts only misleading.

---

## Another Bad Example

Looking critically at some of the more recent additions to the standard library, you can find examples where the principles of good overload set design are violated. One of the most telling cases is std::filesystem — this has been noted more than once in committee discussions.

std::filesystem is often cited as an example of a "modern" and convenient API for working with the filesystem, and in terms of the capabilities it brings, it really is rich and represents a major step forward for the language. But if you look specifically at the overload design it implements, it's far from the benchmark set by the algorithms.

The problem starts with the central data type std::filesystem::path. This type is meant to be a universal representation of a path, and a huge number of overloaded functions are built around it — meaning the starting point wasn't an algorithm, but an established practice and a data type. As a result, almost every operation has multiple versions: with path, with const char*, with std::string, sometimes with an additional std::error_code, sometimes without. Formally all of this is supposed to look like convenience for the user, but in practice it turns into a complex and ambiguous maze of choices where the inexperienced developer typically picks the first solution that seems to fit — note, not the correct one, just the fitting one. I'm sure everyone has seen that funny video; it's sad, but it perfectly describes how most developers actually use std::filesystem.

For example, the exists function — in one case it takes a path and can throw an exception; in another it takes a path and an std::error_code and doesn't throw. Semantically those are already two different contracts: one version works through exceptions, the other through error codes, but the function name is the same. Looking at a call to exists(p), you can't tell from the code which error handling model is in use until you look at the signature or the documentation.

```cpp
std::filesystem::exists("data/config.json");

// Throws exceptions
bool exists(const std::filesystem::path& p);

// Doesn't throw, returns error via error_code
bool exists(const std::filesystem::path& p, std::error_code& ec) noexcept;
```

The situation gets even worse when implicit conversions of std::filesystem::path enter the picture — path is implicitly constructible from a string. As a result, a call that looks perfectly innocent on the surface actually involves creating a temporary path object, memory allocations, encoding conversion, and again a choice between several overloads. To know exactly which version will be called and what side effects it carries, you need a solid understanding of all the overloads and the resolution rules.

A separate problem is the overloads that group operations with different meanings under very general names: status, symlink_status, exists, is_regular_file, is_directory look like variations on one idea, but in practice they differ in behavior, especially in the presence of symbolic links and errors. The developer is forced to constantly remember which function follows symlinks, which doesn't, and how each one responds to an inaccessible path. Formally this turns out to be not one overload set but a separate API family, flattened into the linear namespace of the standard library right alongside the algorithms. But while the algorithms explicitly tell you what they do inside (I do what you see), here all the cognitive burden (read the docs before using) is shifted onto the developer.

From the standpoint of the principles described above, several of them are violated at once. We are clearly made to "solve the overload puzzle in our heads" — to correctly use std::filesystem (to avoid being the person from that video) you need to remember not just what the functions do, but the differences between their error-handling versions, exceptions, and implicit conversions. The purpose of the functions couldn't be expressed with a single short name, because different behavioral models hide under the same name. And finally, the overloads don't quite do the same thing — the differences aren't limited to performance, they affect the meaning of the call itself.

This doesn't mean std::filesystem is "bad" or unusable. This part of the standard library solves a large and complex problem, and the chosen API simply reflects the explicit compromises made during standardization. From the standpoint of good design, its overload sets are more of a cautionary example than a model to follow. The code becomes less self-documenting, and understanding what's happening requires knowing the library's internals — and that's precisely why, when designing your own interfaces, it's useful to keep this example in mind: even the standard library is not immune to questionable decisions, though of course there are good reasons why they went with this particular design.

---

## How to Do It Better

Continuing the conversation about good overload sets, it's useful to look at situations where overloading genuinely "works as intended" and strengthens an interface rather than complicating it. Such examples usually share one important trait: all overloads serve the same idea, just adapted to different forms of input or different usage scenarios.

Let's start with the simplest and perhaps most common case: overloads for related types. Here we can return to working with strings in C++ — if you look at your code, you'll see that string representation basically comes down to two types: std::string and const char*. This looks natural to anyone using the language, because both types represent a string, just in different forms, and that's a historical legacy that has become convention.

Good historical practice is to make one of these overloads the base and the other a thin wrapper. One takes std::string (let's call it the thin wrapper), and the other (the base) calls the const char* variant. As a result, behavior is always consistent, all logic is in one place, and you avoid creating temporary objects and unnecessary allocations without complicating either the interface or the implementation.

Another common and perfectly justified technique is adding overloads by argument count. Imagine a concat function that assembles a string from several fragments. From the user's perspective it's convenient to pass one fragment, two, or three, without thinking about containers, argument arrays, or formatting. Calls like concat("Hello"), concat("Hello", world), or concat("Hello", world, '!') look natural and are easy to read.

```cpp
auto s1 = concat("Hello");
auto s2 = concat("Hello, ", world);
auto s3 = concat("Hello, ", world, "!");
```

What's important is that in all these cases the goal is the same: build a string. The only difference is the amount of input, and the overloads don't change the meaning of the operation — they just make the interface more flexible and convenient. We don't need to know the implementation details, and we can confidently trust the compiler to pick the form of the call that best matches the current task.

Another well-known example is std::vector::push_back, where overloading is used not for convenience but for performance. There's a version taking const T&, intended for copying an already existing object, and a version taking T&&, which allows efficiently moving temporary objects or the result of std::move. From the programmer's perspective both functions do the same thing — append an element to the end of the vector — but from the implementation perspective they differ fundamentally in cost. That's precisely the strength of this overload set: the semantics remain unified, the behavior predictable, and the differences appear only in efficiency — we don't need to think about which overload exists, letting the compiler choose the optimal one.

---

## Conclusion

Summarizing these examples, the core principle when working with overloads becomes clear: they should solve the same task, but for different forms of input or different situations. They should not change the meaning of an operation, and they should certainly not mislead the developer. In C++, overloading is a powerful tool for building convenient and performant interfaces, but only as long as semantic unity and common sense are preserved. As you've probably noticed, a large number of overloads makes an interface flexible but simultaneously increases the risk of confusion and implicit errors. To keep an overload set predictable and safe, it's important to explicitly specify which types a given overload is valid for.

Historically in C++ this was done with techniques like SFINAE and enable_if, which worked, but the code quickly became heavy to read and maintain. With the arrival of concepts and requires the situation improved enormously — type constraints became part of the interface, readable and expressive, and overload management became explicit and controlled. This is what allows us today to build generic APIs that remain powerful yet understandable to humans, not just compilers. And that's what the next chapter will be about…
