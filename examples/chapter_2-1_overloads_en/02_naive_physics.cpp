// Example from: playfull_programming_chapter_2-1_overloads_en.md
// Source line: 61
// Build (MSVC): cl /std:c++20 /EHsc /nologo 02_naive_physics.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 02_naive_physics 02_naive_physics.cpp

#include "../common.h"

void naive_physics() {
    float acceleration = 9.8f;   // m/s²
    float velocity = 100.0f;     // m/s
    float mass = 50.0f;          // kg
    float dt = 2.0f;             // s

    // The compiler allows writing nonsense
    float nonsense1 = acceleration + mass;      // 9.8 + 50 = ???
    // adding m/s² to kg, physically absurd

    float nonsense2 = velocity * mass;          // 100 * 50 = ???
    // got kg·m/s, which is momentum, not acceleration

    float nonsense3 = acceleration / velocity;  // 9.8 / 100 = ???
    // got s⁻¹, frequency, unrelated to the task

    float nonsense4 = velocity + dt;            // 100 + 2 = ???
    // and here we added velocity and time, and it all compiles again

    // Everything compiles with zero error signals, leaving
    // just wrong numbers with no asserts or compiler diagnostics
    printf("%.1f %.1f %.1f %.1f\n", nonsense1, nonsense2, nonsense3, nonsense4);
}

int main() { return 0; }
