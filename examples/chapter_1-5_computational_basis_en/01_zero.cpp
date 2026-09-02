// Example from: playfull_programming_chapter_1-5_computational_basis_en.md
// Source line: 33
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_zero.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_zero 01_zero.cpp

#include "../common.h"

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
