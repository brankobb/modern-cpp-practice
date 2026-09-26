# Lekcija 23 — Pravila generisanja specijalnih funkcija

Kompajler sam piše podrazumevani konstruktor, kopiju, move i destruktor,
ali **samo pod određenim uslovima**. Ako ih ne znaš, klasa se tiho
kopira umesto da se pomera, ili se odjednom ne kompajlira. Ova lekcija je
tabela tih uslova, proverena kodom, i posledice po `std::vector`.

**Izvori:** standard, delovi `[class.default.ctor]`, `[class.copy.ctor]`,
`[class.copy.assign]`, `[class.dtor]`, `[dcl.fct.def.default]` i
`[depr.impldec]`. Uz to *Effective Modern C++* **Item 11** (`= delete`),
**Item 14** (`noexcept`), **Item 17** (generisanje specijalnih funkcija) i
**Item 26** (šablon sa `T&&` i kopija), i C++ Core Guidelines **C.21** i
**C.66**.

**Kako vežbati:**

```
./build.sh 3-zivotni-vek-i-resursi/23-pravila-generisanja/main.cpp
./check_cases.sh 3-zivotni-vek-i-resursi/23-pravila-generisanja
```

- `errors/` (e01–e04): kod koji se **ne kompajlira**.
- `ub/` (u01): `= default` move sa sirovim pokazivačem.
- Srodno: lekcija 14 (`= default`, `= delete`), lekcija 20 (kopija), lekcija 22
  (move), lekcija 25 (rule of 5/0, merenja realokacije).

---

# 1. Tabela (Howard Hinnant, EMC Item 17)

Šta kompajler napiše, zavisno od toga šta si **deklarisao** (i `= default`
i `= delete` se računaju kao deklaracija):

| Ti deklarišeš ↓ / kompajler | podrazumevani ctor | destruktor | copy ctor | copy = | move ctor | move = |
|---|---|---|---|---|---|---|
| ništa | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| bilo koji konstruktor | ❌ | ✅ | ✅ | ✅ | ✅ | ✅ |
| destruktor | ✅ | — | ✅ ⚠️ | ✅ ⚠️ | ❌ **nije deklarisan** | ❌ **nije deklarisan** |
| copy ctor | ❌ | ✅ | — | ✅ ⚠️ | ❌ nije deklarisan | ❌ nije deklarisan |
| copy = | ✅ | ✅ | ✅ ⚠️ | — | ❌ nije deklarisan | ❌ nije deklarisan |
| move ctor | ❌ | ✅ | **obrisan** | **obrisan** | — | ❌ nije deklarisan |
| move = | ✅ | ✅ | **obrisan** | **obrisan** | ❌ nije deklarisan | — |

⚠️ = generiše se, ali je **zastarelo** (`[depr.impldec]`). g++ i clang sa
`-Wextra` upozore kad se upotrebi kopija klase koja ima korisnički copy
konstruktor (`-Wdeprecated-copy`); za klasu sa destruktorom upozorenje
daju tek `-Wdeprecated-copy-dtor` (g++) i `-Wdeprecated` (clang).

Dve vrste "nema":

- **nije deklarisan**: funkcija ne postoji, pa poziv ide na sledećeg
  kandidata. Za move to je **kopija**: `T b = std::move(a)` se
  kompajlira i tiho kopira.
- **obrisan**: funkcija postoji i učestvuje u overload resolution-u, pa
  je poziv greška (`errors/e01`).

Test iz `main.cpp` (član `Probe` javlja šta se stvarno pozvalo):

```
ništa:        move ctor=1 | T b = std::move(a) -> move
destruktor:   move ctor=1 | T b = std::move(a) -> copy    <- trait kaže "može", a zapravo kopira
copy ctor:    move ctor=1 | T b = std::move(a) -> copy
move ctor:    copy ctor=0 copy==0 ...                      <- kopija obrisana
move dodela:  copy ctor=0 ... move ctor=0
```

⚠️ `std::is_move_constructible_v<T>` samo kaže da `T(std::move(x))` **radi**,
ne i da je to pravi move.

---

# 2. Destruktor ukida move

Najčešći slučaj u praksi: klasi se doda destruktor (virtual, logovanje,
debug), i od tada se svaki "move" kopira.

```cpp
struct Logged { std::vector<int> data; ~Logged() {} };
Logged b = std::move(a);   // kopija 1000 elemenata; a.data i dalje ima 1000 (test)
```

✅ Rešenje: kad deklarišeš bilo koju od pet funkcija, deklariši svih pet
(C.21), makar kao `= default`:

```cpp
~LoggedFixed() {}
LoggedFixed(const LoggedFixed&) = default;
LoggedFixed(LoggedFixed&&) = default;
LoggedFixed& operator=(const LoggedFixed&) = default;
LoggedFixed& operator=(LoggedFixed&&) = default;
```

Isto važi za baznu klasu sa `virtual ~Base() = default;`: i on je
deklarisan destruktor, pa baza nema move dok ga ne vratiš.

⚠️ U C++20 klasa sa **bilo kojim** deklarisanim konstruktorom (i
`= default`) nije agregat, pa `LoggedFixed{vec}` ne radi bez pravog
konstruktora (test: g++ i clang u C++20; lekcija 03, `errors/e22`).

---

# 3. `= default` i `noexcept`

| | `noexcept` |
|---|---|
| `T(T&&) = default;` | izvodi se iz članova: `true` ako su move-ovi svih članova `noexcept` |
| `T(T&& o) : s(std::move(o.s)) {}` | `false`, osim ako se napiše `noexcept` |

Test: `HandWritten=false Defaulted=true`. Ručno napisan move bez
`noexcept` se u `std::vector`-u ponaša kao da ne postoji (sledeća
sekcija). Zato: `= default` kad god kompajlerov move radi ispravno, a
ručni uvek sa `noexcept` (C.66).

⚠️ `= default` je **pogrešan** kad klasa ima sirov vlasnički pokazivač:
move `int*` je kopija vrednosti, pa oba objekta obrišu isti niz
(`ub/u01`). Tu ide ručni move (`std::exchange`) ili `unique_ptr` član.

⚠️ `= default` ne garantuje da funkcija postoji: ako član ne može da se
kopira, `T(const T&) = default;` je obrisana (`errors/e03`).

---

# 4. `noexcept` i `std::vector` (EMC Item 14)

Pri realokaciji `std::vector` premešta elemente u novi niz. Ako
premeštanje elementa baci izuzetak na pola posla, stari niz mora da ostane
netaknut (strong garancija `push_back`-a, lekcija 21). To važi samo kad se
**kopira**: move bi već ispraznio deo starih elemenata.

Zato vektor koristi `std::move_if_noexcept`: move **samo** ako je
`noexcept` (ili ako kopija ne postoji).

| Element | `reserve(100)` sa 4 elementa |
|---|---|
| move `noexcept(false)` | kopija=4, move=0 |
| move `noexcept` | kopija=0, move=4 |

Isto pravilo važi za svaku operaciju vektora koja realocira (`push_back`,
`emplace_back`, `reserve`, `resize`).

---

# 5. Šablonski konstruktor nije copy konstruktor

```cpp
struct Box {
    Box(const Box&);
    template <typename T> explicit Box(T&&);   // ne sprečava generisanje kopije...
};
Box b(plainBox);   // ...ali pobedi za ne-const lvalue: T = Box& je tačan match
```

- Šablon **nikad** ne suprimira generisanje specijalnih funkcija (EMC
  Item 17).
- Za `const Box` pobedi copy konstruktor (tačan match, i ne-šablon ima
  prednost). Za **ne-const** `Box` šablon je bolji, jer copy konstruktor
  traži dodavanje `const` (EMC Item 26, lekcija 11).
- Kad šablon radi nešto sa argumentom (npr. pravi `std::string`), to je
  greška pri kompajliranju (`errors/e04`); kad ne radi, kopija tiho radi
  pogrešnu stvar (test).

✅ Ograniči šablon: C++20 `requires (!std::is_same_v<std::decay_t<T>, Box>)`,
C++17 `std::enable_if_t<...>`. Test: kopija tada ide na copy konstruktor.

---

# 6. `= delete` i move-only tipovi

```cpp
Session(const Session&) = delete;             // kopija zabranjena
Session& operator=(const Session&) = delete;
Session(Session&&) = default;                 // move dozvoljen
Session& operator=(Session&&) = default;
```

- ✅ Move-only tip: obriši kopiju, `= default` za move.
- ❌ **Ne briši move** na tipu koji se kopira. Obrisan move učestvuje u
  overload resolution-u i pobedi za rvalue, pa `return w;` i
  `T b = std::move(a);` postanu greška (`errors/e02`).
  Ako tip ne treba da se pomera, samo ne deklariši move: kopija će ga
  zameniti.

---

# Pravilo za praksu

✅ Rule of 0 gde god može. Kad ne može: svih pet, eksplicitno
(`= default` / `= delete` / napisano).

✅ Deklarisao si destruktor (i `virtual`)? Vrati move sa `= default`.

✅ Move konstruktor i dodela su `noexcept`. Proveri
`static_assert(std::is_nothrow_move_constructible_v<T>)` za tipove u
kontejnerima.

⚠️ `= default` move za sirov pokazivač kopira pokazivač.

⚠️ Šablonski konstruktor sa `T&&` otima kopiju; ograniči ga.

⚠️ Nikad `T(T&&) = delete` na kopirajućem tipu.

**Rezime:** kompajler piše specijalne funkcije dok ne deklarišeš neku
sam. Deklarisan destruktor ili kopija tiho ukida move, a deklarisan move
briše kopiju. Najsigurnije je ili ne deklarisati nijednu, ili deklarisati
svih pet. `noexcept` na move-u nije ukras: bez njega `std::vector` kopira.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_predvidi_traitove`](exercises/z1_predvidi_traitove.cpp) | upotreba | pročitaj tabelu generisanja preko type traits (sekcija 1) | — |
| [`z2_destruktor_ukida_move`](exercises/z2_destruktor_ukida_move.cpp) | zašto | zašto "samo dodajem destruktor za log" menja performanse (sekcija 2) | `-DNAIVNO` |
| [`z3_noexcept_vector`](exercises/z3_noexcept_vector.cpp) | zašto | zašto move konstruktor treba noexcept (sekcija 4, EMC Item 14) | `-DNAIVNO` |

## Zapažanja posle vežbe

