# modern-cpp-practice

Plan vežbe: od osnova jezika (Deo 0), preko C++03 temelja (životni vek
objekta, kopiranje, RAII) i move semantike (Deo 1), do modernog sloja (pametni
pokazivači, forwarding, rule of 0, Deo 2).

## Kako je lekcija organizovana

| Fajl / folder | Šta je |
|---|---|
| `notes.md` | objašnjenje po standardu: ✅ ispravno, ❌ greška, ⚠️ zamka; EMC / EC++ / Core Guidelines; na kraju "Pravilo za praksu", tabela vežbi i prazno "Zapažanja posle vežbe" za tvoje beleške |
| `main.cpp` | samo ispravni primeri, sa označenim izlazom; isti izlaz na g++ i clang, C++17 i C++20 |
| `errors/` | kod koji NE SME da se kompajlira (u komentaru: zašto i kako ispravno) |
| `ub/` | kod koji se kompajlira, a ASan/UBSan ga hvata pri pokretanju |
| `runtime/` | kod koji se kompajlira, a program se prekine iako nije UB (npr. `std::terminate`) |
| `exercises/` | tri zadatka: bar jedan "upotreba" (vežbaš jezik) i bar jedan "zašto" (prvo vidiš problem, pa ga ispraviš); rešenja u `exercises/solutions/` |

Build i pokretanje jednog fajla preko `./build.sh <fajl.cpp>`: kompajlira sa
AddressSanitizer + UndefinedBehaviorSanitizer i `-pedantic-errors` (kod koji
standard zabranjuje je uvek greška, ne samo warning). Dodatni argumenti idu
kompajleru (`-std=c++20`, `-DNAIVNO`, drugi `.cpp` fajl).

```
./build.sh week1-cpp03-to-move/s01-object-lifetime/main.cpp
./build.sh week0-fundamentals/03-uniform-init/exercises/z2_narrowing_senzor.cpp -DNAIVNO
```

Provere (bash; na Windows-u iz MSYS2 shell-a):

```
./check_cases.sh week0-fundamentals/04-pointers-and-references   # errors/, ub/ i runtime/ jedne lekcije
./check_exercises.sh week0-fundamentals/04-pointers-and-references  # zadaci i rešenja (bez argumenta: sve)
./check_refs.py                                                   # reference između lekcija
```

`check_exercises.sh` gradi svaki zadatak i rešenje sa g++ i clang++, u C++17
i C++20, sa `-Werror`: zadatak mora da se kompajlira i nerešen, izlaz rešenja
mora da bude tačno blok OČEKIVANI IZLAZ iz zadatka, a demonstracije problema
(`-DNAIVNO` i sl.) moraju da pokažu ono što tekst tvrdi.

## Kako raditi zadatak

1. Pročitaj korake u komentaru na vrhu fajla.
2. Zadatak "zašto": prvo pokreni sa makroom iz koraka 1 (npr. `-DNAIVNO`) i
   pogledaj problem (pogrešan izlaz, ASan izveštaj ili grešku kompajlera).
   Objasni sebi ZAŠTO se to desilo, pre nego što pišeš ispravku.
3. Piši kod u `#else` grani / na mestu `TODO`, otkomentariši test u
   `main()` i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
4. Tek onda otvori `exercises/solutions/` i uporedi pristup, ne samo izlaz.

## Realan plan

Prvobitni plan ("10 sesija, ~2h svaka") je pisan pre nego što je dodat
Deo 0 i pre nego što je svaka lekcija dobila `errors/`, `ub/` i vežbe.
Procena sada, za lekciju sa svim delovima:

| Deo lekcije | Otprilike |
|---|---|
| `notes.md` + pokretanje `main.cpp` | 1–1.5 h |
| `errors/` i `ub/` (pročitaj, pokreni, objasni) | 0.5–1 h |
| tri zadatka iz `exercises/` | 1.5–2.5 h |

To je 3–5 sati po lekciji, dakle dva sedenja po ~2h. Ukupno: 17 lekcija u
Delu 0 (01–17) + 8 sesija u Delovima 1 i 2 (s05 i s10 su same po sebi
vežbe) ≈ 27 lekcija ≈ 54 sedenja. Uz 5 sedenja nedeljno, to je 10–11
nedelja, a ne dve. Predlog:

- tempo, ne datum: lekcija je gotova kad svi zadaci rade i kad umeš da
  objasniš svaki "zašto" zadatak bez gledanja u rešenje;
- ono što već znaš preleti: pročitaj "Pravilo za praksu" i uradi samo "zašto"
  zadatke; ako prođu bez muke, idi dalje;
- posle svakih 4–5 lekcija jedno sedenje za ponavljanje (tvoje beleške iz
  "Zapažanja posle vežbe").

## Windows (MSYS2 clang64) i razlike

Koristi `build.ps1` (isti flegovi). Treba ti MinGW-w64 g++ na
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

Sve u ovom repozitorijumu je provereno na Linux-u: g++ 13 i clang 18, oba
sa **libstdc++** (GNU standardna biblioteka). MSYS2 clang64 koristi
**libc++** (LLVM standardna biblioteka) i linker lld, pa ponešto može da
izgleda drugačije. Šta verovatno izgleda drugačije (ovde NIJE provereno
na Windows-u):

- `-D_GLIBCXX_ASSERTIONS`, `-D_GLIBCXX_SANITIZE_VECTOR` i `-D_GLIBCXX_DEBUG`
  postoje samo u libstdc++. U libc++ ne rade ništa, pa demonstracije koje ih
  koriste (lekcija 17: sekcija 7 i zadaci z2, z3) tamo izgledaju drugačije;
- LeakSanitizer (curenje memorije: lekcija 10 z3, s01 z2, s08 z2) na
  Windows-u ASan ne podržava, pa tamo curenje vidiš samo po brojaču živih
  objekata koji zadaci ispisuju;
- tekstovi poruka (`what()` izuzetaka, imena iz `typeid`, poruke
  kompajlera) i neke nespecifikovane stvari (stanje moved-from stringa,
  koliko puta `vector` raste) mogu da se razlikuju;
- skripte za proveru su bash -- pokreći ih iz MSYS2 shell-a.

**Molba:** kad pokreneš lekciju na Windows-u i vidiš razliku u odnosu na
ono što piše u `notes.md` ili u bloku OČEKIVANI IZLAZ, zapiši je u
"Zapažanja posle vežbe" te lekcije (šta, sa kojim kompajlerom i flegovima)
i javi -- to ide u lekciju kao proverena razlika.

## Deo 0: osnove (fundamentals audit)

Brz audit osnova (gotchas/edge-case fokus, ne tutorial od nule) — most ka
Delu 1. Grupisano po istoj logici kao kurs čiji je redosled analiziran:
tipovi/I-O/funkcije prvo, pa debugging dok su greške sveže, pa uniform
init/pokazivači/reference/const/auto pre nego što uđeš u klase.

| Fold. | Tema |
|---|---|
| [01](week0-fundamentals/01-types-io-functions) | Primitivni tipovi, I/O, funkcije (kurs 14–17, ES.100–106, F.20) — veličine i `<cstdint>`, `char` i bajtovi, promocije i signed/unsigned, prekoračenje (signed UB, unsigned modulo), deljenje, pokretni zarez, stanje stream-a i `>>` pa `getline`, manipulatori, `[[nodiscard]]`, izlazak bez `return`-a, redosled argumenata; pogrešni slučajevi u `errors/` i `ub/` |
| [02](week0-fundamentals/02-debugging) | Debugging (kurs 18) — debug build, gdb (breakpoint, `next`/`step`/`finish`, uslovni breakpoint, `watch`, `bt` posle pada) sa stvarnim transkriptima, čitanje ASan izveštaja (tri steka, "freed by"/"allocated by"), UBSan nastavlja posle greške, `assert` vs `static_assert` i `NDEBUG`, `-Werror`; bagovi za vežbu u `ub/`, `errors/` |
| [03](week0-fundamentals/03-uniform-init) | Inicijalizacija (kompletno, EMC It. 7) — default/value/direct/copy/list init, narrowing, initializer_list prioritet, agregati, C++20 designated; pogrešni slučajevi u `errors/` |
| [04](week0-fundamentals/04-pointers-and-references) | Pokazivači i reference (kompletno, EMC It. 8, EC++ It. 16/20/21) — `nullptr`, aritmetika, `void*`, `const`, vezivanje referenci i životni vek, prosleđivanje/vraćanje, vlasništvo; pogrešni slučajevi u `errors/` i `ub/` |
| [05](week0-fundamentals/05-compound-types) | Složeni tipovi (EMC It. 9/10) — podela tipova, C nizovi i `std::array`, `enum` vs `enum class`, `union` i `std::variant`, `using` vs `typedef`, funkcijski tipovi; pogrešni slučajevi u `errors/` i `ub/` |
| [06](week0-fundamentals/06-namespaces-linkage) | `namespace`, linkage i `inline` (EC++ It. 4/25/30) — using-deklaracija vs direktiva, ADL i `swap` idiom, external/internal linkage, ODR i šta sme u header, C++17 `inline` promenljive, redosled inicijalizacije između fajlova; dva `.cpp` fajla (build: `./build.sh .../main.cpp .../util.cpp`); pogrešni slučajevi u `errors/` (i linker greške) i `ub/` |
| [07](week0-fundamentals/07-const-qualifier) | `const` (kompletno, EC++ It. 3, EMC It. 13/15/16) — pokazivači i `int**` zamka, const member funkcije, `mutable` i thread-safety, const povratna vrednost, `const_cast`, STL, `constexpr`, top-level vs low-level; pogrešni slučajevi u `errors/` i `ub/` |
| [08](week0-fundamentals/08-auto-range-for) | `auto` i dedukcija tipova (kompletno, EMC It. 1–6) — tri slučaja template dedukcije, `auto`, `decltype`/`decltype(auto)`, prikaz tipa, `vector<bool>` proxy, range-for i životni vek; pogrešni slučajevi u `errors/` i `ub/` |
| [09](week0-fundamentals/09-function-advanced) | Funkcije (EMC It. 11/26, EC++ It. 37) — overloading i overload resolution, overload po vrsti reference, `= delete`, podrazumevani argumenti, pokazivači na funkcije i `std::function`; pogrešni slučajevi u `errors/` i `ub/` |
| [10](week0-fundamentals/10-dynamic-memory) | Dinamička memorija (EC++ It. 16, EMC It. 21) — `malloc`/`free` vs `new`/`delete`, neuspela alokacija, `new[]`/`delete[]` i zašto se oblici ne mešaju, 2D nizovi na četiri načina, `make_unique`/`vector`; pogrešni slučajevi u `errors/` i `ub/` |

### OOP bridge (pre Dela 1)

Week1 pretpostavlja da već znaš da praviš klase — ovo popunjava tu rupu.

| Fold. | Tema |
|---|---|
| [11](week0-fundamentals/11-classes-basics) | Klase: osnove (EC++ It. 4/5/6/22, EMC It. 11) — class vs struct i invarijanta, konstruktori i init lista (redosled!), destruktor, NSDMI, `this` i chaining, `static` i `const` članovi, copy konstruktor (plitka vs duboka kopija), delegirajući konstruktori, `= default`/`= delete`; pogrešni slučajevi u `errors/` i `ub/` |
| [12](week0-fundamentals/12-operator-overloading) | Operator overloading (EC++ It. 10/11/23/24) — član vs slobodna funkcija, `friend` (hidden friend), `operator=` i dodela samom sebi, poređenje (C++17 i C++20 `<=>`), stream, `[]`, `++`, `()`, `*`/`->`, šta se ne preopterećuje; pogrešni slučajevi u `errors/` i `ub/` |
| [13](week0-fundamentals/13-inheritance-polymorphism) | Nasleđivanje i polimorfizam (EC++ It. 7/9/32/33/38/39) — pristup i vrste nasleđivanja, redosled konstrukcije, sakrivanje imena, `virtual`/`override`/`final`, virtual destruktor, virtual u konstruktoru, vptr/vtable i devirtualizacija, slicing i `clone()`, dijamant, kompozicija, apstraktne klase i interfejsi; pogrešni slučajevi u `errors/` i `ub/` |
| [14](week0-fundamentals/14-type-conversions) | Konverzije tipova (EC++ It. 27) — implicitne konverzije, `static_cast`/`dynamic_cast`/`const_cast`/`reinterpret_cast` i zašto ne C-cast, strict aliasing i `bit_cast`, konstruktor i operator konverzije (`explicit operator bool`), korisnički → korisnički tip, proverena konverzija brojeva, `typeid` i RTTI; pogrešni slučajevi u `errors/` i `ub/` |
| [15](week0-fundamentals/15-strings) | Stringovi (kurs 85, 86, 88, 90) — literali i raw stringovi, `std::string` (npos, bajtovi vs slova, `c_str()` životni vek), brojevi ↔ tekst (`stoi`, `from_chars`), string streams, korisnički literali (`""s`, `ms`, `_km`); pogrešni slučajevi u `errors/` i `ub/` |
| [16](week0-fundamentals/16-constexpr) | `constexpr` (kurs 91, EMC It. 15) — sme vs mora pri kompajliranju, UB u konstantnom izrazu je greška, literal tipovi, tabele u `.rodata`, `if constexpr`, `static_assert`; C++20 `consteval`/`constinit` u `main_cpp20.cpp`; pogrešni slučajevi u `errors/` i `ub/` |
| [17](week0-fundamentals/17-vector-initializer-list) | `std::vector` i `std::initializer_list` (kurs 92, 93) — pravljenje, size vs capacity, reserve vs resize, pristup i izmene, `initializer_list` (const elementi, životni vek niza), šta ASan ne vidi i `-D_GLIBCXX_ASSERTIONS`; pogrešni slučajevi u `errors/` i `ub/` |

## Deo 1: od C++03 temelja do move semantike

| Ses. | Tema | Ključno | Izvor |
|---|---|---|---|
| [1](week1-cpp03-to-move/s01-object-lifetime) | Životni vek objekta | trajanje skladišta; redosled u bloku, nizu i klasi; privremeni objekti i produženje života; izuzetak u konstruktoru; `std::exit`; redosled uništavanja static objekata; `ub/` | EC++ It. 4, 13; standard `[basic.life]` |
| [2](week1-cpp03-to-move/s02-copying) | Kopiranje | šta kompajler piše; kada se kopija pravi; kada je obrisana; plitka vs duboka kopija; rule of 3; kopiraj sve delove; rule of 0; `errors/` i `ub/` | EC++ It. 5, 6, 11, 12 |
| [3](week1-cpp03-to-move/s03-raii) | RAII i exception safety | RAII klasa; stack unwinding; resursi u članovima; basic/strong/nothrow sa testom; destruktor ne baca; `unique_ptr` sa deleterom, scope guard; `errors/` i `ub/` | EC++ It. 8, 13, 14, 29 |
| [4](week1-cpp03-to-move/s04-move-semantics) | Move semantika (kurs 53–55, 58) | kategorije vrednosti; vezivanje referenci; `std::move` je cast; move ctor/dodela; imenovana `T&&` je lvalue; move na `const` kopira; moved-from stanje; kad move nije jeftin; `errors/` i `ub/` | EMC It. 23, 25, 29 |
| [5](week1-cpp03-to-move/s05-buffer-exercise) | Vežba `Buffer` (kurs 56) | `exercise.cpp` (kostur) i `main.cpp` (rešenje): rule of 3 → 5 → 0; merenje kopija/move-ova; `noexcept` i realokacija `vector`-a; `errors/` i `ub/` | C.20, C.21, C.66 |

## Deo 2: moderni sloj

| Ses. | Tema | Ključno | Izvor |
|---|---|---|---|
| [6](week2-modern-layer/s06-generation-rules) | Pravila generisanja | tabela generisanja proverena kodom; destruktor ukida move; `= default` i `noexcept`; `vector` i `move_if_noexcept`; šablonski konstruktor otima kopiju; move-only tipovi; `errors/` i `ub/` | EMC It. 11, 14, 17, 26 |
| [7](week2-modern-layer/s07-copy-elision) | Copy elision i parametri (kurs 57) | RVO/NRVO i pessimizing move sa testom; automatski move na `return` (i razlika g++/clang, P1825); garantovana elizija; izbor oblika parametra; `emplace_back`; `errors/` | EMC It. 25, 41, 42 |
| [8](week2-modern-layer/s08-smart-pointers) | Pametni pokazivači (kurs 72–82) | `unique_ptr` i vlasništvo u parametrima; `shared_ptr` i kontrolni blok; make funkcije (brojanje alokacija); `weak_ptr`; ciklusi; deleteri; nizovi; `enable_shared_from_this`; `errors/` i `ub/` | EMC It. 18–22 |
| [9](week2-modern-layer/s09-forwarding-lifetime) | Forwarding i lifetime zamke | forwarding vs rvalue referenca; reference collapsing; `std::forward` vs `std::move`; kada forwarding ne radi; `string_view`; invalidacija iteratora; lambda capture; `errors/` i `ub/` | EMC It. 24, 25, 28, 30–32 |
| [10](week2-modern-layer/s10-uniqueptr-embedded) | Vežba | `exercise.cpp` (kostur) i `main.cpp` (rešenje): sopstveni `UniquePtr`; guard za prekide koji vraća prethodno stanje; `std::lock_guard` sa `SpinLock`; objekat bez heap-a (placement new, `alignas`, `std::launder`); `errors/` i `ub/` | — |

## Deo 3: dodatne teme (`week3-advanced/`)

Teme koje nadograđuju Delove 0–2: izuzeci, C++17 novine, šabloni... Isti
format kao ostale lekcije; numeracija sesija se nastavlja (s11, s12...).

| Ses. | Tema | Ključno | Izvor |
|---|---|---|---|
| [11](week3-advanced/s11-exceptions) | Izuzeci (kurs 114–119) | `throw`/`try`/`catch` i hijerarhija `std::exception`; redosled `catch` blokova; sopstveni tip izuzetka; stack unwinding; `throw;` vs `throw e;`; `std::nested_exception`; function-try-block, destruktor ne baca; `noexcept` specifikator i operator; `std::exception_ptr`; `errors/` i `runtime/` (`std::terminate`) | EC++ It. 8, 29; EMC It. 14; E.2, E.14–E.16 |

## Provera posle Dela 2

Kad završiš s10 (ne na određeni datum). Odgovori naglas, oko 2 minuta po
pitanju. Zapiši odgovore u [ANSWERS.md](ANSWERS.md) pre nego što ih izgovoriš.

1. Šta kompajler generiše ako deklarišeš samo destruktor?
2. Zašto `std::move` sam po sebi ništa ne pomera?
3. Zašto move konstruktor treba da bude `noexcept`?
4. Kada `return std::move(x)` šteti?
5. `unique_ptr` vs `shared_ptr`: kolika je cena i kada koji?
6. Kako copy-and-swap daje strong garanciju?
7. Kako izgleda RAII bez heap-a u embedded kodu?

Ako sigurno odgovoriš na bar 5 pitanja, prelaziš na STL. U suprotnom, prva dva
sedenja posle toga idu na ponavljanje: "zašto" zadaci iz week1 i week2, bez
gledanja u rešenja.
