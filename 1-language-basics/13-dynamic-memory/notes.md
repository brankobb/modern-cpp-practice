# Lekcija 13 — Dinamička memorija: `malloc`, `new`, `new[]`, 2D nizovi

Lekcija 04 (sekcija 13) pokazuje šta ide naopako sa vlasništvom:
use-after-free, double free, `new[]` + `delete`, curenje. Ova lekcija
objašnjava sam mehanizam: šta tačno rade `malloc` i `new`, zašto se ne
mešaju, kako radi `new[]` i kako se pravi 2D niz na heap-u.

**Izvori:** standard, delovi `[basic.stc.dynamic]` (dinamičko trajanje),
`[expr.new]` i `[expr.delete]`, `[new.delete]` (`operator new`/`operator
delete`) i `[c.malloc]`. Uz to *Effective C++* **Item 16** (isti oblik
`new` i `delete`), *Effective Modern C++* **Item 21** (`make_unique`
umesto `new`), i C++ Core Guidelines **R.10** (izbegavaj `malloc`/`free`) i
**R.11** (izbegavaj eksplicitne `new` i `delete`).

**Kako vežbati:**

```
./build.sh 1-language-basics/13-dynamic-memory/main.cpp       # svi ISPRAVNI slučajevi
./check_cases.sh 1-language-basics/13-dynamic-memory          # svi POGREŠNI slučajevi
```

- `errors/` (e01–e08): kod koji se **ne kompajlira**.
- `ub/` (u01–u10): kod koji se kompajlira, a ASan/LSan ga hvata pri
  pokretanju.

---

# 1. Zašto dinamička memorija

| Trajanje (`[basic.stc]`) | Primer | Kad nastaje / nestaje |
|---|---|---|
| automatic | lokalna promenljiva | ulaz u blok / izlaz iz bloka |
| static | globalna, `static` | pre `main` (ili pri prvom prolazu) / posle `main` |
| **dynamic** | `new`, `malloc` | kad ti kažeš / kad ti kažeš (`delete`, `free`) |

Dinamička memorija treba kad:

- objekat mora da **nadživi** funkciju koja ga pravi;
- se **veličina** zna tek u toku izvršavanja (`new int[n]`; `int arr[n]`
  je VLA i nije C++, lekcija 05);
- je objekat **prevelik** za stek.

Cena: oslobađanje je tvoja odgovornost. Zato u modernom kodu memoriju
drži objekat koji je sam oslobađa (sekcija 7).

---

# 2. `malloc` / `free` (C)

```cpp
int* p = static_cast<int*>(std::malloc(3 * sizeof(int)));  // bajtovi, void*, bez inicijalizacije
if (p == nullptr) { /* nema memorije */ }
std::free(p);
```

| | `malloc` | `calloc` | `realloc` |
|---|---|---|---|
| Veličina | u bajtovima | broj × veličina | nova veličina |
| Sadržaj | neinicijalizovan | nule | stari sadržaj se čuva |
| Neuspeh | `nullptr` | `nullptr` | `nullptr`, a **stari blok ostaje** |

- ❌ `int* p = std::malloc(...)` bez cast-a se ne kompajlira. U C++ `void*`
  ne prelazi sam u `int*` (`errors/e01`).
- ⚠️ `realloc` može da premesti blok. Stari pokazivač je posle toga
  nevažeći (`ub/u09`), a `p = realloc(p, n)` gubi blok ako `realloc` ne
  uspe. Zato rezultat ide u novu promenljivu.
- ✅ `free(nullptr)` je dozvoljen i ne radi ništa.
- ❌ **`malloc` ne poziva konstruktor.** Za `int` i proste strukture od
  njih to je u redu (C++20, P0593, to i formalno dozvoljava za tzv.
  implicit-lifetime tipove). Za tip sa `std::string` u memoriji nema
  ispravnog objekta, pa prva upotreba pukne (`ub/u06`).
  ✅ Ako baš moraš: **placement new** `new (raw) Named{...}` pozove
  konstruktor na postojećoj memoriji. Destruktor se onda poziva ručno
  (`obj->~Named()`), pa tek onda `free` (`main.cpp`, sekcija 2). Detaljnije
  u lekciji 33.

**Pravilo (R.10):** u C++ kodu `malloc` ne treba. Sreće se samo na granici
sa C bibliotekama, i tada se oslobađa sa `free`.

---

# 3. `new` / `delete`: alokacija + konstruktor

```cpp
Tracer* t = new Tracer;  // 1) operator new(sizeof(Tracer))  2) konstruktor
delete t;                // 1) destruktor                     2) operator delete
```

To je cela razlika u odnosu na `malloc`/`free`: `new` pravi **objekat**, a
`malloc` samo rezerviše **bajtove**. `main.cpp` to pokazuje klasom koja se
javlja iz konstruktora i destruktora.

Oblici inicijalizacije su isti kao kod običnih promenljivih (lekcija 03):

| Izraz | Rezultat |
|---|---|
| `new int` | ⚠️ default-init: **neodređena** vrednost, čitanje je UB |
| `new int()` | 0 (value-init) |
| `new int(5)`, `new int{5}` | 5 |
| `new Point{1, 2}` | agregat |
| `new const int` | ❌ const mora da dobije vrednost (`errors/e02`) |

ASan i UBSan **ne hvataju** čitanje neinicijalizovane vrednosti: `new int`
pa `*p` prolazi bez prijave i ispisuje smeće. To hvata MemorySanitizer, koji
postoji samo za clang.

**Pravila za `delete`:**

- `delete nullptr` je dozvoljen i ne radi ništa. `if (p) delete p;` je
  suvišan.
- `delete p` ne postavlja `p` na `nullptr`.
- `delete` sme samo na pokazivač koji je vratio `new`:

| Pogrešno | Šta se desi |
|---|---|
| `delete x;` (`x` nije pokazivač) | ❌ ne kompajlira se (`errors/e06`) |
| `delete &x;` (lokalna promenljiva) | UB, `bad-free` (`ub/u04`) |
| `delete[] (a + 1);` (sredina niza) | UB, `bad-free` (`ub/u10`) |
| `delete` na `void*` | UB; g++ samo upozori, clang odbije da kompajlira |

---

# 4. Kad alokacija ne uspe

| | Neuspeh |
|---|---|
| `malloc` | vraća `nullptr` |
| `new` | **baca `std::bad_alloc`**, nikad ne vraća `nullptr` |
| `new (std::nothrow) T` | vraća `nullptr` |

Zato je `if (p == nullptr)` posle običnog `new` besmislen: do te linije se
ne stigne.

Za `new T[n]` kod kog `n * sizeof(T)` ne staje u `size_t` (ili je `n`
negativan), standard traži izuzetak tipa `std::bad_array_new_length`
(izveden iz `bad_alloc`), a za `nothrow` verziju `nullptr`. Ni jedan
kompajler ovo ne radi potpuno po standardu:

| `n * sizeof(T)` prelije `size_t`, ili je `n < 0` | g++ 13 | clang 18 |
|---|---|---|
| `new int[n]` | `bad_array_new_length` | `bad_alloc` |
| `new (std::nothrow) int[n]` | baca `bad_array_new_length` | `nullptr` |

✅ Praktično: hvataj `const std::bad_alloc&`, to radi na oba.

⚠️ Pod ASan-om zahtev za stvarno preveliku memoriju ne baca izuzetak nego
prekida program ASan-ovom prijavom. Zato `main.cpp` pokazuje slučaj sa
prelivanjem, koji do alokatora ni ne stigne.

---

# 5. `new[]` / `delete[]`

```cpp
Tracer* ts = new Tracer[3];   // konstruktor za svaki element: 1, 2, 3
delete[] ts;                  // destruktor obrnutim redom: 3, 2, 1
```

| Izraz | Rezultat |
|---|---|
| `new int[n]` | `n` neodređenih vrednosti |
| `new int[n]()` | `n` nula |
| `new int[5]{1, 2}` | `1 2 0 0 0` |
| `new int[]{7, 8, 9}` | veličina 3 iz liste |
| `new int[3]{1, 2, 3, 4}` | ❌ previše vrednosti (`errors/e03`) |
| `new Widget[3]` bez podrazumevanog konstruktora | ❌ (`errors/e04`) |
| `new int[0]` | ✅ nije `nullptr`, ali nema elemenata |

**Zašto `delete[]` a ne `delete`?** Za tip sa destruktorom `new[]` sačuva
**broj elemenata** ispred niza, da bi `delete[]` znao koliko destruktora da
pozove. Test (g++ i clang): `new WithDtor[3]` (3 × 4 bajta) traži od
alokatora 20 bajtova, a `new Trivial[3]` samo 12. `delete` ne zna za taj
broj, a `delete[]` na običnom `new` čita nepostojeći broj. Zato mešanje
oblika nije "radi za int", nego UB (EC++ Item 16; `ub/u03`, lekcija 04
`ub/u09`).

**Izuzetak u konstruktoru elementa:** ako treći konstruktor baci izuzetak,
jezik sam uništi prva dva (obrnutim redom) i oslobodi memoriju. Tu nema
curenja (`main.cpp`, sekcija 5).

**Mešanje alokatora** je uvek UB, i ASan ga prijavljuje kao
`alloc-dealloc-mismatch`:

| Alocirano sa | Oslobađa se sa | Pogrešno |
|---|---|---|
| `malloc`/`calloc`/`realloc` | `free` | `delete` (`ub/u01`) |
| `new` | `delete` | `free` (`ub/u02`), `delete[]` (`ub/u03`) |
| `new[]` | `delete[]` | `delete` (lekcija 04, `ub/u09`) |

g++ sa `-Wall` većinu ovih slučajeva vidi već pri kompajliranju
(`-Wmismatched-new-delete`, `-Wfree-nonheap-object`, `-Wuse-after-free`).
Upozorenje ne treba ignorisati.

---

# 6. 2D nizovi

| Način | Alokacija | Memorija | Oslobađanje | Napomena |
|---|---|---|---|---|
| (a) `new int*[rows]` + `new int[cols]` po redu | `rows + 1` | redovi razbacani | petlja + `delete[]` | ⚠️ lako curi (`ub/u07`); redovi mogu biti različite dužine |
| (b) `new int[rows * cols]`, `m[r * cols + c]` | 1 | jedan blok | jedan `delete[]` | ⚠️ formula se lako pogreši (`ub/u08`) |
| (c) `new int[rows][kCols]` | 1 | jedan blok | jedan `delete[]` | samo kad je broj kolona **konstanta** |
| (d) `std::vector<int>(rows * cols)` ili `vector<vector<int>>` | 1 / `rows + 1` | kao (b) / kao (a) | samo | ✅ podrazumevani izbor |

Zamke:

- ❌ `int** m = new int[3][4];` se ne kompajlira. `new int[3][4]` vraća
  `int (*)[4]` (pokazivač na red od 4 int-a), a ne pokazivač na pokazivač
  (`errors/e07`). Isto kao kod običnih nizova u lekciji 05.
- ❌ `new int[rows][cols]` sa `cols` iz runtime-a se ne kompajlira. Samo
  **prva** dimenzija sme da bude runtime vrednost, ostale su deo tipa
  (`errors/e05`).
- ⚠️ Kod (a): ako `new` za treći red baci izuzetak, prva dva reda i niz
  pokazivača cure, osim ako ih ručno ne počistiš u `catch`. Kod (b), (c) i
  (d) toga nema, jer je alokacija jedna ili je drži `vector`.
- ⚠️ Kod (b): zamenjeni red i kolona (`c * cols + r`) izlaze iz bloka kad
  matrica nije kvadratna (`ub/u08`). Kod kvadratne ne izlaze, nego tiho
  transponuju podatke, i to nijedan alat ne hvata. Formulu piši na **jednom**
  mestu, npr. u funkciji `int& at(int r, int c)`.

**Praksa:** jedan blok sa `std::vector<int>(rows * cols)` i funkcijom
`at(r, c)`. Jedna alokacija, podaci jedni do drugih (brže zbog keša) i bez
ručnog `delete`. `vector<vector<int>>` je zgodan kad su redovi različite
dužine.

---

# 7. Moderni C++: bez ručnog `delete` (R.11)

```cpp
auto one  = std::make_unique<int>(42);     // umesto new int(42)
auto many = std::make_unique<int[]>(n);    // umesto new int[n]() -- nule
std::vector<int> v(n);                     // najčešće najbolje rešenje
```

- `std::unique_ptr` / `std::vector` oslobađaju memoriju sami, i kad
  izuzetak prekine funkciju.
- `make_unique` umesto `new` (EMC Item 21): kraće i bez ponavljanja tipa.
  Pre C++17 je štitio i od curenja u `f(std::unique_ptr<T>(new T), g())`
  kad `g()` baci izuzetak. C++17 je taj redosled izvršavanja popravio.
- ❌ `std::unique_ptr<int> p = new int(5);` se ne kompajlira, jer je
  konstruktor iz sirovog pokazivača `explicit` (`errors/e08`). Preuzimanje
  vlasništva ne sme da bude tiho.
- C++20: `std::make_unique_for_overwrite<int[]>(n)` preskače nule kad ćeš
  ionako sve prepisati.

Kako se piše klasa koja sama drži `new[]` memoriju (destruktor, kopiranje,
rule of 3/5) je tema dela 3 (lekcije 19–25).

---

# Pravilo za praksu

✅ `std::vector` i `std::make_unique` umesto `new[]` i `new`. Ručni
`delete` u aplikativnom kodu je znak da nešto nedostaje.

✅ Ako već pišeš `new`, oblik mora da se poklapa: `new`/`delete`,
`new[]`/`delete[]`, `malloc`/`free`.

✅ `new int()` i `new int[n]()` kad hoćeš nule. `new int` ostavlja smeće.

✅ 2D: jedan blok (`vector<int>(rows * cols)`) i jedna funkcija za indeks.

⚠️ `new` ne vraća `nullptr`, nego baca `std::bad_alloc`.

⚠️ `malloc` ne pravi objekte. Za tipove sa konstruktorom samo `new` (ili
placement new).

⚠️ Posle `realloc`, `delete` ili rasta `vector`-a stari pokazivači vise.

**Rezime:** `new` = alokacija + konstruktor, `delete` = destruktor +
oslobađanje, a `malloc`/`free` rade samo pola posla. Tri para alokatora se
ne mešaju, jer svaki drugačije vodi računa o memoriji (npr. broj elemenata
ispred `new[]` niza). U modernom kodu te parove ne pišeš sam: memoriju drži
`vector` ili `unique_ptr`, i oslobađa je kad izađe iz scope-a.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_new_i_blok`](exercises/ex1_new_i_blok.cpp) | usage | new[]/delete[], unique_ptr<T[]> i 2D u jednom bloku (sekcije 5, 6, 7) | — |
| [`ex2_dinamicki_niz`](exercises/ex2_dinamicki_niz.cpp) | usage | sopstveni rastući niz: šta std::vector radi za tebe (sekcija 5) | — |
| [`ex3_curenje_pri_izuzetku`](exercises/ex3_curenje_pri_izuzetku.cpp) | why | zašto ručni new curi čim nešto baci izuzetak (sekcija 7, R.11) | `-DNAIVNO` |

## Zapažanja posle vežbe

