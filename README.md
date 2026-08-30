# Playful Programming in C++

A collection of articles about C++ — written not the way they teach it at university or present it in books.

The focus is on ideas first: generic programming, overload design, algorithm thinking. Each chapter is available in both Russian and English.

**New:** [Computational Basis (RU/EN)](playfull_programming_chapter_1-5_computational_basis_ru.md)

---

## Chapters

| # | Topic | RU | EN |
|---|-------|----|----|
| 1.1 | Generalizations | [RU](playfull_programming_chapter_1-1_generalizations_ru.md) | [EN](playfull_programming_chapter_1-1_generalizations_en.md) |
| 1.2 | Values | [RU](playfull_programming_chapter_1-2_values_ru.md) | [EN](playfull_programming_chapter_1-2_values_en.md) |
| 1.3 | Objects | [RU](playfull_programming_chapter_1-3_objects_ru.md) | [EN](playfull_programming_chapter_1-3_objects_en.md) |
| 1.4 | Procedures | [RU](playfull_programming_chapter_1-4_procedures_ru.md) | [EN](playfull_programming_chapter_1-4_procedures_en.md) |
| 1.5 | Computational Basis | [RU](playfull_programming_chapter_1-5_computational_basis_ru.md) | [EN](playfull_programming_chapter_1-5_computational_basis_en.md) |
| 2 | Overloads | [RU](playfull_programming_chapter_2_overloads_ru.md) | [EN](playfull_programming_chapter_2_overloads_en.md) |
| 3 | Concepts and Constraints | [RU](playfull_programming_chapter_3_requires_ru.md) | [EN](playfull_programming_chapter_3_requires_en.md) |
| 4 | History of Concepts | [RU](playfull_programming_chapter_4_history_ru.md) | [EN](playfull_programming_chapter_4_history_en.md) |
| 5 | Concept Hierarchy | [RU](playfull_programming_chapter_5_hierarhy_ru.md) | [EN](playfull_programming_chapter_5_hierarhy_en.md) |
| 6 | Constraints Revisited | [RU](playfull_programming_chapter_6_again_requires_ru.md) | [EN](playfull_programming_chapter_6_again_requires_en.md) |
| 7 | Name Lookup in C++ | [RU](playfull_programming_chapter_7_names_lookup_ru.md) | [EN](playfull_programming_chapter_7_names_lookup_en.md) |

### Examples (C++)

Runnable snippets from chapters 1.1–1.5 live under [`examples/`](examples/README.md) (RU and EN folders). Build with `/std:c++20` or `-std=c++20`.

---

## Chapter Contents

### Chapter 1.1 — Generalizations

- Kind vs genus: the `Weapon` hierarchy trap and why «shooting» is a property, not a type
- Abstract and concrete entities; attributes, identity, and the Ship of Theseus in code
- Kinds and genera; inventory example (medkit, ammo, grenade) and templates as rules over kinds
- `total_weight` and writing code for everything that obeys a rule, not for one concrete type
- Types in C++ as kinds; how the compiler, LLVM IR, and MSVC PDB see them differently
- Concepts (`C++20`) and modules: constraint checking and vendor-specific type serialization

### Chapter 1.2 — Values

- Network desync from «the same number» that is not the same: `-ffast-math`, reordering, `+0.0` vs `-0.0`
- Bits, interpretation, well-formedness; `int` vs `NaN` as legal but ill-formed float values
- Total vs partial types: `int` overflow, `float` grid at world scale, total `enum class Suit`
- Unique vs non-unique representation; entity index + generation counter
- Equality vs representational equality; rationals, quaternions, set containers
- Regularity, substitution, and why the optimizer needs equal inputs to mean equal outputs

### Chapter 1.3 — Objects

- A tower pointer that still "works" after the unit died: "where it lives" vs "who it is"
- An object as a contract between programmer, compiler, and machine; state as a value that can change
- Why objects exist: mutable memory under even "pure" computation
- AoS vs SoA: cache lines, hot fields, napkin math, and measured timings when splitting layout pays
- Object type vs value type; unique representations and why `memcmp` fails on padding
- Lifetime: object starts after initialization and ends when the destructor starts; pools reuse addresses
- Identity tokens separate from state; equality vs identity; Ship of Theseus
- COW strings, cfront, and how optimizers treat objects under as-if
- Addresses that do not survive reallocation; generation counters in ECS; pointers for "where", tokens for "who"

### Chapter 1.4 — Procedures

- A tooltip that shows another unit's name scraps: returning `const char*` to a stack buffer
- Stack frames belong to the call, not the function; leaf functions, red zone, and why debug "worked"
- How many frames fit: measuring frame size, 1 MB stacks, worker threads, fibers; recursion vs heap stack
- Calling conventions: registers vs stack, cdecl vs stdcall, who pops the arguments
- Prologue, epilogue, `enter`/`leave`, and slot reuse inside one frame
- Inline and LTO erase call machinery; what remains is the contract of who reads and writes which objects
- Where state lives: arguments, locals, globals, and `static` hidden state that survives a new match
- Pure in / pure out / in-out; aliasing, `restrict`, and why the compiler fears your pointer
- Three rules: return by value not by address, encode roles in the signature, pass context explicitly

### Chapter 1.5 — Computational Basis

- Multiplayer determinism and why naive `Fixed` division by repeated subtraction melts the profiler
- Minimal, efficient, and expressive bases: `next` is enough in theory, useless in practice
- Why `<=>` and derived comparisons matter for how a type meets the standard library
- Eighty years of hardware bases: from ENIAC and Baby through x86, RISC, SIMD, CRC32C, AVX, VNNI
- Autovectorization vs handwritten intrinsics; when "in silicon" is still slower than SWAR
- Fixing `Fixed` with integer division; choosing primitives so wrappers inherit a cheap foundation

### Chapter 2 — Overloads

- Algorithm-first thinking: why you should define the algorithm before picking types, not the other way around
- Overload sets as universal interfaces: one name, many types, zero ambiguity — when done right
- Good overloads: semantic consistency, non-overlapping domains, and the rule of least surprise
- Bad overloads: implicit conversion traps, ambiguity that hides bugs until runtime
- `std::filesystem` case study: naming pitfalls in a real standard library API and what they teach us
- How overload design scales to generic code without reaching for templates immediately

### Chapter 3 — Concepts and Constraints

- Why SFINAE error messages are unreadable and what concepts do to fix that
- `requires` as a contract: excluding unsuitable candidates before overload resolution even begins
- Three syntactic forms: leading `requires`, trailing `requires`, and abbreviated function templates (`auto`)
- Writing and composing custom concepts with `&&` and `||`
- How the compiler resolves ambiguity between constrained overloads
- `Cat != cat` example: equality semantics, type safety, and what "same type" actually means

### Chapter 4 — History of Concepts

- Stepanov's original vision: concepts as full semantic contracts, not just syntax checks
- Early 1990s proposals with axioms: `EqualityComparable` was supposed to guarantee reflexivity, symmetry, and transitivity — verified by the compiler
- Why "heavy" concepts were cut from C++11 at the last moment after years of committee work
- Concepts Lite (Andrew Sutton, 2009): the deliberate decision to drop semantic requirements and keep only syntax checks
- The core philosophical shift: from "is this type mathematically correct?" to "can code be generated with this type?"
- Equivalence relations as a mathematical apparatus — why the committee decided the compiler shouldn't prove theorems
- What C++20 concepts actually check vs what the early proposals promised

### Chapter 5 — Concept Hierarchy

- Partial ordering of constrained overloads: how the compiler picks the most specialized candidate
- Subsumption in detail: when one constraint set formally implies another, and when it does not
- The syntactic identity rule: why two logically equivalent constraints are treated as unrelated by the compiler
- Practical pitfall: changing the order of `&&` terms in a constraint breaks subsumption and causes ambiguity
- STL iterator hierarchy (`input_iterator` → `forward_iterator` → `bidirectional_iterator` → `random_access_iterator`) as a model of consciously designed concept hierarchy
- `if constexpr` as a more readable alternative to deep concept hierarchies
- Alexandrescu's and Sutter's warnings: concepts are a tool, not an opportunity to prove how clever you are

### Chapter 6 — Constraints Revisited

- Two hard limits on concepts: no recursive concept definitions, no `requires` clause on a concept itself — and why those limits are intentional
- Why these limits exist: keeping concepts as fast, stateless, predictable compile-time predicates, not a new metaprogramming layer
- Three syntactic forms of `requires` and when each reads best
- Concepts on template class parameters: constraining `Vec3<T>` to floating-point types at the declaration boundary
- `concept auto` in local variable declarations: self-documenting code that tells reviewers the intent, not just the type
- Conditional methods and constructors: `requires` on individual class members makes interfaces adapt to type capabilities
- Partial ordering recap: syntactic identity still matters; always extract compound constraints into a named concept

### Chapter 7 — Name Lookup in C++

*Combines three articles: "Does the Compiler Care About Names?", "At Night All Cats Are Grey, and All usings Look the Same", and "Compilers Get Names Mixed Up Too".*

- The two fundamental compiler questions that underlie templates, concepts, and overloads
- Name coexistence vs overloading: why a function and a struct can share a name, but a function and a namespace cannot
- The "if it quacks" rule: how the compiler's function-first preference leads to surprising error messages
- Name mangling from the 1980s: why `void f(int)` and `void f(const int)` are the same linker symbol, and what that means for overloads
- `using` declarations vs `using` directives: a subtle scope difference that silently makes two adjacent lines refer to different variables
- CWG Issue 36: how GCC and MSVC accepted code that C++98 formally prohibited, and how the committee eventually admitted the standard was wrong
- ADL (Koenig lookup): the history of why it was invented, Bjarne's 1993 problem document, and Koenig's 1995 N0645 paper
- ADL side effects: why argument-dependent lookup makes namespaces less strict and can cause silent semantic conflicts between libraries
- Two-phase name lookup: non-dependent names at template definition, dependent names at instantiation — and why this matters
- GCC's "lazy parsing": how deferring all name lookup to instantiation time made non-conformant code compile for decades
- Clang and dependent base classes: why phase-1 lookup in a dependent base is forbidden, illustrated with a specialization example
- MSVC's 25-year technical debt: how MFC coupling politically protected a non-conformant parser until VS 2017, and what it took to fix it

---

## About

These articles are about the *why* behind C++ features, not just the syntax. Starting from algorithms and working toward types — the way the STL authors intended.

Topics covered: kinds and genera, entities and generalization, value types and equality, objects and identity, procedures and calling conventions, stack frames and argument roles, computational basis and choosing primitives for custom types, generic programming, overload sets, interface design, std::filesystem pitfalls, concepts and `requires`, history of concepts in the C++ standard, partial ordering and subsumption, name mangling, name lookup rules (unqualified, qualified, ADL), two-phase lookup, compiler-specific behaviors (GCC, Clang, MSVC).

---

# Нескучное программирование на C++

Подборка статей о C++ — написанных не так, как учат в вузе или подают в учебниках.

Сначала идеи: обобщённое программирование, дизайн перегрузок, алгоритмическое мышление. Каждая глава есть на русском и английском.

**Новое:** [Вычислительная база (RU/EN)](playfull_programming_chapter_1-5_computational_basis_ru.md)

---

## Главы

| № | Тема | RU | EN |
|---|------|----|----|
| 1.1 | Обобщения | [RU](playfull_programming_chapter_1-1_generalizations_ru.md) | [EN](playfull_programming_chapter_1-1_generalizations_en.md) |
| 1.2 | Значения | [RU](playfull_programming_chapter_1-2_values_ru.md) | [EN](playfull_programming_chapter_1-2_values_en.md) |
| 1.3 | Объекты | [RU](playfull_programming_chapter_1-3_objects_ru.md) | [EN](playfull_programming_chapter_1-3_objects_en.md) |
| 1.4 | Процедуры | [RU](playfull_programming_chapter_1-4_procedures_ru.md) | [EN](playfull_programming_chapter_1-4_procedures_en.md) |
| 1.5 | Вычислительная база | [RU](playfull_programming_chapter_1-5_computational_basis_ru.md) | [EN](playfull_programming_chapter_1-5_computational_basis_en.md) |
| 2 | Перегрузки | [RU](playfull_programming_chapter_2_overloads_ru.md) | [EN](playfull_programming_chapter_2_overloads_en.md) |
| 3 | Концепты и ограничения | [RU](playfull_programming_chapter_3_requires_ru.md) | [EN](playfull_programming_chapter_3_requires_en.md) |
| 4 | История концептов | [RU](playfull_programming_chapter_4_history_ru.md) | [EN](playfull_programming_chapter_4_history_en.md) |
| 5 | Иерархия концептов | [RU](playfull_programming_chapter_5_hierarhy_ru.md) | [EN](playfull_programming_chapter_5_hierarhy_en.md) |
| 6 | И снова ограничения | [RU](playfull_programming_chapter_6_again_requires_ru.md) | [EN](playfull_programming_chapter_6_again_requires_en.md) |
| 7 | Поиск имён в C++ | [RU](playfull_programming_chapter_7_names_lookup_ru.md) | [EN](playfull_programming_chapter_7_names_lookup_en.md) |

### Примеры (C++)

Сниппеты из глав 1.1–1.5 лежат в [`examples/`](examples/README.md) (папки RU и EN). Сборка: `/std:c++20` или `-std=c++20`.

---

## Содержание глав

### Глава 1.1 — Обобщения

- Вид и род: ловушка иерархии `Weapon` и почему «стреляет» это свойство, а не тип
- Абстрактные и конкретные сущности; атрибуты, идентичность и корабль Тесея в коде
- Виды и роды; инвентарь (аптечка, патрон, граната) и шаблоны как правила над видами
- `total_weight` и код для всего, что подчиняется правилу, а не для одной конкретной сущности
- Типы в C++ как виды; как их по-разному видят компилятор, LLVM IR и PDB у MSVC
- Концепты (`C++20`) и модули: проверка ограничений и сериализация типов у каждого вендора

### Глава 1.2 — Значения

- Десинк в сетевых играх: «одно и то же число», которое на самом деле разное; `-ffast-math`, порядок сложения, `+0.0` и `-0.0`
- Биты, интерпретация, корректность представления; `int` и `NaN` как легальный, но некорректный для вещественного смысла float
- Полные и частичные типы: переполнение `int`, сетка `float` на масштабе мира, полный `enum class Suit`
- Уникальное и неуникальное представление; индекс сущности и счётчик поколения
- Равенство и представительное равенство; дроби, кватернионы, множества в контейнерах
- Регулярность, подстановка и почему оптимизатору нужно, чтобы равные входы означали равные выходы

### Глава 1.3 — Объекты

- Указатель башни, который «работает» после смерти юнита: «где лежит» против «кто это»
- Объект как договор программиста, компилятора и машины; состояние как значение, которое может меняться
- Зачем объекты: изменяемая память даже под «чистым» вычислением
- AoS против SoA: кэш-линии, горячие поля, салфеточный расчёт и замеры, когда разнесённая раскладка окупается
- Тип объекта и тип значения; уникальные представления и почему `memcmp` ломается на дырах
- Время жизни: объект начинается после инициализации и кончается с деструктором; пулы переиспользуют адреса
- Токены идентичности отдельно от состояния; равенство против тождественности; корабль Тесея
- COW-строки, cfront и как оптимизатор видит объекты под as-if
- Адреса, которые не переживают реаллокацию; поколения в ECS; указатели для «где», токены для «кто»

### Глава 1.4 — Процедуры

- Подсказка с обрывками чужого имени: возврат `const char*` на стековый буфер
- Кадр принадлежит вызову, а не функции; leaf-функции, красная зона и почему в отладке «работало»
- Сколько кадров влезает: замер размера кадра, стек 1 МБ, воркеры, файберы; рекурсия против стека в куче
- Соглашения о вызовах: регистры против стека, cdecl против stdcall, кто снимает аргументы
- Пролог, эпилог, `enter`/`leave` и переиспользование слотов внутри одного кадра
- Инлайн и LTO обнуляют механику вызова; остаётся договор о том, кто что читает и пишет
- Где живёт состояние: аргументы, локальные, глобальные и скрытый `static`, который переживает новый матч
- Чистый вход / чистый выход / вход-выход; aliasing, `restrict` и почему компилятор боится указателя
- Три правила: наружу значением, не адресом; роли в сигнатуре; контекст передавать явно

### Глава 1.5 — Вычислительная база

- Детерминизм в мультиплеере и почему наивное деление `Fixed` вычитаниями сжигает профайлер
- Минимальная, эффективная и выразительная базы: `next` хватает в теории и не хватает на практике
- Зачем `<=>` и производные сравнения: как тип стыкуется со стандартной библиотекой
- Восемьдесят лет железных баз: от ENIAC и Baby через x86, RISC, SIMD, CRC32C, AVX, VNNI
- Автовекторизация против рукописных интринсиков; когда «в кремнии» всё ещё медленнее SWAR
- Починка `Fixed` целочисленным делением; выбор примитивов, чтобы обёртки наследовали дешёвое основание

### Глава 2 — Перегрузки

- Алгоритм первее типа: почему нужно сначала определить алгоритм, а уже потом выбирать типы
- Множество перегрузок как универсальный интерфейс: одно имя, много типов, никакой неоднозначности — если всё сделано правильно
- Хорошие перегрузки: семантическая согласованность, непересекающиеся области применимости, принцип наименьшего удивления
- Плохие перегрузки: ловушки неявного преобразования, неоднозначность, которая скрывает баги до рантайма
- Разбор `std::filesystem`: ошибки именования в реальном API стандартной библиотеки и что они нам говорят
- Как правильно спроектированные перегрузки масштабируются на обобщённый код без немедленного введения шаблонов

### Глава 3 — Концепты и ограничения

- Почему сообщения об ошибках SFINAE нечитаемы и как концепты это исправляют
- `requires` как контракт: исключение неподходящих кандидатов ещё до начала разрешения перегрузок
- Три синтаксических формы: leading `requires`, trailing `requires` и сокращённые шаблонные функции (`auto`)
- Написание и составление собственных концептов через `&&` и `||`
- Как компилятор разрешает неоднозначность между ограниченными перегрузками
- Пример `Cat != cat`: семантика равенства, типобезопасность и что значит «один и тот же тип»

### Глава 4 — История концептов

- Исходное видение Степанова: концепты как полноценные семантические контракты, а не просто синтаксические проверки
- Предложения начала 1990-х с аксиомами: `EqualityComparable` должен был гарантировать рефлексивность, симметричность и транзитивность — и всё это проверялось бы компилятором
- Почему «тяжёлые» концепты были исключены из C++11 в последний момент после многолетней работы комитета
- Concepts Lite (Эндрю Саттон, 2009): осознанный отказ от семантических требований в пользу только синтаксических проверок
- Ключевой философский сдвиг: от вопроса «математически ли корректен этот тип?» к вопросу «можно ли сгенерировать код с этим типом?»
- Отношения эквивалентности как математический аппарат — почему комитет решил, что компилятор не должен доказывать теоремы
- Что на самом деле проверяют концепты C++20 в сравнении с тем, что обещали ранние предложения

### Глава 5 — Иерархия концептов

- Частичный порядок ограниченных перегрузок: как компилятор выбирает наиболее специализированного кандидата
- Подчинение (subsumption): когда один набор ограничений формально влечёт другой, а когда нет
- Правило синтаксического тождества: почему два логически эквивалентных ограничения считаются компилятором несвязанными
- Практическая ловушка: изменение порядка операндов `&&` в ограничении ломает подчинение и приводит к неоднозначности
- Иерархия итераторов STL как образец осознанно спроектированной иерархии концептов
- `if constexpr` как более читаемая альтернатива глубоким иерархиям концептов
- Предостережения Александреску и Саттера: концепты — инструмент, а не способ продемонстрировать ум

### Глава 6 — И снова ограничения

- Два жёстких ограничения: нельзя рекурсивно определять концепт и нельзя вешать `requires` на само определение концепта — и почему это сделано намеренно
- Почему эти ограничения существуют: концепты должны оставаться быстрыми, предсказуемыми предикатами времени компиляции, а не новым слоем метапрограммирования
- Три синтаксические формы `requires` и когда каждая из них читается лучше
- Концепты на параметрах шаблонного класса: ограничение `Vec3<T>` вещественными типами прямо на границе интерфейса
- `concept auto` в локальных переменных: самодокументирующийся код, который сообщает намерение, а не просто тип
- Условные методы и конструкторы: `requires` на отдельных членах класса позволяет интерфейсу адаптироваться к возможностям типа
- Итог про частичный порядок: синтаксическое тождество по-прежнему важно; выносите составные ограничения в именованный концепт

### Глава 7 — Поиск имён в C++

*Объединяет три статьи: «Важны ли компилятору имена», «Ночью все кошки серы, а using'и одинаковы» и «Компиляторы тоже путаются в именах».*

- Два фундаментальных вопроса компилятора, на которых строятся шаблоны, концепты и перегрузки
- Сосуществование имён против перегрузки: почему функция и структура могут разделять имя, а функция и пространство имён — нет
- Правило «если крякает»: как приоритет функций при поиске имён приводит к неожиданным сообщениям об ошибках
- Манглинг имён из 1980-х: почему `void f(int)` и `void f(const int)` — один и тот же символ для линкера
- `using`-объявление против `using`-директивы: тонкое различие в области видимости, из-за которого две соседние строки тихо обращаются к разным переменным
- CWG Issue 36: как GCC и MSVC принимали код, который C++98 формально запрещал, и как комитет признал ошибку в стандарте
- ADL (поиск Кёнига): история появления, документ Страуструпа 1993 года и бумага Кёнига N0645 (1995)
- Побочные эффекты ADL: почему зависящий от аргументов поиск делает пространства имён менее строгими и может порождать молчаливые семантические конфликты
- Двухфазный поиск имён: независимые имена при определении шаблона, зависимые при инстанциации — и почему это принципиально важно
- «Ленивый парсинг» GCC: как откладывание всего поиска до инстанциации годами позволяло компилировать нестандартный код
- Clang и зависимые базовые классы: почему поиск в зависимой базе запрещён в первой фазе — иллюстрация на примере специализации
- 25-летний технический долг MSVC: как связка с MFC политически защищала нестандартный парсер до VS 2017 и что потребовалось для его исправления

---

## О проекте

В статьях разбирается *зачем* нужны возможности C++, а не только синтаксис. От алгоритмов — к типам, как задумывали авторы STL.

Уже затронуто: виды и роды, сущности и обобщение, типы значений и равенство, объекты и идентичность, процедуры и соглашения о вызовах, стековые кадры и роли аргументов, вычислительная база и выбор примитивов для своих типов, обобщённое программирование, перегрузки, дизайн интерфейсов, подводные камни `std::filesystem`, концепты и ограничения `requires`, история концептов в стандарте C++, частичный порядок и подчинение, манглинг имён, правила поиска имён (неквалифицированный, квалифицированный, ADL), двухфазный поиск, поведение компиляторов (GCC, Clang, MSVC).
