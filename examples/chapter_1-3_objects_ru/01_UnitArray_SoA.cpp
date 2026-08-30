// Example from: playfull_programming_chapter_1-3_objects_ru.md
// Source line: 65
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_UnitArray_SoA.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_UnitArray_SoA 01_UnitArray_SoA.cpp

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <string_view>
#include <regex>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <deque>
#include <queue>
#include <future>
#include <list>
#include <memory>
#include <memory_resource>
#include <utility>
#include <algorithm>
#include <numeric>
#include <functional>
#include <optional>
#include <variant>
#include <tuple>
#include <type_traits>
#include <iterator>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <system_error>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <new>
#include <limits>
#include <concepts>
#include <ranges>
#include <span>
#include <bit>
#include <coroutine>
#if __has_include(<generator>)
#include <generator>
#endif
#include <immintrin.h>

// структура массивов: поля одного юнита разнесены по памяти
struct UnitArray_SoA {
    std::vector<float> x;   // все координаты x подряд
    std::vector<float> y;   // все y
    std::vector<float> z;   // все z
    std::vector<int> hp;    // здоровье отдельным массивом

    size_t size() const { return x.size(); }

    // позиция юнита i это (x[i], y[i], z[i]), три ссылки в разные массивы,
    // а не один объект языка, на который можно взять указатель
    struct PositionRef {
        float& x;
        float& y;
        float& z;

        PositionRef(float& px, float& py, float& pz): x(px), y(py), z(pz) {}

        void print() const {
            std::cout << x << ", " << y << ", " << z << "\n";
        }
    };

    PositionRef position(size_t i) {
        return PositionRef(x[i], y[i], z[i]);
    }
};

void example_soa() {
    UnitArray_SoA units;
    units.x  = {10.0f, 12.0f, 14.0f};
    units.y  = { 0.0f,  0.0f,  0.0f};
    units.z  = { 5.0f,  7.0f,  9.0f};
    units.hp = {  100,    80,    60};

    std::cout << "SoA: ";
    units.position(1).print();

    // горячий цикл трогает только позиции, и они лежат подряд
    for (size_t i = 0; i < units.size(); ++i) {
        units.x[i] += 1.0f;
    }
}

int main() { return 0; }
