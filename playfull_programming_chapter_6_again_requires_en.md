# Playful Programming. Constraints Revisited

Although concepts are indeed a powerful and expressive tool, they have fundamental limitations that are important to know about in order to avoid using them for the wrong purposes. These limitations are not accidental and are not "unfinished" parts of the language — they reflect a deliberate architectural decision made by the C++ committee.

The first fundamental limitation is the prohibition on direct and indirect recursion in a concept definition. Simply put, a concept cannot reference itself either directly or through a chain of other concepts. For example, the following definition is illegal:

```
template<typename T>
concept Recurse = Recurse<T>;
```

Even if you try to disguise the recursion through intermediate concepts, the result will be the same — the code will still not compile. This limitation was introduced to eliminate the possibility of infinite loops during constraint checking; unlike ordinary templates, concepts must be checked quickly and predictably, without any risk of getting stuck in an infinite loop during compilation.

---

The second important limitation is that you cannot place a *requires* on the definition of a concept itself. While inside the body of a concept you can write checks, use requires-expressions, logical operations, and other concepts, you cannot additionally constrain the concept's template from the outside with another *requires* — because a concept must always remain a self-contained logical predicate, not yet another variant of templates with its own constraints.

Both of these limitations are closely tied to the philosophy the C++ committee adhered to: not to turn concepts into yet another full-fledged type-level metaprogramming language, as happened with templates. History already knows plenty of examples where a type system becomes so expressive that entire programs are written in it, as happened with type classes in Haskell.

That is why C++ consciously chose a different path and decided to sacrifice expressiveness in favor of predictability, bounded complexity, and an understandable compilation model. Concepts should describe *requirements on types*, not serve as a means of computation or recursive abstraction.

```
template<typename T>
concept CopyableConcept = std::copyable<T> && std::movable<T>;
```

This approach fits the language model perfectly: it is clear, readable, and works well with the partial ordering mechanism, where the compiler effortlessly understands that `CopyableConcept` is stricter than each of its constituent concepts individually, and can correctly apply this when selecting overloads.

### Forms of writing requires

C++ provides several syntactic variants, each of which is convenient in its own situation. The most explicit form is a direct requires placed before the function declaration:

```
template<typename T>
requires std::integral<T>
void resolve(T x);
```

This variant reads well, especially when the conditions are simple and short. The constraint is immediately visible, emphasizing that the function exists only for a particular category of types. The second form is the abbreviated trailing form, where requires is written after the function declaration:

```
template<typename T>
void resolve(T x) requires std::integral<T>;
```

This is convenient in cases where you need to use the names of function arguments inside the conditions, or when the signature itself is already long enough and you want to visually separate the constraints from the main part of the declaration. And finally, the most compact and often the most readable form — using a concept directly:

```
void print(std::integral auto x);
```

Despite its appearance, this is still a template function; the template parameter simply is not written out explicitly, but instead constrained by a concept at the point of use. This syntax makes the code resemble ordinary functions, creating less "technical noise" while still preserving all the benefits of static type requirement checking. Together, these forms allow you to choose a balance between explicitness, compactness, and expressiveness, while the constraints imposed on concepts themselves help keep the codebase within the reasonable bounds of template engineering, rather than turning it into yet another layer of metaprogramming.

### Application to classes and variables

Now let us look at how constraints and concepts are applied not only to free functions but also to classes, methods, and even local variables — because this is where it becomes clear that concepts are not a "feature for template gurus" but an everyday development tool for a junior developer or even below.

I will start with classes and templates, where concepts can be used directly in the template parameter list of a class, constraining the acceptable types as early as the declaration. For example, if we want our engine class Vec3 to work only with floating-point types, this can be done directly:

```
template<std::floating_point T>
class Vec3 {
    // ...
};
```

Such a declaration immediately documents our intent, and any attempts to write Vec3\<Vasia\> or Vec3\<bool\> will produce an invalid-type error — the compiler will not even try to instantiate the class and emit errors somewhere deep inside the implementation; it will stop at the interface boundary, making template classes much closer to ordinary "strict" types. It is also worth noting that concepts work not only at the template level but also at the local code level — you can use auto with concepts even for local variables, for instance like this:

```
std::integral auto EntityNumber = 0;
std::floating_point auto EntitySpeed = 5.2;
```

This looks like mere "syntactic sugar" — until you are deep in debugging an animation system where every parameter type is a bare auto and the actual type is only visible ten classes deep, and the IDE says: sorry buddy, I have no idea what type this is. In practice, this technique makes code self-documenting: the reader explicitly sees not just "there's some number here" but "here is an integer counter" or "here is a floating-point speed."

The next step is applying requires to class methods. Here, unlike many other languages, a class's interface can depend on the template parameters, and *requires* makes it possible to express this formally — you can declare a method such that it only exists when the template parameter satisfies certain requirements.

```
template<typename T>
struct Entity {
    float mass() requires IsObjectHasMass<T> {
        // implementation only for objects with mass
    }
};
```

That is, the method mass() does not always exist — only when T satisfies the concept *IsObjectHasMass*. From the language's perspective, such a method simply does not exist for some instantiations, and an attempt to call it will produce not an error deep inside the implementation but a plain message saying the type has no such member. This lets us create "smart" interfaces where the set of available operations depends on the properties of the types, rather than on implicit design-time agreements. Constructors and any other functions can be constrained in the same way, which is especially useful for generic wrappers and containers where the ability to construct an object depends on the characteristics of the parametric type:

```
template<typename T>
struct EntityDamagable {
    EntityDamagable() requires HasDamageFunction<T> = default;
    EntityDamagable(const T&) requires HasHealth<T>;
};
```

In this case the default constructor exists only when *T* has some trait-function that allows dealing damage, and the constructor taking const *T&* exists only if *T* has "health" — i.e., when we are able to copy its value — allowing the class's interface to automatically adapt to the capabilities of the type while still remaining strictly formalized and compiler-verifiable code. In the end we arrive at the conclusion that concepts permeate the entire language, from template classes and methods all the way to class variables and constructors, making it possible to design interfaces that precisely reflect the capabilities of types while remaining strict, predictable, and easy to read.

**A note on partial ordering of overloads**

In the previous article on concept hierarchy I already showed that the compiler knows how to construct a partial ordering (*partial ordering*) between template functions based on which concepts are used in their constraints, and when there are several suitable overloads it selects not "the first one found" but the most specialized one.

Let me return to the resolve example from the previous article and modify it slightly. Suppose we have two resolve() functions: one accepts any integer type satisfying std::integral, and the other accepts only signed integers satisfying std::signed_integral. Now when calling resolve(-5) both overloads are formally applicable, because the type int satisfies both std::integral and std::signed_integral.

But the compiler selects the second version, because std::signed_integral is logically stricter: every type that is a std::signed_integral is automatically also a std::integral, but not the other way around. This ordering of overloads is what is called partial ordering — "partial" because for any two arbitrary constraints you cannot always unambiguously say which one is more specialized.

```
template <std::integral T>
void resolve(T value) {
    std::cout << "integral version: " << value << '\n';
}

template <std::signed_integral T>
void resolve(T value) {
    std::cout << "signed integral version: " << value << '\n';
}

int main() {
    resolve(42);    // int → signed_integral
    resolve(-5);    // int → signed_integral
    resolve(42u);   // unsigned int → integral
}
```

When compiling this code, the following occurs: for the calls resolve(42) and resolve(-5) the compiler sees two suitable overloads — both accept the type int, which satisfies both std::integral and std::signed_integral. It then compares the constraints and finds that std::signed_integral\<T\> is more specialized, since every type that satisfies this concept automatically satisfies std::integral\<T\> as well — therefore the version for signed integers is selected.

```
resolve(-5);    // int → signed_integral
```

In the case of resolve(42u) the situation is different, because the type unsigned int satisfies std::integral but does not satisfy std::signed_integral, which leads to a situation where the second overload is discarded at the constraint-checking stage, leaving only one version. The partial ordering mechanism is based on comparing the constraints written in requires — for this, the compiler reduces them to a set of so-called atomic constraints: minimal logical expressions that cannot be broken down any further — and it is at this level that comparison takes place.

Let me go over this point once more, because it can easily slip out of focus: atomic constraints are considered identical only upon literal syntactic coincidence, and even logically equivalent expressions such as `(sizeof(T) > 4)` and `(sizeof(T) > 4 && true)` will be different constraints from the compiler's perspective. But this rule makes the behavior of the system predictable and implementable.

Therefore I will repeat the conclusion from the previous article: if you want one concept to be considered more specialized than another, it must explicitly include its constraints in the same syntactic form (the logical straight line), and not merely be logically equivalent (the logical curve) — otherwise you may end up with two overloads that are incomparable, and the call will become ambiguous.

With that I am essentially concluding the introductory section on concepts, and will move on to the real "meat" of the matter: how the compiler finds the names of entities in code and what problems arise from that, how it determines which namespace to take an overload from, what ADL has to do with it and how to break it, and why mangling ate all your types. Come along — it will be terrifyingly interesting.

P.S. All of the above is theory, but if you want to learn more about memory management, hidden allocations in the standard library, and careless use of the language — especially when it comes to game development without a heap — come to my C++ course on Stepik covering C++ without dynamic memory allocation, where all of this is laid out systematically. The promo code as usual is **HABR50**; if anyone needs more or a trial access, write me a private message and I will find a spare invite.

[Playful Programming. C++ without memory allocations](https://stepik.org/a/255930)

<img width="1042" height="228" alt="image" src="https://github.com/user-attachments/assets/71afb6b9-9c2b-47b5-9596-ed533bf7e3cd" />

