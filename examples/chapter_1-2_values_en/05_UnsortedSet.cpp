// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 197
// Build (MSVC): cl /std:c++20 /EHsc /nologo 05_UnsortedSet.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 05_UnsortedSet 05_UnsortedSet.cpp

#include "../common.h"

// cheap insert, expensive equality
struct UnsortedSet {
    std::vector<int> elements; // order and duplicates arbitrary

    void insert(int x) {                              // O(1)
        elements.push_back(x);
    }

    bool operator==(const UnsortedSet& other) const { // O(n log n)
        auto a = elements;
        auto b = other.elements;
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        a.erase(std::unique(a.begin(), a.end()), a.end());
        b.erase(std::unique(b.begin(), b.end()), b.end());
        return a == b;
    }
};

// same kind, trade reversed
struct SortedSet {
    std::vector<int> elements; // invariant: sorted, no duplicates

    void insert(int x) {                              // O(n), tail shift
        auto it = std::lower_bound(elements.begin(), elements.end(), x);
        if (it == elements.end() || *it != x) {
            elements.insert(it, x);
        }
    }

    bool operator==(const SortedSet& other) const {   // O(n)
        return elements == other.elements;
    }
};

int main() { return 0; }
