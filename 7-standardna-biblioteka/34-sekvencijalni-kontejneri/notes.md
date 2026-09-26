# Lekcija 34 — Sekvencijalni kontejneri (kurs 166–171)

STL (Standard Template Library) ima tri dela: **kontejnere** (čuvaju
elemente), **iteratore** (hodaju kroz njih) i **algoritme** (rade nad
opsegom iteratora, ne znajući za kontejner). Ova lekcija pokriva
sekvencijalne kontejnere -- elementi su redom kojim si ih stavio:
`array`, `vector`, `deque`, `list`, `forward_list`. Asocijativni
(`set`, `map`, `unordered_*`) su u lekciji 35, a algoritmi i složenost u lekciji 36.

Već obrađeno, ovde samo upućujemo:

- `std::array` detaljno: lekcija 05, sekcija 3;
- `std::vector` detaljno (size/capacity, reserve/resize, pristup, izmene,
  šta ASan ne vidi): lekcija 07;
- pokazivač na element vektora posle rasta: lekcija 04, sekcija 12.

**Izvori:** standard, `[containers]`: `[container.reqmts]` (zajednički
zahtevi i invalidacija), `[sequence.reqmts]`, `[array]`, `[vector]`,
`[deque]`, `[list]`, `[forward.list]`; `[iterator.requirements]`
(kategorije iteratora). *Effective STL* (S. Meyers) **Item 1** (pažljivo
biraj kontejner). Core Guidelines **SL.con.1** (`std::array` ili
`std::vector` umesto C niza), **SL.con.2** (`vector` je podrazumevani
izbor).

**Kako vežbati:**

```
./build.sh 7-standardna-biblioteka/34-sekvencijalni-kontejneri/main.cpp          # svi ISPRAVNI slučajevi
./check_cases.sh 7-standardna-biblioteka/34-sekvencijalni-kontejneri             # svi POGREŠNI slučajevi
./check_exercises.sh 7-standardna-biblioteka/34-sekvencijalni-kontejneri         # vežbe
```

- `errors/` (e01–e05): kod koji se **ne kompajlira** -- uglavnom operacije
  koje kontejner namerno nema.
- `ub/` (u01): iterator liste posle `erase`.
- Brojevi alokacija i `sizeof` u `main.cpp` su izmereni za libstdc++ (g++ i
  clang na Linux-u); libc++ (MSYS2 clang64) može da da druge.

---

# 1. Kontejneri, iteratori, algoritmi (kurs 166)

```cpp
template <typename It>
int zbir(It first, It last);            // radi za SVE što ima iteratore

zbir(v.begin(), v.end());               // vector
zbir(l.begin(), l.end());               // list
zbir(std::begin(niz), std::end(niz));   // C niz
```

- Opseg je **poluotvoren** `[first, last)`: `last` pokazuje iza
  poslednjeg (lekcija 04, sekcija 3).
- Iteratori se razlikuju po tome šta umeju -- i to određuje koji algoritmi
  rade sa kojim kontejnerom:

| Kategorija | Ume | Kontejneri |
|---|---|---|
| random access | `++`, `--`, `it + n`, `it2 - it1`, `it[n]` | `array`, `vector`, `deque` (i C niz) |
| bidirectional | `++`, `--` | `list`, `set`, `map` |
| forward | samo `++` | `forward_list`, `unordered_*` |

- ❌ `std::sort` traži random access, pa ne radi na listi (`errors/e01`);
  lista ima svoj `l.sort()`.
- `std::next(it, n)` i `std::distance(a, b)` rade za sve kategorije (za
  listu u O(n)).

---

# 2. `std::array` (kurs 167)

- Fiksna veličina, deo tipa; elementi su **unutar objekta** (na steku ako
  je objekat na steku): `sizeof(std::array<int, 5>) == 20`, 0 alokacija
  (test). Pun STL interfejs (`size`, iteratori, `at`), a bez cene u odnosu
  na C niz (SL.con.1). Detaljno: lekcija 05, sekcija 3.

---

# 3. `std::vector` (kurs 168)

- Jedan neprekidan blok (`data()`), random access, `push_back` amortizovano
  O(1). Podrazumevani izbor (SL.con.2): najbolje koristi keš procesora.
- Raste tako što alocira veći blok i **premesti** elemente: 100 x
  `push_back` bez `reserve` = 8 alokacija, sa `reserve(100)` = 1 (test).
  Posle rasta stari pokazivači i reference vise (test: `data()` se
  promenio; zadatak z3).
- ⚠️ `insert`/`erase` u sredini ili na početku pomeraju sve iza: O(n).
  Zato nema `push_front` (`errors/e04`; zadatak z2).
- Detaljno: lekcija 07.

---

# 4. `std::deque` (kurs 169)

"Double-ended queue": niz blokova fiksne veličine plus tabela pokazivača
na blokove.

- `push_front`, `push_back`, `pop_front`, `pop_back`: O(1). Pristup po
  indeksu: O(1) (dva koraka: blok, pa mesto u bloku).
- Elementi se **ne premeštaju** kad se dodaje na krajeve: reference i
  pokazivači ostaju važeći (test: referenca na prvi element posle 2000
  dodavanja i dalje čita 1, bez ASan prijave). ⚠️ Iteratori se ipak
  poništavaju pri svakom dodavanju.
- 100 x `push_back` = 2 alokacije (test): tabela i jedan blok od 512
  bajtova.
- ❌ Nije jedan blok memorije: nema `data()` (`errors/e05`), pa se ne može
  dati C API-ju kao niz.
- ✅ Za red (FIFO) i red sa prioritetnim ubacivanjem na početak (zadatak z1).

---

# 5. `std::list` i `std::forward_list` (kurs 170)

Povezane liste: svaki element je poseban **čvor** na heap-u (100 x
`push_back` = 100 alokacija, test), sa pokazivačem na sledeći (i prethodni
u `list`).

- `insert`/`erase` na poziciji iteratora: O(1), bez pomeranja drugih
  elemenata.
- ✅ Iteratori i reference ostaju važeći pri svemu osim brisanja baš tog
  elementa (test: iterator na 2 posle `push_front`, `push_back` i `insert`;
  `ub/u01`: iterator obrisanog čvora).
- `splice`: premešta čvorove iz jedne liste u drugu (ili unutar iste) u
  O(1), bez kopiranja i bez alokacije (zadatak z1).
- Svoje verzije algoritama koji bi inače pomerali elemente: `l.sort()`,
  `l.remove(x)`, `l.remove_if(f)`, `l.unique()`, `l.merge(...)`.
- ❌ Nema pristupa po indeksu (`errors/e03`) ni random access iteratora.
- `forward_list` (C++11): jednostruka lista, minimum memorije
  (`sizeof` = 8, test; `list` = 24). Ide samo napred, pa ima `insert_after`,
  `erase_after` i `before_begin()`. ❌ Nema ni `size()` (`errors/e02`).
- ⚠️ Cena: alokacija po elementu, dodatni pokazivači po čvoru, elementi
  razbacani po memoriji (loše za keš). Zbog keša `vector` često ispadne
  brži od liste i tamo gde bi po složenosti lista trebalo da pobedi --
  izmeri pre nego što izabereš listu zbog brzine. Lista se obično bira
  zbog stabilnih iteratora i `splice`-a.

---

# 6. Zajednički interfejs i izbor kontejnera

Svi sekvencijalni kontejneri: `begin`/`end`, `size`/`empty` (osim
`forward_list::size`), konstruktor iz opsega (`std::deque<int> d(v.begin(),
v.end())`), `front`, `insert`, `erase`, `clear`, `swap`.

**Invalidacija** -- šta prestaje da važi posle operacije
(`[container.reqmts]` i opisi kontejnera):

| | dodavanje na kraj | dodavanje na početak | umetanje u sredinu | brisanje |
|---|---|---|---|---|
| `vector` | sve, ako je rast; inače ništa | — | od mesta umetanja nadalje (sve, ako je rast) | od mesta brisanja nadalje |
| `deque` | svi iteratori; reference **ostaju** | svi iteratori; reference **ostaju** | sve | na krajevima samo obrisani; u sredini sve |
| `list`, `forward_list` | ništa | ništa | ništa | samo obrisani |

**Složenost:**

| Operacija | `vector` | `deque` | `list` |
|---|---|---|---|
| pristup po indeksu | O(1) | O(1) | O(n) |
| dodavanje/brisanje na kraju | O(1)* | O(1) | O(1) |
| dodavanje/brisanje na početku | O(n) | O(1) | O(1) |
| u sredini (imaš iterator) | O(n) | O(n) | O(1) |
| alokacija | retko (blok) | po bloku | po elementu |

\* amortizovano

✅ Izbor (Effective STL Item 1, SL.con.2): **`vector`** osim kad imaš
razlog. `array` kad je veličina poznata pri kompajliranju. `deque` za rad
na oba kraja ili kad reference moraju da prežive dodavanje. `list` kad
treba `splice` ili iteratori koji ostaju važeći dok se ostali elementi
dodaju i brišu.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 166 | Introduction | sekcija 1; `errors/e01` |
| 167 | std::array | sekcija 2; lekcija 05 |
| 168 | std::vector | sekcija 3; `errors/e04`; lekcija 07; zadatak z2 |
| 169 | std::deque | sekcija 4; `errors/e05`; zadaci z1, z3 |
| 170 | std::list & std::forward_list | sekcija 5; `errors/e02`, `e03`; `ub/u01`; zadatak z1 |
| 171 | Sequence Containers Demo Code | `main.cpp` |

---

# Pravilo za praksu

✅ `vector` je podrazumevani kontejner; `reserve` kad znaš veličinu.

✅ `deque` za red i za rad na oba kraja; `list` za `splice` i stabilne
iteratore.

✅ Interfejs kontejnera nudi samo ono što je jeftino. Operacija koje nema
(`vector::push_front`, `list::operator[]`, `forward_list::size`) su
namerno izostavljene.

⚠️ Pokazivači, reference i iteratori na elemente: proveri tabelu
invalidacije pre nego što ih sačuvaš.

⚠️ Na listi `erase(it)` poništava baš `it`: `it = l.erase(it)`.

**Rezime:** sekvencijalni kontejneri se razlikuju po rasporedu u memoriji:
jedan blok (`vector`, `array`), blokovi (`deque`) ili čvorovi (`list`). Iz
toga slede i cene operacija i pravila invalidacije. Iteratori ih
povezuju sa algoritmima, a kategorija iteratora određuje koji algoritam
sme da se primeni.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_red_i_zadaci`](exercises/z1_red_i_zadaci.cpp) | upotreba | deque kao red, list sa splice, array kao brojač (sekcije 2, 4, 5) | — |
| [`z2_umetanje_na_pocetak`](exercises/z2_umetanje_na_pocetak.cpp) | zašto | zašto vector nije za umetanje na početak (sekcije 3, 4) | `-DNAIVNO` |
| [`z3_stabilne_adrese`](exercises/z3_stabilne_adrese.cpp) | zašto | zašto izbor kontejnera određuje da li pokazivači "drže" (sekcije 3, 4, 5) | `-DNAIVNO` |

## Zapažanja posle vežbe
