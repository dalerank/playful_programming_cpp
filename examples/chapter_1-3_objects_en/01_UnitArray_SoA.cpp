// Example from: playfull_programming_chapter_1-3_objects_en.md
// Source line: 65
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_UnitArray_SoA.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_UnitArray_SoA 01_UnitArray_SoA.cpp

#include "../common.h"

// structure of arrays: one unit's fields are spread across memory
struct UnitArray_SoA {
    std::vector<float> x;   // all x coordinates in a row
    std::vector<float> y;   // all y
    std::vector<float> z;   // all z
    std::vector<int> hp;    // health in a separate array

    size_t size() const { return x.size(); }

    // unit i's position is (x[i], y[i], z[i]), three references into
    // different arrays, not one language object you can take a pointer to
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

    // hot loop touches only positions, and they sit contiguously
    for (size_t i = 0; i < units.size(); ++i) {
        units.x[i] += 1.0f;
    }
}

int main() { return 0; }
