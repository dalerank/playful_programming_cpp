// Example from: playfull_programming_chapter_2-1_overloads_ru.md
// Source line: 225
// Build (MSVC): cl /std:c++20 /EHsc /nologo 07_Mat2.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 07_Mat2 07_Mat2.cpp

#include "../common.h"

struct Mat2 { int a, b, c, d; };           // повороты и отражения на сетке

Mat2 operator*(Mat2 m, Mat2 n) {           // тот самый «перегруженный оператор»
    return Mat2{m.a * n.a + m.b * n.c, m.a * n.b + m.b * n.d,
                m.c * n.a + m.d * n.c, m.c * n.b + m.d * n.d};
}

template <typename T>
T pow_fast_careful(T x, unsigned n) {      // алгоритм из середины главы, слово в слово
   T result = T{1};

   while (n > 0) {
       if (n & 1)
           result = result * x;

       n >>= 1;

       if (n)
           x = x * x;
   }

   return result;
}

void one_algorithm() {
    printf("%d\n", pow_fast_careful(3, 5));            // 243, тут T{1} угадал

    Mat2 rot{0, -1, 1, 0};                             // поворот на 90 градусов
    Mat2 r4 = pow_fast_careful(rot, 4);                // четыре поворота, ждём единичную
    printf("{%d %d %d %d}\n", r4.a, r4.b, r4.c, r4.d); // {1 0 0 0}, а не {1 0 0 1}
}

int main() { return 0; }
