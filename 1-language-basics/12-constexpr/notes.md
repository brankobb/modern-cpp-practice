# Lekcija 12 — `constexpr`: izračunavanje pri kompajliranju

`constexpr` omogućava da se deo programa izvrši **dok se kompajlira**:
konstante, tabele, provere pravila. Rezultat je ugrađen u program, pa pri
pokretanju ne košta ništa, a greška u takvom izračunavanju je greška pri
kompajliranju. Lekcija 09 (sekcija 9) ima kratko poređenje `const` i
`constexpr`; ovde je cela tema.

**Izvori:** standard, delovi `[dcl.constexpr]`, `[expr.const]` (šta je
konstantni izraz i šta u njemu nije dozvoljeno), `[stmt.if]` (`if constexpr`),
`[dcl.constinit]` i `[meta.const.eval]`. Uz to *Effective Modern C++*
**Item 15** (koristi `constexpr` kad god možeš), i C++ Core Guidelines
**Con.5**, **F.4** i **P.5** (provera pri kompajliranju umesto pri
izvršavanju). Nastavak kursa 91.

**Kako vežbati:**

```
./build.sh 1-language-basics/12-constexpr/main.cpp                   # C++17
./build.sh 1-language-basics/12-constexpr/main_cpp20.cpp -std=c++20  # consteval, constinit, vector/string
./check_cases.sh 1-language-basics/12-constexpr
```

- `errors/` (e01–e09): kod koji se **ne kompajlira**.
- `ub/` (u01–u02): isti kod kao u e01 i e02, ali pozvan pri izvršavanju,
  gde UB prolazi tiho i hvata ga samo UBSan.

---

# 1. `constexpr` funkcija: sme, ne mora

```cpp
constexpr int square(int n) { return n * n; }

constexpr int a = square(12);            // MORA pri kompajliranju: constexpr promenljiva
std::array<int, square(3)> grid{};       // MORA: veličina niza
static_assert(square(12) == 144);        // MORA
int b = square(readSensor());            // pri izvršavanju: argument se zna tek tada
```

`constexpr` na funkciji znači "**sme** da se izvrši pri kompajliranju".
Da li će, zavisi od mesta poziva:

| Kontekst | Kada se izvršava |
|---|---|
| inicijalizacija `constexpr` promenljive, `static_assert`, veličina niza, argument šablona, `case` labela | **mora** pri kompajliranju (ili greška) |
| sve ostalo | kompajler bira; sa argumentima iz runtime-a pri izvršavanju |

Od C++14 u `constexpr` funkciji smeju petlje, lokalne promenljive i više
`return`-ova (`factorial`, `isPrime` u `main.cpp`). C++11 je dozvoljavao
samo jedan `return` izraz.

**Šta ne sme** u izračunavanju pri kompajliranju: poziv ne-`constexpr`
funkcije (`errors/e04`), `reinterpret_cast`, `goto`, `static` lokalne
promenljive (do C++23), `new`/`delete` koji preživi izračunavanje
(C++20 dozvoljava privremenu alokaciju), bacanje izuzetka koji izađe iz
izraza, i **svaki UB**.

---

# 2. UB pri kompajliranju je greška

Kompajler koji računa konstantni izraz **mora** da odbije UB:

| UB | Pri kompajliranju | Pri izvršavanju (ista funkcija) |
|---|---|---|
| signed overflow `add(INT_MAX, 1)` | ❌ greška (`errors/e01`) | tiho; UBSan: `signed integer overflow` (`ub/u01`) |
| indeks van niza `at(3)` | ❌ greška (`errors/e02`) | tiho; UBSan: `index 3 out of bounds` (`ub/u02`) |
| deljenje nulom | ❌ greška (`errors/e03`) | tiho (ili pad programa) |
| `std::array::at` van opsega | ❌ greška (izuzetak nije konstanta, test) | izuzetak |

Zato je `constexpr` + `static_assert` besplatan "sanitizer": logika koja
može da se proveri unapred (tabele, konverzije jedinica, parsiranje
konstantnih stringova) proverava se na **svakom** build-u.

---

# 3. Literal tipovi: sopstvene klase u `constexpr`

```cpp
struct Point {
    int x, y;
    constexpr Point(int px, int py) : x(px), y(py) {}
    constexpr Point operator+(Point o) const { return Point(x + o.x, y + o.y); }
};
constexpr Point target = Point(0, 0) + Point(3, -4);
static_assert(target.manhattan() == 7);
```

- Klasa je **literal tip** kad ima `constexpr` konstruktor (ili je
  agregat), trivijalan destruktor (C++20: `constexpr` destruktor) i
  članove literal tipova.
- `std::array`, `std::pair`, `std::string_view`, `std::optional` su
  literal tipovi. `std::string` i `std::vector` u C++17 **nisu**
  (`errors/e06`).
- C++20: `std::string` i `std::vector` smeju **unutar** `constexpr`
  funkcije (privremena alokacija, `main_cpp20.cpp`), ali `constexpr`
  promenljiva tog tipa prolazi samo ako nije alocirala: kratak string
  (SSO) da, dugačak ne (test: g++ i clang).

---

# 4. Tabele izračunate pri kompajliranju

```cpp
template <std::size_t N>
constexpr std::array<std::uint32_t, N> makeSquares() { ... petlja ... }
constexpr auto squares = makeSquares<16>();
```

Rezultat je gotov podatak u programu. Test: `objdump -t` pokazuje
`squares` u sekciji `.rodata` (read-only). Na mikrokontroleru to znači
tabela u flash-u, a ne u RAM-u, i nula instrukcija pri pokretanju. Tipični
primeri: CRC tabele, sinusne tabele, konverzije jedinica, maske registara.

---

# 5. `if constexpr` (C++17)

```cpp
template <typename T>
std::string describe(const T& value) {
    if constexpr (std::is_same_v<T, std::string>) return "string of length " + std::to_string(value.size());
    else if constexpr (std::is_integral_v<T>)     return "integer " + std::to_string(value);
    else                                           return "something else";
}
```

- Uslov je konstantni izraz, a grana koja ne važi se za taj `T` **ne
  kompajlira**. Kod običnog `if` obe grane moraju biti ispravan kod za
  svaki `T` (`errors/e05`).
- Zamenjuje većinu trikova sa `std::enable_if` i tag dispatch-om (EMC
  Item 27).
- Van šablona `if constexpr` odbacuje granu, ali ona i dalje mora biti
  ispravan kod.

---

# 6. `static_assert`

```cpp
static_assert(sizeof(T) == 4, "Register: the type must be exactly 4 bytes");
static_assert(std::is_trivially_copyable_v<T>, "...");
```

- Provera pri kompajliranju sa sopstvenom porukom (`errors/e09`). U šablonu
  se proverava za **svaki** tip kojim se šablon koristi.
- Poruka je opciona od C++17 (`static_assert(uslov);`).
- Tipična upotreba: veličine struktura koje idu preko mreže ili u
  registre, pretpostavke o platformi (`sizeof(void*) == 8`), ograničenja
  šablona (C++20 ima i `requires`).

---

# 7. C++20: `consteval`, `constinit`, `std::is_constant_evaluated`

| | Značenje | Primer greške |
|---|---|---|
| `consteval` funkcija | **mora** pri kompajliranju | poziv sa `argc` (`errors/e07`) |
| `constinit` promenljiva | inicijalizacija **mora** biti statička, a promenljiva nije `const` | inicijalizacija običnom funkcijom (`errors/e08`) |
| `std::is_constant_evaluated()` | `true` kad se funkcija izvršava pri kompajliranju | — |

- `consteval` kad je poziv sa runtime vrednošću greška u dizajnu (npr.
  provera formata stringa, kao u `std::format`).
- `constinit` rešava redosled inicijalizacije globalnih promenljivih
  između fajlova (lekcija 08, `ub/u01`), a promenljiva ostaje promenljiva
  (test: `limit += 1`).
- `std::is_constant_evaluated()` omogućava jednoj funkciji dva algoritma:
  jednostavan za kompajler, brz (npr. SIMD ili `memcpy`) za izvršavanje.

---

# Pravilo za praksu

✅ `constexpr` za konstante i za funkcije koje mogu da budu `constexpr`
(EMC Item 15): ništa ne košta, a otvara upotrebu pri kompajliranju.

✅ `static_assert` za pretpostavke o tipovima i platformi.

✅ Tabele i konverzije koje se ne menjaju: izračunaj pri kompajliranju.

✅ `if constexpr` umesto `enable_if` za grane po tipu u šablonu.

⚠️ `constexpr` funkcija sa runtime argumentima je obična funkcija: UB je
tada tih kao i inače.

⚠️ `constexpr` ne garantuje izračunavanje pri kompajliranju; garantuje ga
samo kontekst (ili `consteval`).

**Rezime:** `constexpr` pomera računanje iz izvršavanja u kompajliranje,
tamo gde je rezultat poznat unapred. Nagrada je dvostruka: program je brži
i manji (tabele u read-only memoriji), a svaki UB i svaka pogrešna
pretpostavka u tom delu postaju greška pri kompajliranju, umesto greške na
terenu.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVE`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_crc_table`](exercises/ex1_crc_table.cpp) | usage | constexpr funkcija, tabela pri kompajliranju, static_assert i if constexpr (sekcije 1, 4, 5, 6) | — |
| [`ex2_ub_at_compile_time`](exercises/ex2_ub_at_compile_time.cpp) | why | zašto je constexpr i alat za hvatanje UB-a (sekcija 2) | `-DNAIVE`, `-DCONSTEXPR` |
| [`ex3_register_layout`](exercises/ex3_register_layout.cpp) | why | zašto static_assert za pretpostavke o rasporedu u memoriji (sekcija 6) | `-DNAIVE` |

## Zapažanja posle vežbe

