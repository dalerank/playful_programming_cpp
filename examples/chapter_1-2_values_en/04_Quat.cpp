// Example from: playfull_programming_chapter_1-2_values_en.md
// Source line: 181
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_Quat.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_Quat 04_Quat.cpp

#include "../common.h"

// One rotation, two representations: q and -q
struct Quat { float x, y, z, w; };

static float dot(const Quat& a, const Quat& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Bitwise different, same rotation by meaning
bool same_rotation(const Quat& a, const Quat& b, float eps = 1e-5f) {
    return std::abs(dot(a, b)) > 1.f - eps; // absolute value, sign does not matter
}

int main() { return 0; }
