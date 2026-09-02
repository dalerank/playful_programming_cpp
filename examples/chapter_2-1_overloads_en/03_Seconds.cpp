// Example from: playfull_programming_chapter_2-1_overloads_en.md
// Source line: 89
// Build (MSVC): cl /std:c++20 /EHsc /nologo 03_Seconds.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 03_Seconds 03_Seconds.cpp

#include "../common.h"

struct Seconds { float v; };
struct Metres  { float v; };
struct Speed   { float v; };     // m/s
struct Accel   { float v; };     // m/s²

Speed  operator*(Accel a, Seconds t) { return Speed{a.v * t.v}; }
Metres operator*(Speed s, Seconds t) { return Metres{s.v * t.v}; }

void typed_physics() {
    Accel g{9.8f};
    Seconds t{2.0f};

    Speed v = g * t;         // 19.6 m/s, type changed along with meaning
    Metres path = v * t;     // 39.2 m of travel at constant speed

    // Accel oops = g * g;   // no overload for this, which is good news
    // Metres bad = v;       // will not compile either, meters are not speed

    printf("%.1f %.1f\n", v.v, path.v);
}

int main() { return 0; }
