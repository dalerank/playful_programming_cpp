// Example from: playfull_programming_chapter_1-6_regularity_ru.md
// Source line: 29
// Build (MSVC): cl /std:c++20 /EHsc /nologo 01_Loot.cpp
// Build (clang): clang++ -std=c++20 -Wall -o 01_Loot 01_Loot.cpp

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

/*
   Enemy, нерегулярный тип. У каждого врага свой лут, он живёт
   в куче, и Enemy держит на него сырой указатель.
   Копирование мелкое, а == сравнивает адрес лута: копия «равна»
   оригиналу, но не независима от него, лут у них один на двоих.
   Зато два врага с одинаковым содержимым карманов неравны.
*/
struct Loot { int gold; int ammo; };

class Enemy {
    Loot* loot_;
    int   id_;
public:
    // Конструктора по умолчанию нет, и std::default_initializable такой тип не пропустит.
    // Поля инициализируем строго в порядке объявления. Про нарушение
    // компилятор умеет предупредить (C5038), только предупреждение это
    // по умолчанию выключено и молчит даже на /W4, а на зависимых полях
    // перестановка дала бы уже чтение мусора.
    Enemy(int id, int gold)
        : loot_(new Loot{gold, 0}), id_(id) {}

    // Деструктора тоже нет, и тоже намеренно, с ним мелкая копия
    // отдала бы один и тот же Loot операционной системе дважды.

    // Копирующий конструктор копирует указатель, а не лут.
    // После этого copy.loot_ == orig.loot_, и обыск одного трупа
    // опустошает карманы другого. Это нарушение регулярности,
    // копия зависит от оригинала.
    Enemy(const Enemy& o)
        : loot_(o.loot_), id_(o.id_) {}       // <- намеренная ошибка

    // Оператор == сравнивает адреса, поэтому два врага с одинаковым
    // золотом и патронами считаются неравными. Это равенство по
    // идентичности указателя, а не по значению содержимого.
    bool operator==(const Enemy& o) const {
        return loot_ == o.loot_;              // <- намеренная ошибка
    }
};

int main() { return 0; }
