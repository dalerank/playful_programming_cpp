# Computational Basis

Most teams that ship multiplayer strategy games eventually hit simulation determinism bugs, where every client must get not "roughly the same" result but a bit-for-bit identical one. With `float` that is not always possible because some operations are not reproducible: addition, multiplication, division, and square root under IEEE 754 must round exactly, and those are repeatable, while `sin`, `cos`, `exp`, and a pile of other algorithms and functions are not standardized, so across libraries, compiler versions, and even flags like `/fp:fast` they can differ in the low bits. That is why teams end up writing their own fixed-point type, their own operations, their own sines and roots. Some operations come for free (addition and comparison); with others you have to think about the implementation, or do it "the blunt way" and get fooled by how easy the naive version looks.

```cpp
// 16.16, deterministic arithmetic for replays
struct Fixed {
    int32_t raw;
};

// division the blunt way: subtract the divisor while there is something to subtract
// (and this is still only for positives; with negatives it gets more fun)
Fixed divide(Fixed a, Fixed b) {
    int64_t rem = (int64_t)a.raw << 16;
    int64_t d = b.raw;
    int32_t q = 0;
    while (rem >= d) {
        rem -= d;
        ++q;
    }
    return Fixed{q};
}
```

It will work, and work correctly, until one day you notice that with five hundred projectiles the profiler shows this `divide` in the hot functions, because by repeating a simple division model from around 1952 you chose a very expensive way. Which operations count as primitive and which as derived is what a computational basis is, and below we will look at how processor architects have done this for the last eighty years and how you should choose yours when you write your own `Fixed`, `BigInt`, or `Vec3`.

To perform actions on a type, you need to understand which operations the type supports at all. For any value type you can define some minimal set of procedures from which every other operation on it can be built, and that set is called the type's computational basis. For unsigned `k`-bit integers, for example, you can take "get zero", "check equality", and "go to the next value", and in theory from those you can implement addition, multiplication, and comparison, just as very long sequences of "next" calls and checks.

## Why three operations are not enough

Here efficiency enters the picture, and a basis is called more efficient if typical procedures over the type come out no worse in asymptotic cost than on another reasonable basis with more "direct" primitives. In the example below with only `next`, everything is bad, and adding two `k`-bit numbers by repeated increment takes on the order of `2^k` steps in the worst case, whereas a hardware `ADD` for a fixed width is usually constant in the argument values, because the cycle count is set by the microarchitecture, not by the magnitude of the addends. Let us look at that basis in full.

```cpp
// ---- minimal basis: theoretically sufficient ----
namespace MinimalBase {
    using uint = unsigned int;

    // three primitives
    uint zero() { return 0; }
    bool equal(uint a, uint b) { return a == b; }
    uint next(uint a) { return a + 1; }

    // addition by repeated increment: O(b) steps
    uint add(uint a, uint b) {
        uint result = a;
        for (uint i = zero(); !equal(i, b); i = next(i))
            result = next(result);
        return result;
    }

    // multiplication by repeated addition: O(a*b) steps
    uint multiply(uint a, uint b) {
        uint result = zero();
        for (uint i = zero(); !equal(i, a); i = next(i))
            result = add(result, b);
        return result;
    }

    // comparison: a < b if, climbing from zero toward b, we meet a
    // before we hit b. O(b) calls to next, no prev and no subtraction
    bool less(uint a, uint b) {
        for (uint i = zero(); !equal(i, b); i = next(i))
            if (equal(i, a))
                return true;
        return false;
    }
}

int main() {
    assert(MinimalBase::add(2, 3) == 5);
    assert(MinimalBase::multiply(4, 5) == 20);
    assert(MinimalBase::less(3, 7));
    assert(!MinimalBase::less(7, 3));
}
```

The word "inefficient" is confirmed by such a basis quickly enough, and `multiply(1000, 2000)` yields two million calls to `next`, and a single such call in a debug build takes 5.5 milliseconds, so there is no talk of any real use when multiplication eats a third of a frame. Hardware multiplication in the same run costs 0.5 nanoseconds, a ratio of about ten million to one.

But if you build the program with optimizations, it finishes in 0.4 microseconds, because the optimizer sees ordinary addition in the inner increment loop and folds it, leaving a thousand iterations of the outer loop out of two million steps. It is tempting to stop there, but that only happened because of the simple data type `unsigned`, whose operations the compiler can see entirely. As soon as `unsigned` is replaced by something nontrivial with a range check, state, or a table lookup, there will be nothing to fold, and you will get milliseconds again. That is what makes a naive basis dangerous: on a toy type the optimizer hides its problems, and on a real one it no longer can.

## Why seven operations are not enough either

But even if your computational basis is efficient, that is still not enough, and the second important property of a basis is its expressiveness. A basis is called expressive if it lets you define the procedures over the type that regularly show up in practice in a convenient and compact way. You can, of course, say that subtracting two arbitrary numbers is a luxury, since it is expressed by adding the opposite, and for unsigned numbers there simply is no opposite. But if you try to write real code with only `+`, `0`, and `next`, you will find that many definitions become bulky and unreadable, so when we design the expressiveness of a basis we add operations that are logically derivable from others, yet the practical benefit of having them explicitly is much higher than the cost of growing the basis.


| Basis         | What we added                                                          | What we pay and what we get                                                                                                |
| ------------- | ---------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------- |
| minimal       | `zero`, `equal`, `next`                                                | you can express everything, but addition is linear in the value and multiplication is quadratic                            |
| efficient     | `add`, `subtract`, `multiply`, `less`                                  | operation cost no longer depends on argument values, but there are twice as many primitives                                |
| expressive    | `not_equal`, `less_equal`, `greater`, `divide`, `modulo`, `min`, `max` | nothing new is computed, but the code becomes clearer, and in `C++20` the whole comparison group is derived from one `<=>` |


It is easy to dismiss the last level as cosmetics, but that kind of cosmetics already raises human "efficiency". Take the `Fixed` from the opening of the chapter, which has no comparisons, and the same type with one added line.

```cpp
struct Fixed {                        // that same type from the opening of the chapter
    int32_t raw;
};

bool less_raw(Fixed a, Fixed b) { return a.raw < b.raw; }

struct FixedOrdered {                 // the same plus one line
    int32_t raw;
    auto operator<=>(const FixedOrdered &) const = default;
};

static_assert(!std::totally_ordered<Fixed>);
static_assert(std::totally_ordered<FixedOrdered>);

void sort_bare(std::vector<Fixed> &v) {
    std::sort(v.begin(), v.end(),
              [](Fixed a, Fixed b) { return less_raw(a, b); });   // a lambda at every call site
}

void sort_ordered(std::vector<FixedOrdered> &v) {
    std::sort(v.begin(), v.end());                            // better
}
```

With the first variant neither `std::clamp`, nor `std::min`, nor `std::set` works, and every algorithm call has to be accompanied by a lambda that is easy to write differently and end up with a sort inconsistent with a search. With the second, all of that is available immediately, you can check it with a concept instead of promising it in comments, and expressiveness turns out to be about how your type will interact with the standard library and with other people's code. That is how it went with the `C++` standard, and the language could formally have stayed minimalist with operators like `-`, `*`, `/`, and checks `<` and `>`, and for a long time the committee hesitated to introduce new operations in order to keep compatibility. But the time came, and in `C++20` they had to do the work of improving expressiveness, and instead of six comparison operations they put one into the basis, the three-way comparison `<=>`, from which `<`, `>`, `<=`, and `>=` are derived, while `==` is declared alongside automatically by the standard. Now it is enough to write one primitive instead of six derived ones and forget about a good half of the bugs where someone forgot to keep `<` consistent with `==`.

## How the basis grew in hardware

### 1940s and 1950s: the minimal basis

ENIAC (1945) had no stored program, and "instructions" were set by plugboards, while basic arithmetic ran through decimal ring-counter accumulators. Separate units handled multiplication, division, and square-root extraction, so even where there was not yet a program, the expensive operations were already buried in hardware. The first stored-program machine was the Manchester Baby (1948), and its basis was very modest, only seven instructions, and of arithmetic there was only subtraction, so the programmer had to assemble addition by hand, and it looked roughly like this.

```cpp
// on the Baby the only arithmetic was subtraction
unsigned int add_via_sub(unsigned int a, unsigned int b) {
    return a - (0u - b);   // the same as a + b
}
```

EDSAC (1949) and the Manchester Mark 1 (1949) already multiplied in hardware. The IBM 701 (1952), IBM's first commercial scientific computer, had hardware multiplication and division that took tens of machine cycles. Against addition in a handful of cycles that still looked like a strong expansion of the computational basis, and division was implemented by successive subtraction, only with a shift on each digit rather than by ones as in the example from the opening of the chapter.

### Late 1950s and 1960s: floating point and addressing modes

The IBM 704 (1954), the first mass-produced computer with hardware floating point and index registers, which let you address arrays with a loop instead of patching the address by hand. It turned out that if the basis has no indexed addressing, you cannot take `a[i]`, and you can only write the address into the instruction itself, so programs of that era rewrote their own code on the fly.

```text
; hypothetical assembly, machine without index registers
      LOAD  BASE       ; address of the start of the array
      ADD   I          ; added the element index
      STORE NEXT+1     ; wrote the sum into the address field of the next instruction
NEXT: LOAD  0          ; that zero was just overwritten; a[i] will be here

; the same fetch on a machine with an index register
      LXA   I, 1       ; element index into index register 1
      CLA   BASE, 1    ; took a[i]; the instruction itself did not change
```

With an index register your code stays unchanged and can be placed in read-only memory or shared among processes. Today what remains of that whole story is `a[i]`, behind which sits one hardware instruction with base-plus-index addressing.

> IBM System/360 (1964) brought unification of the product line and one instruction set for machines from small ones to mainframes. Over a hundred opcodes, addressing of the form `base + index + displacement`.

> The PDP-8 (1965) managed with eight instructions, and the sparse set was compensated by indirect addressing and auto-increment cells, showing that a small basis with smart addressing modes gives flexibility no worse than a large one.

> The CDC 6600 (1964), the first successful supercomputer, where ten functional units worked in parallel and a scoreboard laid out instruction order (true pipelining came later, in the 7600). Its Fortran compiler already scheduled instruction order so the units would not sit idle.



### 1970s: microprocessors, x86, coprocessors

> The Intel 4004 (1971), 4-bit, on the order of forty instructions, no multiplication. A minimal basis in a silicon chip.

> The Intel 8080 (1974), 8 bits, gained the `DAA` instruction for decimal arithmetic, that is adding decimal digits in a binary register. An example of extending the basis for a specific application area, bookkeeping and cash registers, while its counterpart `DAS` for subtraction arrived only with the 8086.

> The Zilog Z80 (1976) added `LDIR` (block move) and `DJNZ` (decrement and jump if non-zero), that is a hardware realization of the "counting loop" pattern, another example of a frequent software idiom dropping into silicon.

The Intel 8086 (1978), 16 bits, 16-bit multiplication `MUL` and `IMUL`, division `DIV`, and the operations `MOVS`, `LODS`, `STOS`, which together with the `REP` prefix gave a hardware memory-copy loop, and where you used to need an explicit loop in assembly, it was now one instruction.

```cpp
void copy_by_hand(char *dst, const char *src, std::size_t n) {
    for (std::size_t i = 0; i < n; i++)
        dst[i] = src[i];
}

void copy_today(char *dst, const char *src, std::size_t n) {
    std::memcpy(dst, src, n);
}
```

On 64 megabytes a byte-by-byte loop takes from eight to eleven milliseconds, while `memcpy` finishes in two and a half, because `memcpy` copies thirty-two bytes at a time with vector registers and also knows how to bypass the cache on large blocks.

> The Intel 8087 (1980, formally already a transition into the 1980s) added a coprocessor with an 80-bit register stack and instructions like `FSQRT`, `FPTAN`, `FPATAN`, `FYL2X`, `F2XM1`. Heavy floating-point arithmetic and some transcendental functions moved out of long library loops into silicon, but separate `FSIN` and `FCOS` appeared later, roughly with the 387, and compilers started preferring hardware opcodes over library `sin` when the target supported them.



### 1980s: RISC and the philosophy that "the compiler is smarter than microcode"

Berkeley RISC (1980) and Stanford MIPS (1981), academic projects that showed that simple fixed-length instructions let you build a long pipeline without stalls, and that complex operations are better implemented in the compiler than in microcode.

> The Intel 80186 (1982) pulled function prologue and epilogue into hardware with `PUSHA` and `POPA`, which save all registers in one instruction, while `ENTER` and `LEAVE` build and tear down a frame. Of that pair only `LEAVE` survived, which we already talked about in the chapter on procedures. The Intel 80386 (1985) added 32 bits and 32-bit forms of the same instructions to that.

ARM (1985, Acorn), a three-address RISC instruction set with conditional execution (predication) of many instructions, so the compiler could fold short `if` branches into straight-line code without jumps. That has nothing in common with speculative execution; the technique is simply about jumping less, and on processors with a weak branch predictor it worked very well. The simplest place to see it is the one-liner `if (x != 0) y = 1;`, which turns into either a branch or predication.

```text
; ordinary path, with a jump
      CMP   r0, #0
      BEQ   skip
      MOV   r1, #1
skip:

; the same thing by predication, no jump at all
      CMP   r0, #0
      MOVNE r1, #1     ; the instruction executes only if not equal
```

Later, 64-bit ARM dropped it, leaving the conditional select `CSEL` and a couple of related instructions. The reason is in predication itself, which takes bits in every instruction and complicates the pipeline, while the win only shows up on short branches, so when moving to wide superscalar cores the cost stopped paying for itself.

> The MIPS R2000 (1985) got a separate multiply and divide unit that ran asynchronously from the pipeline, so you fetched the result from separate registers when it was ready.

> SPARC (1987, Sun) arrived later than the first commercial RISCs (MIPS R2000 and PA-RISC were earlier), but brought register windows, where on a function call the window slides and arguments with locals land in other registers, and you need not save them to memory at all. A rare case of dropping not arithmetic into the basis, but the calling convention itself.



### 1990s: SIMD and widening the basis

> Intel MMX (1996, Pentium MMX) added fifty-seven new instructions and eight 64-bit registers from `mm0` to `mm7`, and a single `PADDB` added eight pairs of bytes at once. The registers were overlaid on the x87 registers, so mixing floating-point arithmetic and SIMD in one function became a separate art. Video codecs, audio, and 2D graphics quickly picked up SIMD, and on lucky algorithms the speedup could be several times, but "N times" always depended on the task, the data, and whether the hot path had been rewritten by hand.

> AMD 3DNow! (1998, K6-2) added a set of instructions for a pair of `float32` and aimed at 3D game geometry with instructions like `PFMUL`, `PFADD`, `PFRCPIT1`. A niche extension of the basis for one specific market, and it never went further than that.

> Intel SSE (1999, Pentium III) added another large layer of SIMD commands and eight new 128-bit registers from `xmm0` to `xmm7`. The `-msse` flag in GCC opened access to them, while automatic vectorization appeared there noticeably later, only in GCC 4.0 (2005), so for the first years SIMD remained hand work.

SSE2 (2001, Pentium 4) added double precision and integer SIMD in the `xmm` registers, after which compilers quickly started switching to SSE2 even for ordinary scalar floating point. What widening the basis looked like in practice is easiest to show on adding two arrays. First the ordinary loop, then the same through SSE, four numbers per iteration.

```cpp
void add_scalar(float *dst, const float *a, const float *b, std::size_t n) {
    for (std::size_t i = 0; i < n; i++)
        dst[i] = a[i] + b[i];
}

// n is a multiple of four
void add_sse(float *dst, const float *a, const float *b, std::size_t n) {
    for (std::size_t i = 0; i < n; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        _mm_storeu_ps(dst + i, _mm_add_ps(va, vb));
    }
}
```

A measurement on an array that sits entirely in cache gives the following picture. If you forbid the compiler to vectorize the first loop, it takes 780 milliseconds, and handwritten SSE gives almost a twofold gain at 452 ms. And the one left to the autovectorizer takes 265 milliseconds, beating the handwritten version by almost two times. (The numbers are taken from an example on my laptop.) There is nothing mystical here, and the compiler is not obliged to stick to the 128 bits that were written by hand; it takes wider registers and unrolls the loop along the way. In 1999 there was no such luxury and all SIMD code was written by hand, and today handwritten intrinsics are justified only where the autovectorizer cannot cope and the developer has to step in.

> x86-64 or AMD64 (2003, AMD Opteron) technically belongs to the 2000s, but its roots go back to the 1990s; the project started around 1999. Registers were now extended to 64 bits, registers from `r8` to `r15` were added, and SSE2 became a mandatory part of the basis.



### 2000s: 64 bits everywhere, cryptography, strings

> SSE3 (2004, Prescott) added horizontal addition `HADDPS`, which adds neighboring lanes inside a register, and it was needed for complex arithmetic and convolutions, so here too code under market pressure moved into silicon.

> SSSE3 (2006, Core 2) brought `PSHUFB`, a byte permute by mask that became one of the most powerful primitives, used to build lookup tables and branchless bit manipulations.

SSE4.1 and 4.2 (2007 and 2008) added `PMULDQ` (32-by-32 multiplication with a 64-bit result), `PCMPESTRM` for string search, and `CRC32`, and now string algorithms and hash functions got hardware support. The `CRC32` algorithm is perhaps the most telling here, because until then a checksum was computed from tables that people dragged from project to project.

```cpp
extern uint32_t table[256];   // that same table, 1 KB of data

uint32_t crc32c_table(const uint8_t *p, std::size_t n) {
    uint32_t c = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < n; i++)
        c = table[(c ^ p[i]) & 0xFF] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

uint32_t crc32c_hw(const uint8_t *p, std::size_t n) {
    uint32_t c = 0xFFFFFFFFu;
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8)
        c = (uint32_t)_mm_crc32_u64(c, *(const uint64_t *)(p + i));
    for (; i < n; i++)
        c = _mm_crc32_u8(c, p[i]);
    return c ^ 0xFFFFFFFFu;
}
```

On sixteen megabytes the table version takes about 28 milliseconds, the hardware one about 1.5, a difference of roughly twenty times, and the table no longer occupies the data cache. But there is a catch here too: the hardware instruction does not compute the `CRC32` that sits in zip and zlib; it uses the Castagnoli variant with a different generating polynomial, called `CRC-32C`. The checksums do not match, so you cannot replace one with the other in an existing file format, and that is also a good example of how what drops into the basis may not be the operation itself but one particular version of it.

### 2010s: AVX, bit algebra, transactional memory

> AVX (2011, Sandy Bridge) added 256-bit `ymm` registers, that is eight `float32` numbers or four `float64` in one register, and also got rid of destructive operations, so the result is now written to a separate register instead of overwriting the first argument.

> AVX2 (2013, Haswell) brought integer SIMD up to 256 bits as well, and GCC with `-march=haswell` started automatically vectorizing noticeably more patterns.

`POPCNT` (SSE4.2 and ABM, around 2008) counts the number of one-bits in a single instruction. In games such code shows up often, whether it is a mask of occupied cells, a mask of living units in a squad, or bit sets for visibility. Before 2008 it was written two ways, the blunt way or with a mask trick, and since `C++20` there is `std::popcount` from `<bit>` for this.

```cpp
// the blunt way: thirty-two passes over the bits
int popcount_naive(uint32_t x) {
    int n = 0;
    for (int i = 0; i < 32; i++)
        n += (x >> i) & 1u;
    return n;
}

// the mask trick, also known as SWAR: add bits in pairs, then in nibbles
int popcount_swar(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0F0F0F0Fu;
    return (int)((x * 0x01010101u) >> 24);
}

// and what it turned into
int popcount_today(uint32_t x) {
    return std::popcount(x);
}
```

On four million words the naive version takes about 35 milliseconds, the mask trick 2.4 milliseconds, and `std::popcount` 1.1 milliseconds, so the hardware instruction is about thirty times faster than the naive loop. But if you rebuild the same thing with `/arch:AVX2`, then `std::popcount` stays at its 1.0 milliseconds, while the mask trick drops to 0.8 milliseconds and already beats the hardware instruction. The reason is that `POPCNT` is scalar and takes one word at a time, while the SWAR arithmetic vectorizes beautifully, and the compiler counts bits in 256-bit registers at once. So "dropped into silicon" does not always mean "got faster", and widening the basis can lose to specialized primitives.

> AVX-512 (Knights Landing in 2016, Skylake-X in 2017) added 512-bit `zmm` registers that hold sixteen `float32` numbers.



### 2020s: matrices and primitives for neural nets

AVX-VNNI (2021, Alder Lake) added the `VPDPBUSD` instruction and its variants to compute a dot product of `int8` and `int16` with accumulation into `int32`. Now one call replaces the inner convolution loop, and it is needed to run neural nets directly on the CPU, without a GPU.

```cpp
// int8 convolution with accumulation into int32, as in network inference
int32_t dot_scalar(const uint8_t *a, const int8_t *b, std::size_t n) {
    int32_t acc = 0;
    for (std::size_t i = 0; i < n; i++)
        acc += (int32_t)a[i] * (int32_t)b[i];
    return acc;
}

static int32_t hsum(__m256i v) {          // add eight lanes
    alignas(32) int32_t t[8];
    _mm256_store_si256((__m256i *)t, v);
    int32_t s = 0;
    for (int k = 0; k < 8; k++) s += t[k];
    return s;
}

// the same thing, but 32 pairs of bytes per instruction
int32_t dot_vnni(const uint8_t *a, const int8_t *b, std::size_t n) {
    __m256i acc = _mm256_setzero_si256();
    for (std::size_t i = 0; i < n; i += 32)
        acc = _mm256_dpbusd_avx_epi32(acc,
                  _mm256_loadu_si256((const __m256i *)(a + i)),
                  _mm256_loadu_si256((const __m256i *)(b + i)));
    return hsum(acc);
}
```

The scalar version on eight kilobytes of data takes 26 milliseconds, and the version with `VPDPBUSD` as written above only 17, so there is a win, but nowhere near what you expect from a hardware instruction that does thirty-two multiply pairs at once. The reason here is the "single" accumulator you passed into the intrinsic: each next instruction waits for the previous result, so the loop hits not throughput but data waiting. If you spread the accumulation across four independent accumulators, time drops to 5 milliseconds, but the same convolution on a pair of older instructions `PMADDUBSW` and `PMADDWD` with four accumulators also finishes in 5 milliseconds. So dropping an operation into silicon does not automatically mean speeding up the code. Of course a special instruction saves opcodes and pipeline space, but you will get the promised win only when you yourself remove the data dependence. All measurements in the chapter were made on my laptop and the numbers here are illustrative; on another processor the ranking may well be different.

### these days...

Modern compilers already live in a world where the computational basis for fundamental types is very rich and well optimized, and their main job is to use that basis as efficiently as possible without breaking the language semantics. So when you write `a + b` for integers, the compiler already knows which machine instruction to take and which processor flags it affects.

## How to fix `Fixed`

Back to division in our type from the opening of the chapter. Five hundred projectiles, each dividing velocity by something on the order of one tenth, and the subtraction loop spins millions of times per division, so all of this costs us seconds, which of course will not do for a real game. But there is a way out: just remember the rules of integer division.

```cpp
struct Fixed {                        // that same type from the opening of the chapter
    int32_t raw;
};

// division on the instruction the 8086 brought back in 1978
Fixed divide(Fixed a, Fixed b) {
    return Fixed{(int32_t)(((int64_t)a.raw << 16) / b.raw)};
}
```

And those same five hundred divisions now take 0.7 microseconds, and the result, importantly, is the same bit for bit. The determinism we need has not gone anywhere, because integer division rounds toward zero and does so the same on any platform, and if you land on a platform with no 64-bit division in hardware, the right answer is long division by bits, thirty-two steps instead of millions. Choosing a computational basis is not only "what is in the processor" but also "which algorithm you laid on top of it".

## What this means for your types

This will be useful if you set out to write operations for your own type. First you have to split them into primitives and derived ones and make sure the primitives are not built out of loops. Multiplication through addition and division through subtraction are clearly not the best options for an implementation, and everything you build on top inherits the cost of the foundation, and no amount of optimizing the wrappers will fix that, and addition through repeated `next` will stay linear and expensive. Eighty years of processor history is a ready-made list of operations that show up often enough to be worth having in the basis, and for your `Fixed` it is enough to look at what people already invented in that time. You are not the first to design this, and that is good news too, because most of the rakes have already been stepped on by someone else. Operations that are logically derivable are still worth adding, even if they are not used every day, because the cost of one line in the type's interface is incomparable to the cost of ten lines at every use site, but you should add them after the basis has become efficient, not instead of that. A pretty `operator<=` with a loop underneath will still be "bad code". And finally, when you design your own type you are essentially repeating the path of machine architects and compiler authors. You choose which operations to make basic so that algorithms express themselves conveniently and the compiler emits efficient code. The difference is that the engineers of early processors did not have that choice, and you do... as you also have eighty years of their experience and tried solutions.
