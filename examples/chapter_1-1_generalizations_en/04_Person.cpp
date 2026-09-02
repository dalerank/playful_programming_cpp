// Example from: playfull_programming_chapter_1-1_generalizations_en.md
// Source line: 143
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_Person.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_Person 04_Person.cpp

#include "../common.h"

struct Person {
   std::string name;
   int age;
};

Person p{"Socrates", 70};

int main() { return 0; }
