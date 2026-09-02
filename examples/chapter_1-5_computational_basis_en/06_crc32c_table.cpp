// Example from: playfull_programming_chapter_1-5_computational_basis_en.md
// Source line: 258
// Build (MSVC): cl /std:c++20 /EHsc /nologo 06_crc32c_table.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 06_crc32c_table 06_crc32c_table.cpp

#include "../common.h"

extern uint32_t table[256];   // that same table, 1 KB of data

uint32_t crc32c_table(const uint8_t *p, std::size_t n) {
    uint32_t c = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < n; i++)
        c = table[(c ^ p[i]) & 0xFF] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

uint32_t crc32c_hw(const uint8_t *p, std::size_t n) {
    uint32_t c = 0xFFFFFFFFu;
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8)
        c = (uint32_t)_mm_crc32_u64(c, *(const uint64_t *)(p + i));
    for (; i < n; i++)
        c = _mm_crc32_u8(c, p[i]);
    return c ^ 0xFFFFFFFFu;
}

int main() { return 0; }
