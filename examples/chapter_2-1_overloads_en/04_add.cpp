// Example from: playfull_programming_chapter_2-1_overloads_en.md
// Source line: 116
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_add.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_add 04_add.cpp

#include "../common.h"

// three functions, one name, and not a single algorithm
int add(int a, int b) { return a + b; }
double add(double a, double b) { return a + b; }
std::string add(const std::string& a, const std::string& b) { return a + b; }

int main() { return 0; }
