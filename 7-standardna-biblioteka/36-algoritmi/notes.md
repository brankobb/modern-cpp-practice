# Lekcija 36 — Složenost, algoritmi, izmene kontejnera u C++11 (kurs 179–187)

Algoritmi (`<algorithm>`, `<numeric>`) su treći deo STL-a: funkcije koje
rade nad **opsegom iteratora** `[first, last)` i ne znaju za kontejner.
Zato jedan `std::find` radi za `vector`, `list`, `set` i C niz. Cena
toga je da algoritam ne može da promeni **veličinu** kontejnera, i to
objašnjava većinu zamki u ovoj lekciji. Na kraju su izmene koje je C++11
(i C++17) doneo samim kontejnerima.

Već obrađeno, ovde samo upućujemo:

- kontejneri i kategorije iteratora: lekcija 34 (sekcija 1) i lekcija 35;
- `std::initializer_list`: lekcija 07, sekcija 6;
- `emplace_back` vs `push_back`: lekcija 24, sekcija 4;
- move pri rastu vektora samo ako je `noexcept`: lekcija 23, sekcija 4;
- lambde kao predikati i komparatori: lekcija 30.

**Izvori:** standard, `[algorithms]`: `[alg.nonmodifying]`,
`[alg.modifying.operations]`, `[alg.sorting]` (sa `[alg.binary.search]`,
`[alg.set.operations]`, `[alg.merge]`), `[numeric.ops]` (`accumulate`,
`reduce`, `iota`); `[iterator.primitives]` (`distance`), `[inserter]`
(`back_inserter`); `[iterator.range]` (`std::begin`, `std::size`,
`std::data`). *Effective STL* **Item 30** (odredište mora da ima mesta),
**Item 31** (izbor algoritma za sortiranje), **Item 32** (`erase` posle
`remove`), **Item 43** (algoritam umesto ručne petlje). Core Guidelines
**ES.1** (standardna biblioteka umesto ručnog koda), **P.3** (izrazi
nameru).

**Kako vežbati:**

```
./build.sh 7-standardna-biblioteka/36-algoritmi/main.cpp                   # svi ISPRAVNI slučajevi
./check_cases.sh 7-standardna-biblioteka/36-algoritmi                      # svi POGREŠNI slučajevi
./check_exercises.sh 7-standardna-biblioteka/36-algoritmi                  # vežbe
```

- `errors/` (e01–e05): kod koji se **ne kompajlira** -- greška se često
  prijavi duboko u `<algorithm>`, ne na liniji poziva.
- `ub/` (u01–u02): `std::copy` u odredište bez mesta; `u02` se hvata samo
  uz `-D_GLIBCXX_SANITIZE_VECTOR` (u zaglavlju fajla).
- Broj poređenja za `set::find` i `sort` u `main.cpp` je za libstdc++;
  libc++ može da da druge (red veličine je isti).

---

# 1. Složenost: "veliko O" (kurs 179)

Složenost kaže kako **raste** cena kad raste broj elemenata `n`, bez
konstanti. `main.cpp` broji poređenja preko komparatora sa brojačem
(`struct Manje`), za 1024 sortirana broja:

| Operacija | Složenost | Izmereno (test) |
|---|---|---|
| `find_if` do elementa 1000 | O(n) | 1001 |
| `binary_search` | O(log n) | 11 (log2(1024) = 10, +1) |
| `set::find` | O(log n) | 16 (crveno-crno stablo je dublje od log2 n) |
| `sort` 1000 izmešanih | O(n log n) | 11729 (n·log2(n) ≈ 9966) |

| Klasa | Primer | n = 1000 → n = 1 000 000 |
|---|---|---|
| O(1) | `v[i]`, `push_back` (amortizovano), `unordered_map::find` (prosek) | isto |
| O(log n) | `binary_search`, `set::find`, `map::insert` | ~10 → ~20 koraka |
| O(n) | `find`, `count`, `copy`, `accumulate`, `vector::insert` u sredinu | 1000 puta više |
| O(n log n) | `sort`, `stable_sort` | ~2000 puta više |
| O(n²) | dve ugnježdene petlje (npr. "za svaki element proveri sve ostale") | milion puta više |

- Standard propisuje složenost svakog algoritma i operacije kontejnera
  (npr. `[sort]`: O(n log n) poređenja). ✅ `sort` je od C++11
  **garantovano** O(n log n); pre toga samo u proseku.
- ⚠️ Složenost nije brzina: za malo `n` konstante i keš odlučuju (lekcija 34,
  sekcija 5 -- `vector` protiv `list`). Za veliko `n` klasa odlučuje.
- ⚠️ Sortiran `vector` + `binary_search` je O(log n) kao `set::find`, sa
  manje memorije i boljim kešom -- ako se retko menja.

---

# 2. Algoritmi koji ne menjaju opseg (kurs 180)

```cpp
std::find(b, e, x)          std::find_if(b, e, pred)       // iterator ili e
std::count(b, e, x)         std::count_if(b, e, pred)
std::all_of / any_of / none_of(b, e, pred)                  // C++11
std::min_element / max_element / minmax_element(b, e)      // iterator(i)
std::equal(b1, e1, b2)      std::mismatch(b1, e1, b2)      // prva razlika
std::accumulate(b, e, init) std::accumulate(b, e, init, op) // <numeric>
```

- **Predikat** prima jedan element i vraća `bool` (`[](int x) { return x % 2; }`).
  **Komparator** prima dva i vraća "prvi ide pre drugog" -- lambda sa
  jednim parametrom kao komparator je greška (`errors/e03`).
- "Nije nađeno" je iterator `e`, ne `-1` ni izuzetak: uvek
  `if (it != v.end())`. Pozicija: `std::distance(v.begin(), it)` (test:
  `find 16 na poziciji 3`).
- `minmax_element` (C++11) nađe oba u jednom prolazu (test; zadatak z1).
- ❌ Algoritam prima opseg, ne kontejner: `std::accumulate(v, 0)` je
  greška (`errors/e05`). C++20 `std::ranges::sort(v)` prima kontejner.
- ⚠️ `accumulate` sabira u tipu **početne vrednosti**: `accumulate(..., 0)`
  nad `double` odseče svaki međuzbir na `int`, bez upozorenja (zadatak z2).
  Za `double` piši `0.0`.
- C++17 `std::reduce` je kao `accumulate`, ali sme da sabira bilo kojim
  redosledom (paralelno). ✅ Za sabiranje celih brojeva isto; za `double`
  rezultat može da se razlikuje u poslednjim ciframa.

---

# 3. Algoritmi koji menjaju opseg (kurs 181)

```cpp
std::copy(b, e, out)        std::copy_if(b, e, out, pred)
std::transform(b, e, out, f)
std::replace(b, e, stara, nova)   std::replace_if(b, e, pred, nova)
std::fill(b, e, x)          std::iota(b, e, pocetna)   // 0, 1, 2, ... (<numeric>)
std::remove(b, e, x)        std::remove_if(b, e, pred)   // ne briše!
std::unique(b, e)           std::reverse(b, e)          std::rotate(b, novi_prvi, e)
```

**Izlazni iterator** (`out`) samo piše: algoritam ne proverava da li ima
mesta.

- ❌ Odredište bez mesta je UB: `ub/u01` (vektor od 3, piše se 5 →
  `heap-buffer-overflow`). ⚠️ `reserve` ne pomaže: menja kapacitet, ne
  veličinu, pa `copy` piše "iza kraja" u sopstvenu memoriju vektora --
  ASan to vidi tek uz `-D_GLIBCXX_SANITIZE_VECTOR` (`ub/u02`), a bez toga
  program tiho ispiše 0 (veličinu).
- ✅ Dve ispravne varijante (Effective STL Item 30):
  - napravi mesto unapred: `std::vector<int> kvadrati(v.size());` (test
    `transform`);
  - pusti da raste: `std::back_inserter(c)` na svako upisivanje zove
    `c.push_back` (test `copy_if`; zadatak z1). ❌ Zato ne radi za
    `std::array` (`errors/e02`). Ima i `std::front_inserter` i
    `std::inserter(c, it)`.
- ⚠️ **`remove` ne briše** (Effective STL Item 32). Algoritam vidi samo
  iteratore, pa ne može da smanji kontejner: elemente koji ostaju prepiše
  ka početku i vrati **novi kraj**; od njega do `end()` su ostaci
  neodređene vrednosti (zadatak z3: `{1,2,3,2,5,2}` posle `remove(2)` ima
  i dalje `size 6`). ✅ **erase-remove** idiom:

  ```cpp
  v.erase(std::remove(v.begin(), v.end(), 0), v.end());   // C++20: std::erase(v, 0)
  ```

- ❌ `remove` na `set`-u se ne kompajlira: elementi su `const` (`errors/e04`).
  Kontejneri koji mogu bolje imaju metodu: `set::erase(x)`,
  `list::remove(x)` (prevezuje čvorove, lekcija 34 sekcija 5).
- `unique` uklanja samo **susedne** duplikate i isto ne briše -- zato
  `sort` pa `erase(unique(...), end())` (test).
- ❌ Algoritam koji menja elemente ne radi preko `const_iterator`-a:
  `std::sort` na `const` vektoru (`errors/e01`).

---

# 4. Sortiranje i sortirani opsezi (kurs 181)

| Algoritam | Šta radi | Složenost |
|---|---|---|
| `sort` | sve, jednaki u bilo kom redosledu | O(n log n) |
| `stable_sort` | sve, jednaki **zadrže** redosled | O(n log n) sa dodatnom memorijom, inače O(n log² n) |
| `partial_sort(b, m, e)` | samo prvih `m - b` najmanjih, sortirano | ≈ n log m |
| `nth_element(b, n, e)` | na mestu `n` element koji bi tu bio posle sortiranja; levo manji, desno veći | O(n) u proseku |

- Test: `stable_sort` po oceni daje `Ana(9) Cane(9) Bora(7) Dara(7)` --
  Ana ostaje ispred Caneta jer je bila ispred. Za sortiranje po drugom
  ključu posle prvog.
- ✅ Effective STL Item 31: ne sortiraj sve ako ne treba. "Top 3" je
  `partial_sort`, medijana je `nth_element` (test; zadatak z1).
- Svi traže random access iteratore (`list` ima svoj `l.sort()`, lekcija 34).

**Nad sortiranim opsegom** (O(log n) poređenja za pretragu, O(n + m) za
kombinovanje):

```cpp
std::binary_search(b, e, x)   // bool
std::lower_bound(b, e, x)     // prvi >= x      upper_bound: prvi > x
std::equal_range(b, e, x)     // par [lower_bound, upper_bound)
std::merge(b1, e1, b2, e2, out)
std::set_intersection / set_union / set_difference(b1, e1, b2, e2, out)
```

- Test: u `{1, 3, 3, 3, 5, 8}` `equal_range(3)` su pozicije 1 do 4;
  `lower_bound(4)` pokazuje na 5 -- mesto gde bi se 4 ubacio da niz ostane
  sortiran.
- ⚠️ Opseg **mora** biti sortiran istim poretkom (istim komparatorom) --
  inače je rezultat besmislen, bez greške i bez upozorenja.

---

# 5. Izmene kontejnera u C++11 i C++17 (kurs 182–186)

| Novo | Primer | Test / gde |
|---|---|---|
| `initializer_list` konstruktor | `std::vector<int> v{1, 2, 3};`, `std::map<...> m{{"a", 1}}` | lekcija 07, sekcija 6 |
| `emplace`, `emplace_back` | `senzori.emplace_back("temp", 1);` -- pravi na mestu | lekcija 24, sekcija 4 |
| `emplace_back` vraća referencu (C++17) | `Senzor& s = v.emplace_back(...)` | test: `poslednji vlaga` |
| `push_back(T&&)`, `insert(T&&)` | `v.push_back(std::move(s));` -- premesti | test: `dug.empty() = true` |
| move pri rastu vektora | samo ako je move `noexcept` | lekcija 23, sekcija 4 |
| `cbegin`, `cend` | `const_iterator` i za ne-const kontejner | test |
| `std::begin`, `std::end` (C++11), `std::size`, `std::data` (C++17) | rade i za C niz | test: `std::size(niz) = 3` |
| `shrink_to_fit` | `capacity` na `size` | test: `capacity 10` (zahtev, ne garancija) |
| `data()` za `vector` | pokazivač na niz, za C API | lekcija 07 |
| novi kontejneri | `array`, `forward_list`, `unordered_*` | lekcije 34, 35 |
| move-only elementi | `std::vector<std::unique_ptr<T>>` | lekcija 33 |

- ⚠️ Stanje `dug` posle `std::move` je "važeće, ali neodređeno"
  (lekcija 22, sekcija 6); `empty() == true` je ono što libstdc++ i libc++ rade,
  ne pravilo standarda.
- ✅ `shrink_to_fit` je **neobavezujući zahtev** (`[vector.capacity]`); ako
  baš mora, stari trik: `std::vector<int>(v).swap(v)`.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 179 | Big O notation | sekcija 1 |
| 180 | Algorithms - I | sekcija 2; `errors/e03`, `e05`; zadatak z2 |
| 181 | Algorithms - II | sekcije 3, 4; `errors/e01`, `e02`, `e04`; `ub/u01`, `u02`; zadatak z3 |
| 182–186 | C++11 Changes to Containers I–V | sekcija 5 |
| 187 | STL Project | zadatak z1 |

---

# Pravilo za praksu

✅ Algoritam umesto ručne petlje (Effective STL Item 43, ES.1): ime kaže
nameru (`count_if`, `any_of`), a složenost je poznata.

✅ Izaberi najjeftiniji algoritam za pitanje: `partial_sort` za "top N",
`nth_element` za medijanu, `binary_search`/`lower_bound` nad sortiranim.

⚠️ Algoritam ne menja veličinu kontejnera: `erase` posle `remove`/`unique`;
odredište mora da ima mesta ili ide preko `back_inserter`.

⚠️ `accumulate(..., 0.0)` za `double` -- tip zbira je tip početne vrednosti.

⚠️ Sortirani algoritmi traže opseg sortiran istim komparatorom.

**Rezime:** algoritmi razdvajaju "šta se radi" od "gde su podaci" --
rade nad bilo kojim opsegom iteratora, uz garantovanu složenost. Zato ne
mogu da dodaju ni brišu elemente, pa se za to kombinuju sa metodama
kontejnera (`erase`, `back_inserter`). C++11 je kontejnerima dodao
pravljenje na mestu, move i listu inicijalizacije.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_analiza_merenja`](exercises/z1_analiza_merenja.cpp) | upotreba | mali STL projekat: analiza loga merenja (sekcije 2, 3, 4; kurs 187) | — |
| [`z2_accumulate_nula`](exercises/z2_accumulate_nula.cpp) | zašto | zašto accumulate "gubi" decimale (sekcija 2) | `-DNAIVNO` |
| [`z3_remove_ne_brise`](exercises/z3_remove_ne_brise.cpp) | zašto | zašto std::remove ne smanji vektor (sekcija 3) | `-DNAIVNO` |

## Zapažanja posle vežbe
