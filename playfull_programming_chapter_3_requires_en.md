# Constraints

In C++, overloads of functions and templates have historically been—and remain—a powerful tool for expressing different implementations of the same interface. To many, overloading looks like a convenient way to give one name to different functions, but in practice, understanding how the compiler chooses the right overload can become a source of bugs and confusion. The compiler follows a complex set of rules we have given it, taking into account not only argument types but also the order of specializations, type conversions, const qualifiers, template parameters, and much more. And errors that arise from overloading are often hard to diagnose, because the compiler’s message may point to deeply nested implementation details instead of the obvious source code. That was the subject of the previous chapter...

With the introduction of **concepts** and **constraints** (`requires`), the language gained the ability to manage this complexity at the interface level. Instead of relying on overload “magic” and SFINAE tricks, we can now state our intentions directly: what properties a type must have for a function or template to be valid—moving from “magic overload resolution” to a declarative description of type requirements.

Let’s talk about what constraints (`requires`) actually do in modern C++, and why this mechanism was such an important step in the evolution of templates. We need to step back a bit and remember that historically, templates in C++ were a powerful but rather dangerous tool—almost a language within a language, with which you could do almost anything if you had the will.

In the end, the compiler would accept any type you gave it, and the check of whether it “really fits” was deferred until instantiation, which often led to an error far from the call site, with the message at the actual failure point turning into a multi-page report about the compiler’s internals and how it handles templates. `requires` changes this model by letting us state our expectations of a type explicitly, right in the declaration of a function or class.

In essence, **requires** is a way to impose constraints on template parameters in the language itself, stating a contract: “take a type T that can do this and that.” If the type does not satisfy these conditions, the template does not even participate in overload resolution—we exclude the wrong instantiation not through a substitution failure (SFINAE) but by ruling out the unsuitable candidate at the overload-selection stage. This helps not only the compiler narrow the set of candidates but also us, because the compiler can now give a short, meaningful message instead of a cascade of secondary errors.

## Cat != cat

Consider a simple example: we have a function that checks equality of two objects of the same type, and we reasonably expect that type to support the `==` operator. Before C++20, we would only get an error about the comparison not being supported after the compiler had tried all possibilities under the hood, hit SFINAE a few times, and then dumped a long log. With `requires`, we can state that expectation directly:

```cpp
template<typename T>
bool check_equality(const T& a, const T& b)
   requires std::equality_comparable<T>
{
   return a == b;
}
```

Here we are saying explicitly: this function exists only for types `T` that satisfy the `std::equality_comparable` concept. If we try to call it with a type that has no `==` operator, the compiler will not try to “drag” us into the template; it will immediately report that the constraint is not satisfied—this type is not equality-comparable. That is a fundamentally different level of feedback compared to classic C++, where the same mistake would produce a chain of messages about a suitable operator not being found somewhere deep inside.

This becomes especially important when there is more than one requirement. In real code, functions rarely impose just one requirement on a type; we might want a type to behave like an iterator and also support a total ordering. We can now combine such conditions directly in the function declaration:

```cpp
template<typename T>
void resolve(const T& v)
   requires std::forward_iterator<T> && std::totally_ordered<T>;
```

In this case the compiler checks each condition separately, and if the type fails at least one, the diagnostic will clearly say which requirement was violated. This contrasts sharply with the old SFINAE techniques based on `enable_if`, partial specializations, and odd expressions with `decltype` hacks, where the error often looked like “no matching overload” with no explanation of why.

## Simplifying “overloaded” code

Another important aspect is splitting overloads by type category. For example, one version of a function works only with integral types, another only with floating-point types. Using `enable_if`, we might write:

```cpp
template<typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
resolve(T x) {
   // Implementation for integral types
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, void>::type
resolve(T x) {
   // Implementation for floating-point types
}
```

With this approach the return type is cluttered with technical details. Without experience in such code, it may be unclear without comments what `enable_if` is doing. We still have to rely on the template “magic” of `::value` and `::type`, and in any case the “what is required” logic is hidden inside the function.

Or, with SFINAE, the same idea from another angle:

```cpp
template<typename T>
auto resolve(T x) -> std::enable_if_t<std::is_integral_v<T>> {
   // Implementation for integral types
}

template<typename T>
auto resolve(T x) -> std::enable_if_t<std::is_floating_point_v<T>> {
   // Implementation for floating-point types
}
```

With `requires` we can write several functions with the same name but different constraints, and thus express different intentions for different type families:

```cpp
template<typename T>
void resolve(T x) requires std::integral<T>;

template<typename T>
void resolve(T x) requires std::floating_point<T>;
```

There are no `enable_if` tricks or hidden conditions here; when reading this code, even without deep knowledge of template mechanics, you can easily see the author’s intent—that the function’s behavior depends on which mathematical category the type belongs to. For the developer this is especially valuable, because `requires` effectively encourages stating requirements to abstractions clearly, creating an explicit contract (or specification) in the code instead of hiding it in “business logic” implementation details.

## A bit more on signatures

With `requires`, if we try to call `resolve()` with a type that is neither integral nor floating-point, the compiler is in an honest, clear situation: no suitable overload exists. It will not guess, will not try to “force” the type into one of the versions, and will not dive into template substitution details; it will simply say that neither `requires std::integral<T>` nor `requires std::floating_point<T>` is satisfied. For the developer this looks like a normal, logical interface error.

Why does this work? From the compiler’s point of view, functions with the same name and formally the same parameters but different `requires` conditions are different functions. This may seem surprising if we are used to thinking of overloading only in terms of the parameter list, but the C++ standard explicitly says: two functions are the same entity only if they have equivalent parameters and equivalent `requires` conditions.

In other words, constraints are now **part of the function signature** at the language level. At the same time, it is important to understand a subtle but essential point: `requires` is part of the function’s interface but does not affect name mangling. So from the linker’s perspective there is no name conflict, while at overload resolution the compiler sees different candidates, each with its own applicability conditions. That is why code like this is perfectly valid (the compiler’s internal logical names for the functions will differ):

```cpp
template<typename T>
void resolve(T t) requires (sizeof(T) > 4)
// inside the compiler: resolve_t_sizeof_gr_4

template<typename T>
void resolve(T t) requires (sizeof(T) <= 4)
// inside the compiler: resolve_t_sizeof_ls_4
```

Both functions exist at once. When we call `resolve()` with a concrete type, the compiler substitutes that type, then checks the constraints and simply picks the version for which the expression in `requires` is true. There is no magic here, as there was with raw templates; it is normal overload selection with an extra filter in the form of constraints.

At this point we should not forget a typical pitfall: if the `requires` conditions are not mutually exclusive, we can easily get ambiguity. Consider:

```cpp
template<typename T>
void resolve(T t) requires (sizeof(T) >= 3);

template<typename T>
void resolve(T t) requires (sizeof(T) <= 4);
```

For type `int`, both conditions are true at the same time, and from the compiler’s point of view both overloads are equally good. Neither overload is more specialized, so we get an ambiguous call. An ambiguous call is not a bug or an implementation quirk—it is a consequence of having specified overlapping contracts. So when designing interfaces with `requires`, we should keep them as clear as possible: either the conditions must be strictly disjoint, or one version must be strictly stronger and clearly dominate the other.

---

## Template templates

Let’s look at another important aspect of `requires`: they can act not only as a simple logical filter, as in the examples above, but also as a tool for checking that expressions are valid. This is especially useful when we care not about numeric conditions like `sizeof(T) > 4`, but about whether a given operation or expression exists for a type. The language provides **requires-expressions** for this, which are most often used when defining concepts. We can define a concept for equality-comparability like this:

```cpp
template<typename T, typename U>
concept entity_comparable = requires(T a, U b) {
   { a == b } -> std::convertible_to<bool>;
};
```

It looks more like pseudocode than “normal” C++. But the meaning is: for types `T` and `U` there must exist objects `a` and `b` such that the expression `a == b` compiles and its result is convertible to `bool`. If any of these requirements fails (e.g. `==` is not defined or returns an odd type), the concept’s contract is not satisfied.

As in the cases above, the error will be local and precise, and the compiler will point to the exact expression it could not validate. That lets us state type requirements in the language’s terms, not in terms of compilation side effects like `void` substitution or SFINAE. We describe not “what will break if the type is wrong” but “what a suitable type must be,” and that is why `requires` and concepts fit so well for tightening existing template code: they narrow and stiffen the interface instead of the old “try it and see what the compiler says.”

## Simple and complex requires

If you are just starting with `requires`, the simplest form will look like an ordinary compile-time boolean expression—it looks and behaves like a `constexpr bool`. We check some property of the type, get true or false, and depending on that the function either participates in overload resolution or not. Most replacements of template conditions boil down to this: size checks, alignment, type category, various `std::is_*` traits and standard-library concepts that themselves reduce to boolean conditions.

After using it for a while, you will notice that there is also a more powerful “complex” form of `requires`, when we care not about the value of some predicate but about **whether the operation itself is valid**. Then we ask the compiler: “Can we write this expression for this type at all?”; “If yes, what does it return?”; “Does it have extra properties, such as a no-except guarantee?” This is no longer abstract logic but direct checking of syntax and semantics at compile time.

The construct `requires { ++a; }` means: for the given type there must be a prefix increment operation, and the expression `++a` must be valid. We say nothing about what it returns or about other properties in the simple case. But if we write `requires { { ++a } noexcept; }`, we add a finer requirement: the operation must not only exist but be marked as non-throwing, so we can state requirements on the behavior at the contract level.

When there are many requirements, writing them directly in the function declaration becomes awkward and hard to read. Long chains of conditions in `requires` quickly turn into noise that obscures the author’s intent, so the language introduced **concepts**. A concept is essentially a named set of requirements to which we give a meaningful name and then use as a building block for other checks.

Suppose we want to express: “type `T` can be implicitly converted to type `U`.” We can define a concept:

```cpp
template<typename T, typename U>
concept CanConvertTo = std::is_convertible_v<T, U>;
```

After that, `CanConvertTo` becomes part of our concept vocabulary, and we can use it in template parameters without spelling out the check each time:

```cpp
template<CanConvertTo<int> T>
void resolve(T value);
```

We can go further and use the abbreviated syntax with `auto`, which makes the code look almost like normal, understandable C++ even to someone not deeply versed in the language:

```cpp
void resolve(CanConvertTo<int> auto value);
```

To a reader, we have removed even the template declaration and reduced the constraint to: the function takes a value that can be converted to `int`. What used to require a “two-story” template is now stated directly in the signature, without reading the implementation or comments.

Amid all these twists with templates, constraints, and new syntax, it is important to remember that concepts are not a special kind of type or a new category of entity. A concept, like the rest of template machinery, is simply a **compile-time predicate**—in effect a boolean expression that depends on template parameters. That is why it can be used not only in function parameter lists but in many other places, for example in `static_assert` to pin down an important precondition:

```cpp
template<typename T>
void some_function(T value) {
   static_assert(CanConvertTo<T, int>);
   // rest of the code...
}
```

With this we state explicitly: the following code only makes sense if `T` can be converted to `int`; and if that stops being true (e.g. after someone’s refactor), the error will appear immediately and in a clear place.

Similarly, a concept can be used in `if constexpr` to choose different implementation branches depending on type properties, or the result of the check can be stored in a `constexpr bool` if that improves readability. In all these cases concepts act as named, compiler-checked conditions, not informal agreements between developers.

# Complex requires and then some

Finally, it is worth looking at more involved cases where `requires` is used not for one or two checks but as a full-fledged language for stating requirements. It is important to understand that `requires` can check not only that expressions are valid but also **that types and nested declarations exist**, and that operations involving them are valid. Moreover, inside a single requires-block we can combine several requirements, and they must all be satisfied at once. Sound complex? Let’s sort it out.

Suppose we care not just about a type `T` but that it behaves like a container with a specific interface. We can require that the type has a nested type `iterator` and that dereferencing such an iterator yields a value convertible to `int`:

```cpp
requires {
   typename T::iterator;
   { *std::declval<typename T::iterator>() } -> std::convertible_to<int>;
}
```

The line `typename T::iterator;` is a **type requirement**. It means: `T` must have a nested type with that name. If it does not, the `requires` condition is not satisfied, and the corresponding function or template is simply not considered by the compiler. The next line checks an expression: we take an imaginary iterator of that type, dereference it, and require that the result can be converted to `int`. So we impose at once an architectural requirement (there must be a type with a nested iterator), a structural one (the iterator must support `*`), and a semantic one (the result must convert to `int`).

Such requires-blocks can already be seen as a compact description of a mini-interface: not an abstract “container” but one with precisely fixed behavior—which interface elements we need and in what form—with all requirements combined by logical AND. If at least one fails, the whole block is false.

The same kind of compound requirements include checks on return types and operation properties, as in the `-> std::convertible_to<...>` example that constrains the result of an expression. We can also specify `noexcept` there, giving a different contract when we need the operation not to throw. In the end we can state very precise contracts (without overdoing it): not just “the operation exists” but “it exists, returns the right type, and does not throw.”

This leads to such nested and compound requires-blocks being moved into separate concepts with meaningful names, which are then used in function and class signatures. That keeps a balance between expressiveness and readability: the tricky checking logic stays in one place, and the interface and behavior stay where they belong.

---

## Summary

To sum up, `requires` and concepts give the programmer a language for talking to the compiler in terms of clear conditions, not vague “if this one doesn’t fit, try the next” heuristics. Just as we can control overloads and state expectations about types and have them checked automatically at compile time, we can now state which operations we need and what they should return, without resorting to bulky, hard-to-explain template tricks like SFINAE.

The result is code that is both stricter and clearer: stricter because requirements are checked formally, and clearer because those requirements are visible in the interface instead of being buried in implementation details. How much stricter and how much clearer—we’ll see next.

