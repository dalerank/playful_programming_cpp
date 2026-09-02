// Example from: playfull_programming_chapter_1-2_values_ru.md
// Source line: 179
// Build (MSVC): cl /std:c++20 /EHsc /nologo 04_Quat.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 04_Quat 04_Quat.cpp

#include "../common.h"

// Один поворот, два представления: q и -q
struct Quat { float x, y, z, w; };

static float dot(const Quat& a, const Quat& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Побитово разные, по смыслу один и тот же поворот
bool same_rotation(const Quat& a, const Quat& b, float eps = 1e-5f) {
    return std::abs(dot(a, b)) > 1.f - eps; // модуль, потому что знак не важен
}

int main() { return 0; }
