// Example from: playfull_programming_chapter_1-2_values_ru.md
// Source line: 248
// Build (MSVC): cl /std:c++20 /EHsc /nologo 06_double.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 06_double 06_double.cpp

#include "../common.h"

void example() {
    double pos_zero =  0.0;
    double neg_zero = -0.0;

    std::cout<<"\n+0.0 == -0.0:"<<(pos_zero == neg_zero)<<"\n";
    // 1, равны по ==

    std::cout<<"signbit(+0.0):"<<std::signbit(pos_zero)<<"\n";
    // 0

    std::cout<<"signbit(-0.0):"<<std::signbit(neg_zero)<<"\n";
    // 1, результат другой!
    // signbit нарушает регулярность:
    // заменили равное на равное, но результат изменился
}

int main() {
    example();
    return 0;
}
