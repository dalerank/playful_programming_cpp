# Procedures

In one of the bugs I had to fix, the tooltip over a unit sometimes showed garbage instead of a name, but the garbage was meaningful: scraps of another unit's name that the cursor had hovered over just before. The culprit was a small function that glued name and level together locally and handed out a `const char*`. In a debug build it worked, but in release it sometimes broke.

```cpp
struct Unit { const char *name; int level; };

// do not do this, we will unpack why below
const char *unit_label(const Unit &u) {
    char buf[64];
    std::snprintf(buf, sizeof buf, "%s [%d]", u.name, u.level);
    return buf;   // the address lives shorter than the pointer to it
}
```

The mistake was not in `snprintf` and not in the buffer size. You have to remember that a procedure is not only code, and every call has its own place in memory and its own locals, and that place should belong to only one call. "Should" does not mean "must", and the rewritten bytes after return do not vanish, they simply become nobody's, and any later call is free to lay its own data over them, so the garbage in the string was not random.

When we move from values and objects to program behavior and to the question of what makes a program a program at all, another fundamental category walks onto the stage, the procedure. In familiar `C++` that is just a function, but in a broader sense a procedure is a sequence of rules over objects and values.

Often it changes state, creates or destroys objects, but there is also a "functional" procedure that only reads inputs and returns a result, leaving no side effects. Such functions are called pure, and here it is worth mentioning a common misconception about `constexpr`, which does not promise purity. It constrains what is allowed during compile-time evaluation, and a `constexpr` function called at runtime is an ordinary function that since `C++14` can use loops and mutation, and since `C++20` even allocations. Right now we care about the big picture of what a procedure does, that is the call, the stack, the conventions, and which objects it touches.

## In it goes and out it comes, remarkably well... (c) Eeyore

Historically compilers started with a very primitive understanding of procedures, and all the "magic" was generating entry and exit correctly, prologue and epilogue, saving registers, organizing the stack. And the stack was not always there: in early Fortran, before the 77 standard, recursion was not supported, so the compiler could lay out locals and the return address statically, once for the whole program. No separate frame per call appeared. Early C already had a stack, but not yet prototypes, meaning how many arguments and of which types you passed the compiler neither knew nor checked, and the callee pulled them off the stack by convention. From that grow both `printf` and stack cleanup of arguments in C.

```text
int sum(int a, int b) {
    return a + b;
}

int main() {
    return sum(2, 3);
}

; simplified, 16-bit, everything goes through the stack
push 3          ; argument b
push 2          ; argument a
call sum
add  sp, 4      ; cdecl: the caller pops the arguments

sum:
    push bp
    mov  bp, sp     ; [bp] old bp, [bp+2] return address
    mov  ax, [bp+4] ; a
    add  ax, [bp+6] ; b
    pop  bp
    ret             ; result in ax
```



## A frame that lives one call

The call stack itself is a region of memory that grows and shrinks as the program enters functions and leaves them, and every function call creates a new frame on the stack, a stack frame or activation record, that holds everything needed to run that particular instance of the function: locals, arguments, return address, saved register values, and possibly extra bookkeeping like a pointer to the caller's frame or padding for alignment.

The key phrase is "that particular instance", because the frame does not belong to the function, it belongs to the call. You leave the function, the stack pointer moves back, and that same memory is handed to the next caller without clearing: there is the whole riddle of the unit tooltip.

The buffer `buf` sat in `unit_label`'s frame, the frame ended on `ret`, and the pointer outward kept living. But while nothing was called between the return and the string read, the old bytes still sat at that address, so in a debug build everything "worked".

Once another call landed in that region of memory, it laid its own locals on the same spot, and since the next call was most often the same `unit_label` for a neighboring unit, scraps of someone else's name crawled out of the tooltip. Formally this is undefined behavior, and arguing about what "will happen" is not worth it, because there are no guarantees here, including the guarantee that it will not crash the game at all.

```text
===================================================================
STACK FRAME REUSE AND DANGLING STACK POINTERS
===================================================================

Step 1: Calling unit_label(Unit{"Archer", 5})
  RSP shifted down, local buffer initialized in the frame:

  Stack memory (address 0x7FFF00):
  ┌─────────────────────────────────────────────────────────────┐
  │ "Archer [5]\0"                                              │  <── buf[64]
  └─────────────────────────────────────────────────────────────┘
  ▲
  │ returned pointer ptr = 0x7FFF00

Step 2: Returning from unit_label (RET instruction)
  Frame ends, RSP moves back up, memory is NOT zeroed:

  Stack memory (RSP moved above):
  [ 0x7FFF00: "Archer [5]\0" ]  <── Memory is considered free,
  ▲                                 but stale bytes remain
  │ ptr (dangling pointer!) still points to 0x7FFF00

Step 3: Next call to render_tooltip() or unit_label(Unit{"Mage", 12})
  The new call lays its own stack frame over the exact same address:

  Stack memory (0x7FFF00 overwritten by new call):
  ┌─────────────────────────────────────────────────────────────┐
  │ "Mage [12]\0"                                               │
  └─────────────────────────────────────────────────────────────┘
  ▲
  │ Old ptr reads 0x7FFF00:
  │ Instead of "Archer [5]", tooltip reads parts of "Mage [12]" or garbage!
```

The "call creates a frame" model is a good starting point, but today it is more of an approximation, because a leaf function often has enough registers and the red zone (on System V AMD64 that is 128 bytes below the stack pointer that you may use without moving the pointer itself), and compilers throw away the frame pointer at any optimization level and address locals straight from the stack pointer. The most radical case, when there is no frame at all because there is no call either, we will unpack separately.

```text
===================================================================
RED ZONE IN SYSTEM V AMD64 (LINUX / macOS x64)
===================================================================

Stack grows downward (toward lower addresses):

  Address:
    ▲
    │  ┌────────────────────────────────────────────────────────┐
    │  │ Caller Stack Frame                                     │
    │  ├────────────────────────────────────────────────────────┤
    │  │ Return Address (RIP)                                   │
    │  └────────────────────────────────────────────────────────┘
    │  ◄── Current Stack Pointer (RSP)
    │
    │  ┌ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┐
    │  │ RED ZONE (128 bytes below RSP):                        │
    │  │                                                        │
    │  │  • Leaf functions use these 128 bytes for locals       │
    │  │    without modifying RSP (saving 'sub/add rsp, N')     │
    │  │                                                        │
    │  │  • OS interrupts and signal handlers are guaranteed    │
    │  │    not to touch this space (allocating below RSP-128)  │
    │  │                                                        │
    │  │  • NOTE: If the function calls another function (CALL),│
    │  │    the Red Zone cannot be used                         │
    │  └ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┘
    ▼  ◄── RSP - 128 (boundary of protected zone)
```

## How many frames fit on the stack

Since a frame takes space, you need to know how many will fit. Walking a scene tree, descending a BSP, flood-filling neighboring cells, pathfinding on a graph, all of that is usually written recursively, and recursion depth depends not on the code but on the data: however many nesting levels the artist put into the prefab, that many frames end up on the stack. So frame size is worth not estimating by eye, but measuring. You can do it from the program itself: take the address of a local on two adjacent recursion levels and subtract one from the other.

```cpp
#include <cstdio>

int walk(int depth, const char *parent)
{
    char scratch[64];
    scratch[0] = (char)depth;
    if (parent)                       // print, do not assert
        std::printf("frame %d: %td bytes\n", depth, parent - scratch);
    if (depth == 4)
        return scratch[0];
    return walk(depth + 1, scratch);
}

int main()
{
    return walk(0, nullptr);
}
```

On MSVC under x64 this program prints the same number at every level: `128 bytes`. Of those, only 64 are ours; the rest is the return address, 16-byte frame alignment, and the mandatory 32 bytes of shadow space that the Microsoft x64 ABI requires reserved for the first four register arguments. Another compiler on another platform will have its own layout, but the order is the same: a modest function with one 64-byte array costs twice what the source suggests.

Next we count on fingers. Take a thread stack of one megabyte for easy arithmetic, a megabyte is 1048576 bytes, divide by 128 and get 8192 frames. What remains is to check the arithmetic with the same recursion, only without an exit condition.

```cpp
#include <cstdio>

// this program is supposed to crash, that is the point
int walk(int depth)
{
    char scratch[64];
    scratch[0] = (char)depth;
    if (depth % 16 == 0) {
        std::printf("%d\n", depth);
        std::fflush(stdout);
    }
    return walk(depth + 1) + scratch[0];
}

int main()
{
    return walk(1);
}
```

It dies with code `0xC00000FD`, that is a stack overflow, right after printing `8000`. The count promised 8192; the difference is only one hundred ninety-two frames, or about twenty-four kilobytes. They went to process startup, runtime library frames, and the guard page at the end of the stack. For a napkin estimate that is excellent accuracy, and the main thing is we now have a number, and we know it is not "will crash someday" but "will crash at eight thousand".

Eight thousand is a lot for a scene tree and very little for a flood fill, where depth easily equals the cell count. And that is still the generous case, because megabytes go to the main thread, while worker threads get their stack size at creation, and there people often set 64 or 128 kilobytes, so the same recursion dies already at five hundred frames.

And fibers and coroutines that task systems run on live with even smaller stacks. Hence the rule worth keeping in mind: if traversal depth depends on content, not on code, better unfold the recursion into a loop with your own stack on the heap, because there an overflow is a check or an exception, not instant process death with no chance to write anything to a log.

Two caveats without which the measurement would be incomplete. The difference of addresses of two unrelated objects is undefined by the standard, so we print it rather than `assert` it, and you have to look at the concrete compiler. The optimizer is free to cancel all this machinery, unroll recursion into a loop, or turn the call into a tail call, and then there is nothing to measure. With `/O2` my frame stayed the same, 128 bytes, because `printf` inside prevented both collapsing the recursion and discarding the frame, but you cannot rely on that.

## Calling conventions

When the compiler generates code for a function call, the first thing it must decide is how to pass arguments. That is where calling conventions come in, which specify whether the first arguments go in registers and the tail on the stack, or (in older schemes) almost everything ends up on the stack. Registers are usually faster, and "everything through the stack" is simpler to implement, so in old schemes there was little choice, and the 8086 had four general-purpose registers in total, each with its own special duties.

And the absence of prototypes together with variable-argument functions demanded a uniform stack layout by which the callee could find arguments without knowing how many there were in advance. Historically architectures and compilers chose different strategies, and today on x86-64 the defaults like System V AMD64 or the Microsoft x64 ABI lean on registers.

```text
; simplified: MSVC __fastcall on 32-bit x86, first two integers in ecx and edx
mov ecx, 2      ; a
mov edx, 3      ; b
call sumreg

sumreg:
    mov eax, ecx
    add eax, edx
    ret
```



## Prologue and epilogue

The prologue runs right after entering the function and before its main logic, and its job is to set up the environment for work: save the values of those registers the function plans to use but which by convention must be restored before return (callee-saved registers). Allocate space for locals, set a special register that will point at a fixed spot inside the frame and let you address locals and arguments at constant offsets regardless of how the stack pointer moves while the function runs.

In classic x86 the prologue looked like saving the old base pointer and setting a new one to the current top of stack, and that sequence became so standard that x86 got a separate `enter` instruction doing it all in one. Little good came of it, because it is microcoded and ran slower than the same two ordinary instructions, so compilers almost never emit it, while its twin `leave` took hold in epilogues and shows up constantly.

Allocating stack space for locals in the prologue is not just subtracting a number from the stack pointer, it is also solving an optimal-placement problem, where the compiler must decide in what order to lay out variables, how to align them, and whether the same memory region can be reused for different variables that are not live at the same time, which saves space in the stack frame.

Early compilers solved this very straightforwardly, and each variable got its own fixed slot, and the frame size was the sum of all locals plus alignment, with almost no reuse. Later, graph-coloring algorithms and relatives became classic for registers, where variables with overlapping lifetimes cannot share one register. A similar idea applies to stack slots, and if two locals do not "overlap" in lifetime, they can share the same frame cell and save a couple of bytes. That also kills any hope of guessing what will sit at the address of the dead `buf` from our tooltip, because the cell is reused not only by later calls but by neighboring variables inside one call.

## Who cleans up after the call

The function epilogue does the reverse, restoring the stack pointer to the state it had before the call and loading the saved values back into registers. Then it executes the return instruction, which pops the return address off the stack and transfers control back to the caller, and here subtleties appear again, because different calling conventions split responsibility for cleaning arguments off the stack differently.

In cdecl the caller does it after return, which lets you support variable-argument functions but requires generating extra code, whereas in stdcall the callee cleans up before return, which saves code size but does not mix with variable arguments at all, because the callee simply does not know how many bytes to pop. That is why the entire Win32 API is declared stdcall, while `printf` in the same codebase stays cdecl. Exactly that difference produced the classic compatibility problems when a library built under one convention was called from code expecting another, the stack got corrupted, and the crash could surface far from the wrong call site.

## And now forget all of that

Modern compilers have moved far beyond the simple model of "allocate space, call the function, free space" toward yet another system with its own theory and optimizations. And the most annoying part for what you just read is that the compiler gladly throws all that machinery away entirely. Once it can see the callee's body, instead of a call it substitutes the body itself, and then neither frame, nor prologue, nor epilogue, nor calling convention for that function exists, and it may not appear in the object file at all.

```cpp
int armor_of(int level) { return 3 * level + 2; }

int damage_after_armor(int dmg, int level) {
    return dmg - armor_of(level);   // there will be no call here
}
```

The keyword `inline`, despite the name, has almost nothing to do with that decision. It is about the one-definition rule and allows putting a function body in a header included ten times, while whether to substitute or not the compiler decides on its own heuristics and does not ask your opinion. But it absolutely needs to see the body, and that part is entirely in your hands, because a body in a header is visible to everyone, a body in a separate `cpp` is visible to no one outside the file unless link-time optimization is on (`/GL` together with `/LTCG` on MSVC, `-flto` on GCC and Clang). Splitting a project into files, usually discussed as a build-time question, also decides what the optimizer will see at all.

Saved `call` and `ret` alone are not the main point here; those are a few cycles and a well-predicted branch. The main point is that after substitution the body sits inside the caller's context, and every other optimization piles onto it: the argument was a constant, so a constant sprouts inside the body, so a zero check vanishes as always false, so a whole branch dies next, and two neighboring stores merge into one. One inline often opens a chain of five transforms, and the speedup comes from those, not from the skipped jump instruction.

There is a flip side too, and the substituted body is copied to every call site, code bloats, the instruction cache starts missing, and on large functions the win goes negative, so heuristics are usually conservative, and `__forceinline` and `[[gnu::always_inline]]` can make things worse than they were.

Another special case is the tail call. If the call is the last thing in the function, the compiler may replace `call` and `ret` with an ordinary `jmp`, reusing the current frame, but the `C++` standard promises this nowhere, so deep recursion on tail calls can live for years in release and crash in a debug build. Clang has the nonstandard `[[clang::musttail]]` for this; everyone else is left hoping it eventually lands in the standard.

Inline is also useful to remember when you look at bugs. It is why frames vanish from the call stack in a release build, why the profiler confidently attributes time to a function that in the source does nothing of the sort, and why the debugger jumps across lines in unexpected order. To check what actually happened to a concrete call, you can only look at the assembly.

And for us the important bit is this. When there is no call, all the agreements from the first half of the chapter zero out: registers, stack, who cleans up after whom, all of that ceases to exist. What does not zero out is the contract about which objects the procedure reads, which it writes, and who else has access to them. That is what the optimizer leans on when it decides what can be substituted, folded, and discarded, and that is what the second half of the chapter is about.

## Passing arguments



As languages and compilers matured it became clear that the key question here is not so much syntax as how the procedure interacts with objects, that is which it reads, which it modifies, which live only for the call, and which survive many calls and even the whole lifetime of the program. It is convenient to look at this on two axes. The first is *where the object lives* relative to the call (argument or result, local, global, the procedure's own static state). The second, below, is *what role* the object plays in the call (pure input, pure output, input/output). We start with "where". Arguments and results are passed into and out of a procedure either directly or indirectly.

*Directly* is when you write `int f(int x)` and the compiler passes the value of `x` in registers or on the stack, then returns the result, say also in a register. *Indirectly* is when you pass a pointer or a reference, like `void increment(int* p)`, and the procedure works with the object not as a copy of a value but as the original in memory. The boundary between these two cases does not run where the source makes it look, because passing a large class by value at the ABI level usually turns into a hidden pointer to a copy the caller made, and returning a large object also goes through a hidden out-parameter, not through a register.

## Where state lives

Local state is temporary objects created at the start of a procedure call, living on the stack or in registers, shaped to the algorithm's needs and destroyed when the procedure finishes. If in `void foo()` you declare `int tmp = 0; std::vector<int> v(10);`, then both `tmp` and `v` are part of that procedure's local state; it needs them for work, but nobody outside knows about them and nobody should. Only "lives on the stack" here has to be read carefully, because on the stack sits `v` itself, that is its couple or three of pointers, while the ten numbers it owns sit on the heap. Locality here is about lifetime, not about which memory the data ended up in, because `v` is destroyed on leaving the function, and that is when the heap is freed with it.

Separately there is global state, objects that not only this procedure but others can access, and across many calls. That includes global variables, static fields, or objects in dynamic memory that different parts of the program hold references to.

And finally, a procedure's own state is objects available only to it (and maybe to tightly related functions), but that persist between calls. The simplest example is a `static` variable inside a function, and almost every engine has an id issuer roughly like this

```cpp
int next_entity_id() {
    static int last = 0;
    return ++last;
}
```

and then `last` lives as long as the program does, but only `next_entity_id` can use it; for everyone else it as good as does not exist, which demonstrates the typical pattern of a procedure's "hidden" state that appeared in old C long before classes.

That is why such counters love to break when you load a new match in the same process and the identifiers keep growing from the old ones, and a reference to an entity from the previous fight, saved somewhere, suddenly points at a live unit. A procedure's hidden state is a global variable with good manners, because it is invisible from outside, but its problems are the same, and someone still has to reset it.

## A ruble in, two out

To talk more precisely about a procedure's behavior we need one more split, namely which objects count as its inputs, which as outputs, and which as both. If a procedure only reads an object's value, never changing it, that object is a pure input for it. If the procedure creates an object, writes data into it, or destroys it without looking at its previous contents, that object acts as an output, and its initial state does not matter, only the new one does.

```cpp
#include <cstddef>
#include <cstdio>

// pure input: count_alive only reads the hitpoint array
std::size_t count_alive(const int *hp, std::size_t n) {
    std::size_t alive = 0;
    for (std::size_t i = 0; i < n; i++)
        if (hp[i] > 0) alive++;
    return alive;
}

void pure_in() {
    int hp[] = {100, 0, 35, 0, 12};
    std::printf("%zu %zu\n", count_alive(hp, 5), count_alive(hp, 5));
}

int main() {
    pure_in();
}
```

A pure input is also a promise to the compiler that it gladly leans on. If it can see `count_alive`'s body, because it sits in a header or was built under link-time optimization, or the function is marked pure, then it may compute it once and reuse the result for the second call, because nobody changed the array between the calls. With a function from a foreign object file it cannot do that; there it must call it twice, because it does not know what is inside.

A per-frame damage buffer, by contrast, is a pure output: at the start of the frame it is wiped entirely, and what it held from the previous frame interests nobody.

```cpp
#include <cstddef>
#include <cstdio>

// pure output: previous buffer contents are not read
void reset_damage(int *damage, std::size_t n) {
    for (std::size_t i = 0; i < n; i++)
        damage[i] = 0;           // write only, no read
}

void pure_out() {
    int damage[8];
    damage[0] = 42;   // dead store
    reset_damage(damage, 8);
    std::printf("%d\n", damage[0]);
}

int main() {
    pure_out();
}
```

If the compiler can see `reset_damage`'s body, it may remove the store `damage[0] = 42` entirely as dead, and through an unknown TU or a DLL you cannot count on that. The program's output will show no difference either way, it is zero in any case, so you have to check such things in the assembly.

The most interesting case is when an object is both read and modified, for example a counter passed by reference that the function increments, or an array element that is first checked against some condition and then updated. Such objects act as input/output of the procedure, and they are most often the source of complex effects and bugs when the programmer does not fully account for who in the program has the right to change their state.

```cpp
// per-frame damage counter: read first, then write
void add_damage(int *stat, int amount) {
    *stat += amount;
}

// hitpoints: old value is read, new value is written, order matters
void clamp_hp(int *hp, int max_hp) {
    if (*hp > max_hp)
        *hp = max_hp;
}
```



## Who may change memory

While `add_damage` is called from one thread, everything is boring and correct. But area damage is computed by workers, because an explosion hits a hundred units, and then several threads arrive at the same counter. `*stat += amount` is not one operation but three: read, add, write, and between the read and the write another thread can slip in with its own read of the same old value. One of the two updates vanishes, the counter shows less than was dealt, and it usually surfaces in a bug report.

Reasoning about intermediate values in such code is pointless, because a data race on a non-atomic object is undefined behavior, not an "imprecise result". The compiler is allowed to assume there is no race, which means it may fuse two consecutive `+=` into one add, or keep the counter in a register for the whole loop and write to memory once at the very end.

The question "what will another thread see in the middle" simply has no answer, because in the language model there is no other thread there. An intermediate state becomes observable only where you explicitly asked for it, for example with `std::atomic<int>`, where `+=` is indivisible.

The usual and cheapest fix in games is to give each worker its own counter and sum them into one at the end of the frame. You get zero synchronization, no race by construction, and the ordering question disappears too. If a shared counter is still necessary, you take an atomic, but then remember that several threads poking one atomic variable will fight over one cache line, and that can cost more than the damage calculation itself.

## Why the compiler fears your pointer

With the next example the story is less obvious. It seems the optimizer should unroll and vectorize a loop that adds an array into an accumulator, because integer addition is associative and the summands can be rearranged. With floating point, by the way, you cannot: there the order affects the result, and reassociation is enabled only by an explicit permission like `-ffast-math`.

```cpp
#include <cstddef>

// input/output in pure form: *total is read and written every iteration
void sum_damage(const int *damage, std::size_t n, int *total) {
    for (std::size_t i = 0; i < n; i++)
        *total += damage[i];
}

// the same algorithm, but the accumulator is local and overlaps nothing
void sum_damage_local(const int *damage, std::size_t n, int *total) {
    int acc = *total;
    for (std::size_t i = 0; i < n; i++)
        acc += damage[i];
    *total = acc;
}
```

What gets in the way is something else entirely. The compiler does not know where `total` points, and from its point of view it may point into `damage`, and even `const` on the first parameter forbids nothing, because that is a promise not to write through *this* pointer, not a promise that nobody else writes to the same addresses.

So each iteration is allowed to change what the loop plans to read on the next, and what remains is either to reread `*total` from memory on every step, or to emit two versions of the loop and check for overlap before entry.

A local accumulator removes the question, because `acc` has no address anyone could obtain and change, and it also cannot overlap `damage`, and the loop becomes what we wanted. Promising the compiler no overlap directly in `C++` is impossible: there is no `restrict` keyword in the standard, it exists in C, and here only extensions live, `__restrict` on MSVC and `__restrict__` on GCC and Clang.

In early compilers analysis of these categories was almost entirely on the programmer's conscience and the compiler simply took the code as written, but modern optimizers actively analyze which variables are only read, which are written, and where side effects are possible, and on that basis decide which calls can be changed (reordering, CSE, DCE) and which transforms are allowed.

## What to do about it

The first rule you have probably already absorbed from the start of the chapter. If a procedure built something on its own frame, it goes out by value, not by address. `std::string` instead of `const char*`, `std::array` instead of a pointer to a local array, or let the caller pass a buffer you can write into. Do not fear the copy, because a large object is returned through a hidden pointer to the caller's memory anyway, so the "expensive return by value" here is usually fictional.

The second rule is about roles. An object's role in the call should be written in the signature, not kept in your head, via a `const` reference or `const` pointer for a pure input, an ordinary reference for output and input-output, and better still via a return value instead of an out-parameter when the object is created wholly inside. This is not about code beauty; it is the only way to let the compiler do its job correctly.

The third rule is about hidden state. Any `static` variable inside a function is a global variable that simply has no name outside, and it will survive loading a new map, a second world in the same process, and a call from another thread. If state must live between calls, better pass it explicitly, as a context object.

And overall the picture should come together like this: a procedure is code plus a frame plus a contract about who reads and writes what. The frame dies on `ret`, and pointers die with it. The contract is written nowhere except the signature and your intentions, and the compiler optimizes everything it can reach. While the rules above are followed, everything works correctly, but as soon as you break any of them, be ready for someone else's name to appear in the tooltip over a unit.
