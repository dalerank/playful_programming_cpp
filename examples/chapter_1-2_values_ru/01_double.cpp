// Example from: playfull_programming_chapter_1-2_values_ru.md
// Source line: 33
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_double.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_double 01_double.cpp

#include "../common.h"

void example() {
    double y = std::nan("");   // «не число»

        std::cout << "y = "        << y                 << "\n";
        std::cout << "y + 1  = "   << y + 1             << "\n";
        std::cout << "y * 0  = "   << y * 0             << "\n";
        std::cout << "y == y = "   << (y == y)          << "\n";
        std::cout << "sqrt(-1) = " << std::sqrt(-1.0)   << "\n";
}

int main() {
    example();
    return 0;
}
