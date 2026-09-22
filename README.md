# modern-cpp-practice

Plan vežbe: od C++03 temelja (životni vek objekta, kopiranje, RAII) do move semantike
i modernog sloja (smart pointeri, forwarding, rule of 0). 10 sesija, ~2h svaka.

Svaka sesija ima svoj folder: `notes.md` (sažetak iz izvora, pisan pre koda) i
`main.cpp` (vežba). Build/pokretanje preko `./build.sh <fajl.cpp>` — kompajlira sa
AddressSanitizer + UndefinedBehaviorSanitizer i `-pedantic-errors` (kod koji
standard zabranjuje je uvek greška, ne samo warning).

```
./build.sh week1-cpp03-to-move/s01-object-lifetime/main.cpp
```

Lekcije koje imaju `errors/` (kod koji ne sme da se kompajlira) i `ub/` (kod
koji ASan/UBSan mora da uhvati pri pokretanju) proveravaju se sa:

```
./check_cases.sh week0-fundamentals/04-pointers-and-references
```

**Windows:** koristi `build.ps1` (isti flegovi). Treba ti MinGW-w64 g++ na
PATH-u — najlakše preko [MSYS2](https://www.msys2.org/)
(`pacman -S mingw-w64-ucrt-x86_64-gcc`, pa dodaj `ucrt64/bin` u PATH) ili
[w64devkit](https://github.com/skeeto/w64devkit).

Ako ti PowerShell odbije da pokrene skriptu ("running scripts is disabled"),
otključaj za tekuću sesiju: `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass`.

Mnogi MSYS2 GCC build-ovi nemaju `libasan`/`libubsan` za mingw target —
ako linker javi `cannot find -lasan`, `build.ps1` to automatski prepozna i
pređe na build BEZ sanitizera (uz upozorenje), pa ništa ne blokira rad.
Za pravi ASan/UBSan na Windows-u instaliraj Clang toolchain umesto GCC-a:
`pacman -S mingw-w64-clang-x86_64-toolchain`, dodaj `clang64/bin` u PATH,
i pokreni sa `.\build.ps1 <fajl> -Compiler clang++`.

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
| [03](week0-fundamentals/03-uniform-init) | Inicijalizacija (kompletno, EMC It. 7) — default/value/direct/copy/list init, narrowing, initializer_list prioritet, agregati, C++20 designated; pogrešni slučajevi u `errors/` |
| [04](week0-fundamentals/04-pointers-and-references) | Pokazivači i reference (kompletno, EMC It. 8, EC++ It. 16/20/21) — `nullptr`, aritmetika, `void*`, `const`, vezivanje referenci i životni vek, prosleđivanje/vraćanje, vlasništvo; pogrešni slučajevi u `errors/` i `ub/` |
| [05](week0-fundamentals/05-compound-types) | Složeni tipovi (EMC It. 9/10) — podela tipova, C nizovi i `std::array`, `enum` vs `enum class`, `union` i `std::variant`, `using` vs `typedef`, funkcijski tipovi; pogrešni slučajevi u `errors/` i `ub/` |
| [06](week0-fundamentals/06-namespaces-linkage) | `namespace`, linkage i `inline` (EC++ It. 4/25/30) — using-deklaracija vs direktiva, ADL i `swap` idiom, external/internal linkage, ODR i šta sme u header, C++17 `inline` promenljive, redosled inicijalizacije između fajlova; dva `.cpp` fajla (build: `./build.sh .../main.cpp .../util.cpp`); pogrešni slučajevi u `errors/` (i linker greške) i `ub/` |
| [07](week0-fundamentals/07-const-qualifier) | `const` (kompletno, EC++ It. 3, EMC It. 13/15/16) — pokazivači i `int**` zamka, const member funkcije, `mutable` i thread-safety, const povratna vrednost, `const_cast`, STL, `constexpr`, top-level vs low-level; pogrešni slučajevi u `errors/` i `ub/` |
| [08](week0-fundamentals/08-auto-range-for) | `auto` i dedukcija tipova (kompletno, EMC It. 1–6) — tri slučaja template dedukcije, `auto`, `decltype`/`decltype(auto)`, prikaz tipa, `vector<bool>` proxy, range-for i životni vek; pogrešni slučajevi u `errors/` i `ub/` |
| [09](week0-fundamentals/09-function-advanced) | Funkcije (EMC It. 11/26, EC++ It. 37) — overloading i overload resolution, overload po vrsti reference, `= delete`, podrazumevani argumenti, pokazivači na funkcije i `std::function`; pogrešni slučajevi u `errors/` i `ub/` |
| [10](week0-fundamentals/10-dynamic-memory) | Dinamička memorija (EC++ It. 16, EMC It. 21) — `malloc`/`free` vs `new`/`delete`, neuspela alokacija, `new[]`/`delete[]` i zašto se oblici ne mešaju, 2D nizovi na četiri načina, `make_unique`/`vector`; pogrešni slučajevi u `errors/` i `ub/` |

### OOP bridge (pre Nedelje 1)

Week1 pretpostavlja da već znaš da praviš klase — ovo popunjava tu rupu.

| Fold. | Tema |
|---|---|
| [11](week0-fundamentals/11-classes-basics) | Klase: osnove (EC++ It. 4/5/6/22, EMC It. 11) — class vs struct i invarijanta, konstruktori i init lista (redosled!), destruktor, NSDMI, `this` i chaining, `static` i `const` članovi, copy konstruktor (plitka vs duboka kopija), delegirajući konstruktori, `= default`/`= delete`; pogrešni slučajevi u `errors/` i `ub/` |
| [12](week0-fundamentals/12-operator-overloading) | Operator overloading (EC++ It. 10/11/23/24) — član vs slobodna funkcija, `friend` (hidden friend), `operator=` i dodela samom sebi, poređenje (C++17 i C++20 `<=>`), stream, `[]`, `++`, `()`, `*`/`->`, šta se ne preopterećuje; pogrešni slučajevi u `errors/` i `ub/` |
| [13](week0-fundamentals/13-inheritance-polymorphism) | Nasleđivanje i polimorfizam (EC++ It. 7/9/32/33/38/39) — pristup i vrste nasleđivanja, redosled konstrukcije, sakrivanje imena, `virtual`/`override`/`final`, virtual destruktor, virtual u konstruktoru, vptr, slicing i `clone()`, dijamant, kompozicija; pogrešni slučajevi u `errors/` i `ub/` |

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
