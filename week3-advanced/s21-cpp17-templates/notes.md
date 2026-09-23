# Sesija 21 — C++17 novine za šablone (kurs 216–223)

C++17 je šablone učinio kraćim na četiri mesta: **CTAD** (argumenti
šablona klase se zaključe iz konstruktora), **fold izrazi** (paket se
"savije" jednim operatorom, bez rekurzije), **`_v` sufiksi** za type
traits, i **`if constexpr`** (grana po tipu, bez `enable_if` i
overload-a). Sve četiri su se već pojavile u ranijim lekcijama; ova
sesija ih skuplja i ide dublje, sa zamkama.

Već obrađeno, ovde samo upućujemo:

- `if constexpr` osnovno, i šta radi van šablona: lekcija 16, sekcija 5;
- variadic šabloni, prvi fold izrazi, prazan paket: s13, sekcija 2;
- klasni šabloni, prvi primer CTAD-a sa deduction guide-om: s13,
  sekcija 3;
- type traits (pitanja, transformacije): s13, sekcija 7;
- dedukcija argumenata funkcijskog šablona: s12, sekcija 2.

**Izvori:** standard, `[over.match.class.deduct]` (kako CTAD bira),
`[temp.deduct.guide]` (deduction guide), `[expr.prim.fold]` (fold izrazi,
prazan paket), `[meta]` (`_v` i `_t`), `[stmt.if]` (`if constexpr`,
odbačena grana). Knjige: *C++ Templates: The Complete Guide* (2. izd.,
Vandevoorde, Josuttis, Gregor) i *C++17 -- The Complete Guide*
(Josuttis) -- *Effective Modern C++* je pisan za C++14 i ove teme nema.

**Kako vežbati:**

```
./build.sh week3-advanced/s21-cpp17-templates/main.cpp              # svi ISPRAVNI slučajevi
./check_cases.sh week3-advanced/s21-cpp17-templates                 # svi POGREŠNI slučajevi
./check_exercises.sh week3-advanced/s21-cpp17-templates             # vežbe
```

- `errors/` (e01–e09): kod koji se **ne kompajlira**. Sve je u šablonima
  i pri kompajliranju -- `ub/` i `runtime/` ova sesija nema.
- `main.cpp` proverava zaključene tipove sa `static_assert(std::is_same_v<...>)`:
  ako se kompajlira, tip je baš taj.

---

# 1. CTAD: dedukcija argumenata šablona klase (kurs 216)

```cpp
std::pair p{1, 2.5};                 // pair<int, double>
std::vector v{1, 2, 3};              // vector<int>
std::array a{1, 2, 3};               // array<int, 3>
std::lock_guard g(m);                // lock_guard<std::mutex>
std::function f = [](int x) { return x * 2.0; };   // function<double(int)>
```

Kompajler od svakog konstruktora napravi "zamišljenu" funkciju-šablon i
na nju primeni običnu dedukciju (s12, sekcija 2). Test: `pair`, `tuple`,
`vector`, `array`, `optional`, `lock_guard`, `function`, i sopstveni
`Merenje{"temp", 21.5}` → `Merenje<double>`.

- **Deduction guide**, kad iz konstruktora ne može da se zaključi:

  ```cpp
  template <typename It>
  Opseg(It, It) -> Opseg<typename std::iterator_traits<It>::value_type>;
  ```

  Konstruktor prima iteratore, a `T` je tip elementa -- to kaže vodič
  (test: `Opseg<int>`; zadatak z1).
- ❌ Sve ili ništa: `std::pair<int> p{1, 2}` se ne kompajlira
  (`errors/e01`).
- ❌ Agregat (nema konstruktor) u C++17 nema CTAD bez vodiča
  (`errors/e02`); C++20 ga zaključuje i bez vodiča.
- ❌ `std::array a{1, 2.5, 3}`: vodič traži iste tipove (`errors/e03`).
- ⚠️ **Kopija ima prednost**: `std::vector v2{v}` je `vector<int>` (kopija
  od `v`), a ne `vector<vector<int>>`; `std::vector{v, v}` jeste vektor
  vektora (test).
- ⚠️ **String literal daje `const char*`**: `std::pair{"temp", 1}` je
  `pair<const char*, int>`. U mapi ključevi se tada porede kao adrese,
  pa pretraga po tekstu iz bafera ne nađe ništa, bez upozorenja
  (zadatak z3). ✅ `"temp"s` (`std::string_literals`) ili eksplicitan tip.
- ✅ CTAD kad je tip očigledan iz argumenata (`lock_guard`, `pair`,
  `array`); eksplicitan tip kad nije (literali, `{}` sa jednim
  kontejnerom).

---

# 2. Fold izrazi: četiri oblika (kurs 217–220)

| Oblik | Zapis | Za `a1, a2, a3` |
|---|---|---|
| unarni desni | `(a op ...)` | `a1 op (a2 op a3)` |
| unarni levi | `(... op a)` | `(a1 op a2) op a3` |
| binarni desni | `(a op ... op init)` | `a1 op (a2 op (a3 op init))` |
| binarni levi | `(init op ... op a)` | `((init op a1) op a2) op a3` |

- Test sa `-` i `10, 3, 2`: `(a - ...)` = 9, `(... - a)` = 5,
  `(a - ... - 100)` = -91, `(100 - ... - a)` = 85. Za `+` i `*` smer ne
  menja rezultat (15), za `-`, `/`, `<<` menja (zadatak z2).
- **Prazan paket**: unarni fold radi samo za `&&` (`true`), `||`
  (`false`) i zarez (`void()`); za ostale je greška (s13). ✅ Binarni fold
  sa početnom vrednošću radi uvek: `(0 + ... + a)` = 0 (test).
- ❌ Zagrade su deo sintakse (`errors/e04`); operand mora biti prost
  izraz: `(args * 2 + ...)` je greška, `((args * 2) + ...)` nije
  (`errors/e05`).
- "Levi" i "desni" se odnose na **grupisanje** (gde su zagrade):
  `(... + a)` je `(a1 + a2) + a3`. Redosled izračunavanja samih operanada
  je, kao i bez fold-a, za većinu operatora neodređen; za zarez, `&&` i
  `||` je s leva na desno.

---

# 3. Fold u praksi (kurs 219–220)

```cpp
((std::cout << sep << a, sep = ", "), ...);   // ispis sa separatorom
(std::cout << ... << a);                      // binarni levi nad <<
(c.push_back(std::forward<T>(a)), ...);       // poziv po elementu
((a > 0) + ... + 0)                           // brojanje: bool -> 0/1
(std::is_integral_v<T> && ...)                // fold nad TIPOVIMA
```

- **Zarez**: najkorisniji fold -- "uradi ovo za svaki element". Levi
  operand zareza se izvrši pre desnog, pa ide redom s leva na desno (test:
  `1, dva, 3.5, c`).
- Fold nad tipovima pravi uslov pri kompajliranju: `sviCeli<int, char,
  long>` je `true`, `sviCeli<int, double>` je `false` (test,
  `static_assert`).
- ✅ Fold umesto rekurzije sa osnovnim slučajem (s13): jedan izraz, bez
  dodatnih instanci funkcije.

---

# 4. Type traits: sufiksi `_v` i `_t` (kurs 221)

| Duže | Kraće | Od |
|---|---|---|
| `std::is_integral<T>::value` | `std::is_integral_v<T>` | C++17 |
| `typename std::remove_reference<T>::type` | `std::remove_reference_t<T>` | C++14 |

- Kraći oblici su samo prečice, napravljene ovako (i sopstveni trait se
  pravi isto, test `jeString_v`, `bezPokazivaca_t`):

  ```cpp
  template <typename T> inline constexpr bool is_integral_v = is_integral<T>::value;  // variable template
  template <typename T> using remove_reference_t = typename remove_reference<T>::type; // alias template
  ```

- ❌ Duži `::type` oblik u šablonu traži `typename` (`errors/e06`) --
  `::type` zavisi od `T`, pa kompajler ne zna da je tip. `_t` to sakrije.
- ✅ U novom kodu uvek `_v` i `_t`.

---

# 5. `if constexpr`: grana po tipu (kurs 222)

```cpp
template <typename T>
std::string uTekst(const T& x) {
    if constexpr (std::is_same_v<T, bool>)         return x ? "da" : "ne";
    else if constexpr (std::is_integral_v<T>)      return "ceo " + std::to_string(x);
    else if constexpr (std::is_convertible_v<T, std::string>) return std::string(x);
    else                                            return "kontejner od " + std::to_string(x.size());
}
```

- Uslov se računa pri kompajliranju; grana koja nije izabrana se u
  šablonu **ne instancira** -- zato `x.size()` sme da stoji u funkciji koju
  zovemo i sa `int` (test: šest tipova). Sa običnim `if` bi se sve grane
  prevele za svaki tip.
- Redosled je bitan: `bool` je i `is_integral`, pa ide prvi (test: `da`).
- Grane mogu da vraćaju **različite tipove** kad je povratni tip `auto`
  (test: `udvostruci(3)` je `int`, `udvostruci("ab"s)` je `std::string`).
- ❌ Odbacuje se samo grana: kod **posle** `if constexpr` bez `else` se
  instancira uvek (`errors/e07`). ✅ Poslednji slučaj u `else`.
- ❌ Uslov mora biti konstantan izraz; parametar funkcije nije
  (`errors/e08`). `if constexpr` je za uslove od tipova i konstanti.
- Zamenjuje `std::enable_if` / SFINAE overload-e i "tag dispatch" za
  većinu slučajeva: jedna funkcija umesto dve-tri.
- ⚠️ `static_assert(false, "...")` u odbačenoj grani: po C++17 tekstu
  standarda je to greška čak i kad se grana ne izabere. Ispravka
  (P2593, usvojena kao ispravka i za starije standarde) to dozvoljava:
  g++ 13 i clang 18 prihvataju i sa `-std=c++17` (provereno), stariji
  kompajleri odbiju. Prenosivo: uslov koji zavisi od `T`, pa se
  proverava tek pri instancijaciji:

  ```cpp
  template <typename> inline constexpr bool uvekNetacno = false;
  ...
  else static_assert(uvekNetacno<T>, "nepodržan tip");   // greška samo ako se grana izabere
  ```

---

# 6. `if constexpr`: rekurzija i kompajl-vreme (kurs 223)

```cpp
template <typename Prvi, typename... Ostali>
void ispisiRekurzivno(const Prvi& p, const Ostali&... ostali) {
    std::cout << p;
    if constexpr (sizeof...(ostali) > 0) ispisiRekurzivno(ostali...);
}
```

- Kraj rekurzije bez posebnog overload-a za prazan paket (s13, sekcija
  2): za 0 ostalih se poziv ne instancira (test: `temp -> 21 -> C -> 3.5`).
- Isto za rekurziju po ne-tipskom parametru: `fakt<N>()` sa
  `if constexpr (N <= 1)`; sa običnim `if` se instancira i `fakt<0>`,
  `fakt<-1>`... do granice kompajlera: 900 nivoa u g++, 1024 u clang-u
  (`errors/e09`). Test: `static_assert(fakt<5>() == 120)`.
- ✅ Kad se paket može obraditi fold-om (sekcija 3), fold je kraći od
  rekurzije; `if constexpr` rekurzija ostaje za slučajeve kad se prvi
  element obrađuje drugačije od ostalih.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 216 | Class Template Argument Deduction (CTAD) | sekcija 1; `errors/e01`, `e02`, `e03`; zadaci z1, z3 |
| 217 | Folding Basics | sekcija 2 |
| 218 | Fold Expressions - Unary Folds | sekcija 2; `errors/e04` |
| 219 | Fold Expressions - Binary Folds | sekcije 2, 3; `errors/e05`; zadatak z2 |
| 220 | Fold Expressions - Recap | sekcija 3; zadatak z1 |
| 221 | Type Traits Suffixes | sekcija 4; `errors/e06` |
| 222 | if constexpr - I | sekcija 5; `errors/e07`, `e08`; zadatak z1 |
| 223 | if constexpr - II | sekcija 6; `errors/e09` |

---

# Pravilo za praksu

✅ CTAD kad je tip očigledan iz argumenata; deduction guide kad
konstruktor ne otkriva `T`.

⚠️ CTAD sa string literalom daje `const char*`, a `{kontejner}` pravi
kopiju, ne kontejner kontejnera.

✅ Fold umesto rekurzije za "uradi za svaki element" (zarez) i za
sabiranje/proveru svih (`+`, `&&`).

⚠️ Za operator koji nije asocijativan (`-`, `/`) izaberi smer fold-a
svesno; za mogući prazan paket koristi binarni fold sa početnom
vrednošću.

✅ `_v` i `_t` umesto `::value` i `typename ...::type`.

✅ `if constexpr` umesto `enable_if` overload-a; poslednja grana u `else`.

**Rezime:** CTAD pomera zaključivanje tipova sa funkcija na klase, fold
izrazi obrađuju paket jednim izrazom, `_v`/`_t` skraćuju traits, a
`if constexpr` pravi grane koje postoje samo za tipove za koje imaju
smisla. Zajedno čine da generički kod izgleda skoro kao običan.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_generican_zapis`](exercises/z1_generican_zapis.cpp) | upotreba | deduction guide, fold izrazi i if constexpr u jednom generičkom zapisu merenja (sekcije 1, 2, 3, 5) | — |
| [`z2_smer_folda`](exercises/z2_smer_folda.cpp) | zašto | zašto je bitno da li je fold levi ili desni (sekcija 2) | `-DNAIVNO` |
| [`z3_ctad_literal`](exercises/z3_ctad_literal.cpp) | zašto | zašto CTAD od string literala ne daje std::string (sekcija 1) | `-DNAIVNO` |

## Zapažanja posle vežbe
