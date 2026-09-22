# modern-cpp-practice

Plan vežbe: od C++03 temelja (životni vek objekta, kopiranje, RAII) do move semantike
i modernog sloja (smart pointeri, forwarding, rule of 0). 10 sesija, ~2h svaka.

Svaka sesija ima svoj folder: `notes.md` (sažetak iz izvora, pisan pre koda) i
`main.cpp` (vežba). Build/pokretanje preko `./build.sh <fajl.cpp>` — kompajlira sa
AddressSanitizer + UndefinedBehaviorSanitizer.

```
./build.sh week1-cpp03-to-move/s01-object-lifetime/main.cpp
```

**Windows:** koristi `build.ps1` (isti flegovi). Treba ti MinGW-w64 g++
(GCC 12+ za ASan na Windows-u) na PATH-u — najlakše preko
[MSYS2](https://www.msys2.org/) (`pacman -S mingw-w64-ucrt-x86_64-gcc`, pa
dodaj `ucrt64/bin` u PATH) ili [w64devkit](https://github.com/skeeto/w64devkit).

```powershell
.\build.ps1 week1-cpp03-to-move\s01-object-lifetime\main.cpp
```

## Nedelja 0: fundamentals audit

Brz audit osnova (gotchas/edge-case fokus, ne tutorial od nule) — most ka
Nedelji 1. Grupisano po istoj logici kao kurs čiji je redosled analiziran:
tipovi/I-O/funkcije prvo, pa debugging dok su greške sveže, pa uniform
init/pokazivači/reference/const/auto pre nego što uđeš u klase.

| Fold. | Tema |
|---|---|
| [01](week0-fundamentals/01-types-io-functions) | Primitivni tipovi, I/O, funkcije — overflow, integer promotion, cin fail state |
| [02](week0-fundamentals/02-debugging) | Debugging (gdb umesto VS) — namerni buffer overflow, uhvati ga breakpoint-om i ASan-om |
| [03](week0-fundamentals/03-uniform-init) | Uniform initialization — narrowing, most vexing parse, initializer_list preferencija |
| [04](week0-fundamentals/04-pointers-and-references) | Pokazivači → reference — array decay, dangling pointer, referenca mora biti inicijalizovana |
| [05](week0-fundamentals/05-assignment-quizzes) | Kviz: pokazivači i reference — predict-the-output |
| [06](week0-fundamentals/06-reference-vs-pointer) | Reference vs pointer — kad koji, slicing kroz prosleđivanje po vrednosti |
| [07](week0-fundamentals/07-const-qualifier) | const Qualifier — `const int*` vs `int* const`, mutable, const_cast |
| [08](week0-fundamentals/08-auto-range-for) | Auto → range-based for — auto skida const/ref, `auto&` vs `const auto&` u petlji |
| [09](week0-fundamentals/09-function-advanced) | Overloading, default args, inline, function pointers, namespace |

### OOP bridge (pre Nedelje 1)

Week1 pretpostavlja da već znaš da praviš klase — ovo popunjava tu rupu.

| Fold. | Tema |
|---|---|
| [10](week0-fundamentals/10-classes-encapsulation) | Klase, enkapsulacija, `this`, static članovi — struct vs class, method chaining, instance counter |
| [11](week0-fundamentals/11-operator-overloading) | Operator overloading — member vs free function, `operator[]`, `operator<<`, `operator==` |
| [12](week0-fundamentals/12-inheritance-polymorphism) | Nasleđivanje i polimorfizam — abstraktne klase, `override`, dynamic dispatch, veza sa virtual destructor/slicing iz ranijih sesija |

## Nedelja 1 (28.9 – 4.10): od C++03 temelja do move semantike

| Ses. | Tema | Ključno | Izvor |
|---|---|---|---|
| [1](week1-cpp03-to-move/s01-object-lifetime) | Životni vek objekta | automatic/static/dynamic storage; kada se zovu ctor/dtor; redosled konstrukcije baza i članova; init lista ide po redosledu deklaracije | Effective C++ (3. izd.) st. 5–14, 29; learncpp.com (konstruktori) |
| [2](week1-cpp03-to-move/s02-copying) | Kopiranje (C++03) | copy ctor i copy assignment; šta kompajler generiše; shallow vs deep copy; self-assignment; rule of 3 | Effective C++ st. 5–14, 29; learncpp.com (kopiranje) |
| [3](week1-cpp03-to-move/s03-raii) | RAII i exception safety | stack unwinding; basic/strong/nothrow garancija; copy-and-swap; zašto destruktor ne sme da baca | Effective C++ st. 5–14, 29; Arthur O'Dwyer, *Back to Basics: RAII and the Rule of Zero* (CppCon 2019) |
| [4](week1-cpp03-to-move/s04-move-semantics) | Move semantika | lvalue/rvalue, `T&&`, `std::move` je samo cast, move ctor/assignment, moved-from stanje | Effective Modern C++ st. 23; Klaus Iglberger, *Back to Basics: Move Semantics* (CppCon 2019) |
| [5](week1-cpp03-to-move/s05-buffer-exercise) | Vežba | klasa `Buffer` (dinamički niz): prvo rule of 3 + copy-and-swap, pa dodaj move (rule of 5); proveri ASan-om | — |

## Nedelja 2 (5.10 – 11.10): moderni sloj

| Ses. | Tema | Ključno | Izvor |
|---|---|---|---|
| [6](week2-modern-layer/s06-generation-rules) | Pravila generisanja | kada se special members generišu ili brišu; `= default`, `= delete`; `noexcept` move i realokacija `vector`-a | Effective Modern C++ st. 11, 14, 17, 29 |
| [7](week2-modern-layer/s07-copy-elision) | Copy elision i parametri | RVO/NRVO, C++17 garancija, zašto `return std::move(x)` šteti; prosleđivanje po vrednosti + move | Effective Modern C++ st. 41 |
| [8](week2-modern-layer/s08-smart-pointers) | Smart pointeri | `unique_ptr` (deleter, move-only, bez cene), `shared_ptr` (kontrolni blok, atomski brojač), `weak_ptr`, `make_unique`/`make_shared` | Effective Modern C++ st. 18–21 |
| [9](week2-modern-layer/s09-forwarding-lifetime) | Forwarding i lifetime zamke | forwarding reference, `std::forward`, reference collapsing; dangling na temporary, `string_view`, invalidacija iteratora | Effective Modern C++ st. 24, 25, 28 |
| [10](week2-modern-layer/s10-uniqueptr-embedded) | Vežba | sopstveni `UniquePtr<T>`; `Buffer` prepiši u rule of 0; embedded: RAII guard za lock/IRQ i placement new na statičkom baferu | — |

## Provera 11.10

Odgovori naglas, oko 2 minuta po pitanju. Zapiši odgovore u [ANSWERS.md](ANSWERS.md) pre nego što ih izgovoriš.

1. Šta kompajler generiše ako deklarišeš samo destruktor?
2. Zašto `std::move` sam po sebi ništa ne pomera?
3. Zašto move konstruktor treba da bude `noexcept`?
4. Kada `return std::move(x)` šteti?
5. `unique_ptr` vs `shared_ptr`: kolika je cena i kada koji?
6. Kako copy-and-swap daje strong garanciju?
7. Kako izgleda RAII bez heap-a u embedded kodu?

Ako sigurno odgovoriš na bar 5 pitanja, prelaziš na STL. U suprotnom, prve dve sesije
sledeće nedelje idu na ponavljanje.
