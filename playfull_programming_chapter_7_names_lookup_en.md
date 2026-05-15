# Playful Programming. Name Lookup in C++

There is an old joke: "What is the difference between an ordinary C++ programmer and a good C++ programmer?" The first one writes code, and the second one can explain why it works.

That is of course a joke, but nowadays not every even good programmer can explain how a particular piece of code or the internal logic that led to a particular solution works, without resorting to ultimate phrases like "that's what the standard says" or "that's how the compiler mangled it."

At the center of everything that happens inside the compiler are two processes: name lookup and overload resolution, which we covered in the previous articles. And every time the compiler processes your code — it processes only two questions:

**First:** "What can this name possibly mean here?" **Second:** "If there are multiple options, which one is correct?"

If you understand how the compiler answers these questions, you will understand how everything else works — templates, concepts, overloads — all of it is built on answering these two questions.

---

## Does the Compiler Care About Names?

### Big Problems of Small Names

When you have several overloaded functions with the same name, "in your head" this is perceived as "one function" — I have discussed this with colleagues and even seasoned C++ veterans perceive multiple overloads as "one" function. This is not by accident: the standard's designers specifically aimed for this appearance in order to hide the complexity. The intentions are as always good: if it's called the same, it should do the same thing, right? Almost — the devil, as usual, is in the details...

```
void print(int x);
void print(double x);
void print(std::string x);
```

We see a function print and think: "There's a function print that knows how to print different things" — and this is a useful abstraction that the standard's designers impose on us. But for the compiler this is a different universe — for it these are three (3) different functions that happen to be named the same. And every time you write print(...), the compiler has to figure out which exact function you had in mind, and it is precisely because we, as humans, wanted to simplify our lives, that all the complications begin. Open any code — preferably not your own, since you know it, but someone else's — and find something like `resolve(x);`

Can you say what that is? It is probably a function call — or maybe not. In reality, the simple question "what is resolve()?" is fairly difficult for the compiler, and compilers typically have several name-analysis blocks, because resolve could be:

```
// A regular function
void resolve(x) { std::cout << "Function\n"; }
resolve(x);  // Function call
```

```
// An object with an overloaded call operator
struct Callable {
    void operator()(x) { std::cout << "Functor\n"; }
};
Callable resolve;
resolve(x);  // Call to operator()
```

```
// An object
struct resolve{
    resolve(int x) { std::cout << "Constructor\n"; }
};
resolve(x);  // Creating a temporary object
```

Now the compiler must first gather *all* possible interpretations of what resolve could be in this context — this process is called name lookup — and then select the correct interpretation — this process is called overload resolution. And there is another case; now we will have several constructors:

```
struct resolve {
  int x; 
  static int y; 

  resolve() : x(1) {} // Default constructor 

  resolve(int val) { // Constructor with argument 
    y = val;
  } 
}; 

int resolve::y = 0; // Initialization of static field
```

And code like `resolve s{};` would now be a call to the default constructor, where we create the object s.

Let us return to the case where resolve() is a regular function:

```
int resolve(int x) {  // A regular function!
    return x + 1;
}
```

Here we go... now if the compiler encounters a function with the same name, this will always be treated as a function call, not an object constructor or anything else, because C++ has a rule (if it quacks, it's a duck): if something can be interpreted as a function call, it will be a function call. If previously resolve(2) was a constructor because there were no other options, now the very same notation has become a function call — and the same syntactic form begins working with different implementations and carrying different meanings depending on context. Welcome to C++, as they say.

You can simply write code that will work — right now, on this compiler, under these conditions — but the problem is that sooner or later you will write code, or pick up someone else's code, that will not work, and the compiler will output a wall of log entries at the error site. And without understanding how name lookup works, how overload resolution happens, you will be poking around at random, changing code until it compiles. But without understanding the scopes of name lookup, you will spend a very long time changing code in various places, not knowing that the compiler found the wrong overload. All of this is compounded by modern C++ with its abundance of templates, concepts, ADL, SFINAE and everything else — but all of it depends on only the two questions from the beginning of the article: "what is this name and which variant of the name is correct."

## Is the Committee Trying to Make the Language Harder?

The meaning of any name in C++ depends very heavily on what surrounds it. If you simply look at a name as isolated text, almost nothing will be understandable — I call this the `resolve(x)` problem. Look at any code and try to answer "what is resolve (hypothetically, you can take any text)" while restricting yourself to only two lines above and below this occurrence. Most likely you will fail, because the meaning of a name can only be understood through context — what functions, variables, and logic appear above, below, and around it in the code.

If you try to replicate what the compiler does with such code (a classic example, taught in first or second year university courses if you were unlucky enough to start with C++ as your first language):

```
void resolve();      // a function
struct resolve {     // a struct
    resolve(int);
};
...
  // bla-bla lots of code
...
resolve s{42};
```

you get roughly the following:

1. The compiler finds both candidates: the function resolve and the struct resolve. The question arises: is resolve{0} an attempt to create an object with an argument, or to somehow "use" the function? The situation is obvious to us humans, but the compiler sees two entities and both fit — ambiguity. Therefore the standard gives a very specific rule for what to do in such a case:
*If a name can mean a function, then when checking candidates the compiler is required to first consider the function option, and if it finds a function with that name, that candidate is deemed correct.*
2. The compiler sees resolve{42} and knows that a function resolve exists.
3. It tries to parse resolve{42} as a notation associated with a function
4. → Either as the beginning of a function body (but you cannot write a function body here)
5. → → Or as some kind of expression involving a function pointer (but now the syntax does not fit)
6. From a syntactic standpoint for functions — this is an error
7. We go no further, see point 1, and do not even switch to considering struct resolve, simply emitting an error message.

```
<source>:11:5: error: must use 'struct' tag to refer to type 'resolve' in this scope
   11 |     resolve s{2};
      |     ^
      |     struct 
<source>:3:6: note: struct 'resolve' is hidden by a non-type declaration of 'resolve' here
    3 | void resolve();      // a function
      |      ^
```

Even if we have a struct *resolve* with a constructor from int that could handle this situation, the compiler simply never gets to it, because it is stopped by the standard rule and the failed attempt to interpret *resolve* as a function.

## Problems with Names

Now let us look at which entities can actually share the same name without conflict. It is important to distinguish two things here: "coexistence" of names, where different entities simply may have the same name (as in the example above), and overloading, where multiple entities of the same "kind" (for example, functions) deliberately share a name — which often becomes a source of confusion.

Name collisions are not always considered a conflict, and the compiler distinguishes these entities perfectly well when they belong to different categories: one is callable, the other is a type (https://godbolt.org/z/WTh1oG6sM)

```
struct resolve
{
    int x;
};

void resolve(int value)
{
    // regular function
}

int main()
{
    struct resolve f{42};   // resolve is a struct
    resolve(10);     // resolve is a function
}
```

Here the same name *resolve* is used both as a type name and as a function name, but the surrounding context fully determines what exactly is meant, and the compiler has no difficulty resolving the ambiguity.

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/66b/df7/41e/66bdf741e1e300462271c99180c78dd9.png)

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/eb1/2c3/45d/eb12c345dbac87d7f29cc89f4679ea5d.png)

By the way, this also works:

```
struct resolve
{
    int x;
};

void resolve(int value)
{
    struct resolve resolve{42};
}

int resolve()
{
    resolve(10);     // resolve is a function
    return 42;
}

int main() {
    resolve();
    struct resolve resolve{::resolve()};
}
```

But this freedom does not always exist, and if you try to declare a namespace with a name that is already used for a function, the compiler will emit an error, because namespaces live in the same "name category" as enumerations (enum) and functions, and here such ambiguity will be an error.

```
void kot()
{
}

namespace kot   // error: name conflict
{
    int x;
}

<source>(8): error C2757: 'kot': a symbol with this name already exists
                           and therefore this name cannot be used as a namespace name
```

Now the compiler cannot determine whether *kot* is a callable entity or a name container, so it prohibits such code. For the same reason, a namespace and an enum with the same name conflict with each other — a name must unambiguously point to a single entity at its name level.

```
enum Kot
{
    Vaska,
    Murzik,
    Murka
};

namespace Kot   // error: name conflict
{
    int Afigenariy;
}
```

Here *Kot* is already taken by the enumeration and cannot be reused for a namespace. However, re-declaring a namespace with the same name does not create a new entity — it simply reopens the existing namespace and adds new names to it.

```
namespace Kot 
{
    int Murzik;
}

namespace Kot
{
    double Murka;
}

namespace Kot   // not a new entity
{
    void Tuguduk();
}

int main()
{
    Kot::Murzik = 0;
    Kot::Tuguduk();
}
```

Unlike functions or variables, there is neither overloading nor redeclaration here — it will remain the same name scope. The standard makes an exception at this point. Now, if we have several declarations with the same name that have passed the rules above without triggering analyzer errors, the compiler may treat this as an *overload* or as a repeated declaration.

```
void log(int value);
void log(double value);

int main()
{
    log(10);     // calls log(int)
    log(3.14);   // calls log(double)
}
```

Here the compiler handles such code perfectly, because it sees two different parameter sets and forms a set of overloaded functions. Only if the functions differ solely in return type will overloading not occur. From the language's perspective this is considered a repeated declaration of the same function, because the return type does not participate in overload selection — welcome to C++, as they say.

```
int    size();
double size();  // error: difference only in return type
```

That is not all — the same `const` on a parameter passed *by value* does not create a new overload either, while by reference it does — but I will not go into that now to avoid straying too far into the weeds.

```
void f(int x);
void f(const int x);  // same declaration

int main()
{
    int x = 42;
    f(10);
    const int y = 42;
    f(y);
}
```

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/fb5/f14/87a/fb5f1487af276b5bb9b071c6590a4c3f.png)

The same applies to default parameter values — different default values do not mean different functions. Default parameters do not participate in forming overloads; they are merely substituted at the call site.

```
void g(int x = 1);
void g(int x = 2);  // redeclaration of the same function

int main()
{
    int x = 42;
    g(10);
}

<source>(4): error C2572: 'g': redefinition of default argument: parameter 1
<source>(3): note: see declaration of 'g'
```

So we arrive at the following set of rules:

```
Function + struct 
  -> Allowed: different name categories, context differs.

Function + namespace 
  -> Not allowed: both entities participate in name lookup at the same level.

namespace + namespace 
  -> Allowed (exception): not a conflict but a reopening.

enum + namespace / function 
  -> Not allowed: the name must be unambiguous.
```

If you think the committee members were sitting around inventing edge cases, the answer is no. The reason is much, much simpler, and behind all of this stands the ancient idea of name mangling — straight from the 80s (how the compiler encodes types into symbol names), i.e., how the compiler turns a function name and its signature into a string for the linker. The linker is essentially "dumb" and cannot understand anything other than "hashes" — and this too was done not without reason, but for speed: otherwise even a small program would take several minutes to link. For example:

```
void f(int);
void f(double);
```

may turn into something like:

- _Z1fi — f(int)
- _Z1fd — f(double)

Therefore, when we reach the linking stage, if two declarations produce the same "mangled" name, the compiler considers them to refer to the same entity. That is precisely why `void g(int);` and `void g(const int);` produce the same mangled identifier and constitute one declaration rather than an overload, while different parameters (by type) produce different mangled names and are considered overloads. At this point we are approaching the rules of name lookup, which I will cover in the following sections.

---

## At Night All Cats Are Grey, and All usings Look the Same

One of the most complex parts of C++ is still considered to be the name lookup rules, and errors related to name lookup typically manifest at runtime. The code compiles and even works for a while, but by moonlight it behaves differently from what the programmer expects. Behind simple identifiers lies a multilevel system of scopes, name categories, and special rules — and much of what is in our current standard grows directly out of the eighties, often unchanged. Let us look at how the compiler sees names in C++, what scopes exist, and why they behave differently.

C++ has several types of scopes. You can probably name off the top of your head: global namespace scope, template parameter scope, class scope, and function parameter scope — but there is also block scope and enumeration scope. Between these scopes there is a historically established asymmetry that often surprises: two using declarations introducing the same name into the same scope inside a namespace the compiler will accept without objection, but if you try to do the same in other scopes, you get a redeclaration error. In the series of "Playful Programming" articles I analyze slippery cases and how we got here. 

---

First let us look at namespace scope, where a repeated using declaration of the same name is considered harmless:

```
namespace lib {
    int value = 42;
}

namespace demo {
    using lib::value;
    using lib::value; // perfectly valid

    void f() {
        value = 10;   // unambiguously lib::value
    }
}
```

Here both `using lib::value;` declarations introduce the same name into the same scope of namespace demo. The compiler treats the second declaration as a harmless repetition of the first — no new entity is created, no conflict arises. But in block scope the situation was historically different, on paper:

```
namespace lib {
    int value = 42;
}

void f() {
    using lib::value;
    using lib::value; // compile error: redeclaration (pre-1999)

    value = 10;
}
```

Interestingly, this is one of those cases where practice outpaced the formal specification: GCC and MSVC would typically accept this, even though older editions of the standard described the example more strictly. The committee eventually acknowledged this discrepancy and brought the rule in line with how the code was already living in real compilers. CWG Issue 36 (https://cplusplus.github.io/CWG/issues/36.html) references C++98 §7.3.3/9, which had this example:

```
namespace A { int i; }

namespace A1 {
    using A::i;
    using A::i;  // OK: double declaration
}

void f() {
    using A::i;
    using A::i;  // error: double declaration
}
```

And the comparison there was quite reasonable: if a repeated function declaration is permissible, then why should a repeated using in an analogous situation suddenly break.

```
void f() {
    void g();
    void g();  // well-formed 
               // redeclaration of a function is allowed
}
```

A Proposed Resolution (04/99) was submitted at that time, which effectively permitted ignoring this rule — meaning the committee agreed that this had been an error in the standard — and in C++03 §7.3.3/9 the text was corrected, and a repeated using declaration of the same name in the same scope became allowed. Moving on, there is a separate story — and without ambiguous interpretations — for class scope:

```
struct S {
    using lib::value; // error: using-declaration for 
                      // non-member at class scope
};
```

The error here is not that value is a variable, but that inside a class, using is not a universal import of external names — it works for names from base classes, and lib is not a base class. And the most illustrative example of mixing two different mechanisms:

```
namespace B {
    int x = 10;
}

namespace C {
    int x = 20;
}

namespace A {
    using namespace B;  // using directive

    void f() {
        using C::x;      // using declaration

        A::x = 1;
        x = 2;
    }
}
```

Let us break down what is happening here: the directive `using namespace B;` does not declare any names directly inside A — it merely tells the compiler: "if during name lookup inside A you need to look outside, treat the names from B as accessible." This is an important point, because the directive does *not introduce* x into the current scope, but only expands the lookup rules.

The `using C::x;` inside function f behaves entirely differently. This construct is already a declaration that directly introduces the name ***x*** into the block scope of function f. From this point on, x inside f is a local name and a synonym for C::x.

```
A::x = 1;
```

Here a qualified name is used, so the compiler looks for x specifically inside namespace A. There is no variable x in A itself, but due to the `using namespace B` directive the name B::x becomes visible as a member of A for qualified lookup — therefore this line unambiguously refers to B::x. And now the next line:

```
x = 2;
```

This is an unqualified name, and by the name lookup rules the compiler first searches in the innermost scope — i.e., the block of function f. There it finds `using C::x;`, that is, the local name ***x*** bound to C::x. The search ends there, and B::x is not even considered. So we get a rather unexpected but fully standard-compliant result, where in the same function, in two adjacent lines, we are working with two different variables that intuitively share one name.

```
A::x = 1; // refers to B::x
x = 2;    // refers to C::x
```

And this is precisely the moment where name lookup takes a bite even out of experienced developers. Now imagine using ***using*** in a large listing with functions spanning several screens. Are you confident you can keep those namespaces in your head and catch this during a quick review? I too sometimes lose track of it — which is why usings at review time are yet another red flag that someone is doing something wrong.

```
namespace B {
    int x = 10;
}

namespace C {
    int x = 20;
}

namespace A {
    using namespace B;  // using directive

    void f() {
        using C::x;     // using declaration

        A::x = 1;
        x = 2;
    }
}

int main() {
    A::f();
    printf("A::x: %d\n", A::x);
    printf("B::x: %d\n", B::x);
    printf("C::x: %d\n", C::x);
    return 0;
}

Program stdout
A::x: 1
B::x: 1
C::x: 2
```

About how the compiler itself can get names mixed up — and we have at least three compilers, so we will get at least three behaviors accordingly — in the next section.

P.S. I periodically torment people with this sophistry and the example above during interviews, simply to understand whether the candidate has encountered these things in production or not. Unfortunately, nearly 80% struggle to answer this seemingly simple question.

---

## Compilers Get Names Mixed Up Too

Names in source text are simply convenient labels for humans: variables, functions, types. But for the compiler a name is an entry point into a rather complex algorithm that must unambiguously determine *exactly what you meant*. And this is where things get interesting: the same name written in different contexts can mean completely different things, and sometimes mean nothing at all, depending on where and how it is used.

C++ is particularly tricky in this regard. The language has grown over decades, and name lookup rules have evolved alongside it: namespaces were added, then templates, ADL, two-phase lookup. All of this not only made the model more complex — it made it counterintuitive in places, even for experienced developers. Add to that the fact that different compilers have historically implemented these rules differently, in their own way, and some of these differences still surface in code.

There is no need to treat the compiler as a black box — although name lookup sometimes genuinely does look like magic — but if you break it down into individual steps, you can see that behind this "magic" lies a perfectly rigorous (if historically burdened) system of rules. I will try to explain it.

---

To understand how name lookup works, you first need to understand the system for describing the names we write in code. At the most basic level we have an identifier — essentially just a sequence of letters, digits, and underscores: speed, foo, or my_var.

When we use a template, its name is also an identifier, but when we add template arguments, for example my\<T, 1\>, we get not just a template name but a qualified template name, or template-id.

To unify these concepts, the term unqualified-id is introduced — an unqualified identifier. This is a general concept that includes not only ordinary identifiers but also more complex name forms, such as operators, destructors, or user-defined literals.

When we add the scope resolution operator ::, we get a qualified-id (qualified name), for example, std::vector, Foo::~Foo, and so forth. It is important to understand that in the expression Foo::bar the name bar is qualified, while Foo itself is the qualifier but is itself an unqualified name.

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/ee4/1d9/67c/ee41d967c74326feec5061a2adbca89a.png)

```
// Unqualified name 
// just an identifier

vector<int> v;      
// 'vector' — unqualified name

sort(v.begin(), v.end()); 
// 'sort' — unqualified name

// Qualified name (qualified-id) 
// with the :: operator

std::vector<int> v;       
// 'vector' — qualified, 
// 'std' — qualifier

std::sort(v.begin(), v.end());

struct Foo {
    ~Foo() {}
    static void bar() {}
};

Foo::bar();   
// 'bar' — qualified name
// 'Foo' — qualifier

Foo::~Foo();  
// '~Foo' — qualified name 
// of the destructor

// template-id — template name with explicit arguments
std::vector<int> v1;           // 'vector<int>' — template-id
std::pair<int, float> p;       // 'pair<int, float>' — template-id
```

The remaining key concept for name lookup is the terminal name. This is the last lexical name that the lookup algorithm ultimately tries to find. For example, in the expression obj->f the terminal name is f, and that is exactly what the compiler searches for after all the lookup stages.

```
// Terminal name - the last name the compiler looks up
obj->f();      // terminal name: 'f'

std::vector<int>::size_type x; // terminal name: 'size_type'

Foo::bar();    // terminal name: 'bar'

ns::Foo::bar();// terminal name: 'bar', qualifiers: 'ns', 'Foo'

// Where this matters: lookup in dependent contexts
template<typename T>
void wrapper(T& obj) {
    obj.size();      
    // 'size' - terminal unqualified name
    // compiler looks it up in type T at instantiation time

    T::value_type x; 
    // 'value_type' - terminal qualified name
    // without 'typename' the compiler may not understand this is a type
    
    typename T::value_type y; // this is more correct
}
```

Understanding the terms "identifier," "template-id," "qualified-id," and especially "terminal name" as the point where the entire lookup algorithm ultimately ends up is the key to understanding both the standard and compiler behavior in general. The general name lookup rule in C++ is not complicated, but it is convoluted — because it evolved in leaps from standard to standard, with vendors, large companies, and even well-known developers all contributing their share.

MSVC until 2017 did not support two-phase name lookup in compliance with the standard — when parsing a template, the compiler deferred lookup of all names until instantiation, whereas the standard required splitting the lookup into two phases as early as the template definition stage. GCC and Clang followed the standard more strictly; MSVC ignored this requirement for over 20 years.

Andrew Koenig proposed a modification to the name lookup rules for names associated with namespaces, which entered the standard under the name "Koenig lookup" and is officially enshrined in the standard as [basic.lookup.koenig], although in common usage it is more often called ADL — argument-dependent lookup.

The story of ADL is particularly interesting, because Koenig did not invent ADL — he merely formulated and pushed through a solution to an already existing problem. In the early 1990s the committee was adding namespaces to the standard draft, but it turned out that they conflicted with operator overloading. The question was how to make overloaded operators for user-defined types be found automatically, without explicitly specifying the namespace.

The specific pain point looked like this:

```
namespace sak {
    struct BigNum {
        int value;
    };

    // operator<< defined in namespace sak, next to the type
    std::ostream& operator<<(std::ostream& os, const BigNum& n) {
        return os << "BigNum(" << n.value << ")";
    }
}

int main() {
    sak::BigNum n(42);

    // ADL: compiler sees that n has type sak::BigNum,
    // looks for operator<< in namespace sak and finds it
    std::cout << n << "\n";

    // Without ADL one would have to write something like:
    sak::operator<<(std::cout, n) << "\n";

    // Or like this — but then the whole point of operator overloading is lost:
    // std::operator<<(std::cout, n); 
    // won't compile, since there's no such thing in std
}
```

Without ADL the compiler would report an error saying it cannot find operator\<\<, because the call does not explicitly specify that it resides in namespace sak. The problem was seen before Koenig — there was simply no solution. Bjarne Stroustrup described the problem in document P0262 "Name Space Management in C++" (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/1993/N0262.pdf) back in 1993, in Appendix D (Appendix D: Overloading and Namespaces), where you can see what the world looked like before ADL.

```
// explicit call defeats the whole purpose of operator overloading
mylib::operator<<(cout, s);

// or dragging operator<< into global scope via using
using mylib::operator<<;
cout << s;  // works now, but using must be written everywhere
```

ADL is often attributed to Andrew Koenig, even though he is not its inventor — but Koenig published document N0645 "Reconciling overloaded operators with namespaces" (https://www.open-std.org/jtc1/SC22/wg21/docs/papers/1995/N0645.pdf) in January 1995, setting out a concrete solution. His initial proposal applied only to overloaded operators, not to all functions. Koenig worked at AT&T Bell Labs, where by that time there was already an established practice of such lookup in the company's own work, and he more or less formalized and brought to the committee what was already being applied in practice. The alternative was to require explicit qualification everywhere — i.e., writing std::operator\<\<(std::cout, obj) instead of std::cout \<\< obj. This is technically correct but completely defeats the purpose of operator overloading and makes the code "dirty."

But ADL also brought side effects that the committee did not fully foresee: it made namespaces less strict and required explicit qualification where it would otherwise be unnecessary, and dependence on ADL can lead to semantic problems when two libraries expect different behavior from the same unqualified name.

At present the algorithm looks like this: in any scope, ordinary unqualified lookup is performed first, then lookup through base classes if applicable, and for function calls ADL is additionally launched to expand the set of candidates with names from the associated namespaces of the arguments. If a name is qualified, qualified lookup is performed in the specified namespace or class, without ADL and without ordinary unqualified lookup (we go bottom-up).

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/2fd/dbb/700/2fddbb700acc7778362616cbc3405b2f.png)

In template code, lookup is split into two phases: non-dependent names are looked up at the point of template definition, and dependent names at the point of instantiation — and it is in the second phase that ADL is activated for dependent names.

## GCC

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/692/a51/13d/692a5113d20812ddbd170b909715507a.png)

A historical peculiarity of GCC in template code manifested in the fact that the boundaries between the first and second phase of lookup were interpreted differently from Clang and from the standard's requirements, which meant that ADL for dependent names did not always fire at the right moment. Code that should have compiled might not compile, or conversely would compile where it should not have.

GCC implemented templates before the standard had fully formulated these rules, and historically used so-called "lazy parsing," in which the body of a template was not fully analyzed during the first parse but was deferred until instantiation. This meant that the first lookup phase was in essence not performed at all, and all names — including non-dependent ones — were looked up at instantiation time.

The practical consequence was as follows:

```
void foo(int) {}  // global foo

template<typename T>
void bar(T x) {
    foo(x);  // dependent name - looked up in phase 2 via ADL, correct
    foo(42); // NON-dependent name - should be looked up in phase 1
             // GCC: deferred to instantiation and found foo(int) - ok
             // Clang: looked up in phase 1 and found no matching foo - error
}

namespace myns {
    struct MyType {};
    void foo(MyType) {}
}

bar(myns::MyType{});
// GCC: compiled without issues
// Clang: error: use of undeclared identifier 'foo'
```

## Clang

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/415/25b/e0b/41525be0b4d933e2de92b98b67922633.png)

When two-phase lookup was formalized in C++98, the committee faced a conceptual question: what to do with names from a base class if the base class is itself a template?

If the base Base\<T\> is not fully known at the point of definition of Derived\<T\> and depends on the parameter T, then different specializations may contain completely different sets of members.

The committee decided that dependent base classes do not participate in the first phase of unqualified lookup, for the reason that if lookup in a dependent base were permitted in the first phase, adding a new member to a specialization of the base class could change the meaning of code in the derived class — violating the principle of template predictability (i.e., the behavior would depend on which arguments it is instantiated with).

```
// Why the committee forbade lookup in a dependent base in phase 1:
template<typename T>
struct Base {
    // the general version has no method process
};

// specialization for int - has process
template<>
struct Base<int> {
    void process() {}
};

template<typename T>
struct Derived : Base<T> {
    void run() {
        process();  // if lookup in the dependent base were allowed:
                    // for T=int - finds Base<int>::process, compiles
                    // for T=float - finds nothing and fails
                    // with identical template code we get different behavior
    }
};
```

## MSVC

![](https://habrastorage.org/r/w1560/getpro/habr/upload_files/80a/450/dc2/80a450dc24486138e6e5299ffb38c953.png)

Historically MSVC had non-standard behavior in name lookup within templates. Two-phase name lookup was long not implemented in conformance with the standard, and prior to VS 2017 with the /permissive- flag, dependent names in templates were looked up only at instantiation time, not at definition time — which produced a different set of candidates compared to GCC and Clang.

This is a real compatibility problem, well known in the C++ community. But delayed parsing, as it occurs in MSVC's case, is not merely a choice of name lookup algorithm — it is the vendor's architectural decision about when the compiler looks inside a template body at all. Changing this behavior means rewriting approximately 80% of the parser.

The MSVC team publicly acknowledged that the old parser physically could not implement two-phase lookup, which is why a new parser had to be developed — a process that took several years and in effect meant replacing one of the central components of the MSVC compiler.

If you are curious why Microsoft did not implement a correct parser from the start given the existence of the standard, that question leads us back into the company's history — and oddly enough, to the roots of a UI library. The MFC (Microsoft Foundation Classes) library underpinned the entire Windows application development stack in the 1990s, and some of the particularities of its implementation and features influenced the compiler, adding quirks that were in turn actively exploited in MFC. This created a mutual reinforcement that prevented MSVC from ever becoming truly standard, because it would have broken MFC, and MFC could not become portable because it relied on a non-standard MSVC. But inside Microsoft this coupling was politically protected, because MFC was the foundation of the Windows developer ecosystem.

This is a classic story of technical debt at platform scale, which grew to company scale and began to affect the products being shipped — a once quick and pragmatic decision from 1993 had become an architectural constraint for 25 years that could not be fixed without simultaneously rewriting the parser, patching thousands of Windows SDK components, and fixing MFC, without breaking backward compatibility with code written by hundreds of thousands of developers.

```
template <typename T>
void call_foo(T x) {
    foo(x);   // dependent, but MSVC handled this its own way
}

void foo(int x) { }   // declared AFTER the template

int main() {
    call_foo(42);     
    // GCC: error - foo is not visible at the point of template definition
    // MSVC: ok - finds foo at instantiation
}
```

By the standard, foo(x) is a dependent name (depends on T) and therefore should be looked up at instantiation — but only via ADL. However, foo(int) is not found via ADL for int (there is no associated namespace), so GCC correctly gives an error, while MSVC simply looked up everything at instantiation and found it.

## What's Next...

As I said in the first article, name lookup in C++ is still that part of the language that seems obvious right up until you start looking at the details. Behind familiar `foo`, `bar`, and `vector` lies not just string matching, but a complex and in places historically peculiar system of rules, compromises, and extensions that appeared as the language developed.

I have tried to cover the basic building blocks of this system: identifiers, how unqualified-id and qualified-id are formed from them, where template-id appears, and why it makes sense to single out the "terminal name" as the point where the entire lookup algorithm ultimately ends up. These concepts do not answer all questions by themselves, but without them it is impossible to understand ADL, two-phase lookup, or template behavior.

I hope that after these articles you understand why there is no single "name lookup algorithm" in C++ that simply walks through steps one by one — but there are several different mechanisms that kick in depending on the form of the name and the context.

If you keep this model in mind as a system of several overlapping mechanisms, many C++ "oddities" start to look quite "logical." And from this vantage point you can already move forward and examine specific cases where these rules intersect and begin to behave non-obviously. In the next article I will cover the most basic mechanism — unqualified lookup — and see how even it alone is capable of creating some rather non-trivial situations.
