# Lekcija 31 — `std::function` i `std::bind` (kurs 161–165)

`std::function` je omotač (wrapper) koji može da drži **bilo šta što se
poziva** zadatim potpisom: funkciju, funkcijski objekat, lambdu, metodu.
`std::bind` pravi novi pozivni objekat od postojeće funkcije, sa nekim
argumentima fiksiranim ili preuređenim. Oba su iz C++11 (`<functional>`).
Za `std::bind` danas skoro uvek postoji bolja zamena: lambda (EMC Item
34), pa ova lekcija pokazuje i zašto.

Već obrađeno, ovde samo upućujemo:

- callback preko pokazivača na funkciju i funkcijskog objekta, lambde i
  capture: lekcija 30 (sekcija 9 ukratko uvodi `std::function`);
- pokazivači na funkcije: lekcija 11, sekcija 7; pokazivač na člana:
  lekcija 04, sekcija 14.

**Izvori:** standard, `[func.wrap.func]` (`std::function`, `bad_function_call`),
`[func.bind]` (`std::bind`, placeholders), `[func.memfn]` (`std::mem_fn`),
`[func.invoke]` (`std::invoke`), `[refwrap]` (`std::ref`). *Effective
Modern C++* **Item 34** (lambda umesto `std::bind`) i **Item 5** (`auto`
umesto `std::function` za lambdu kad tip ne mora da se sakrije). Core
Guidelines **T.49** (izbegavaj "brisanje tipa" gde nije potrebno).

**Kako vežbati:**

```
./build.sh 5-functions-as-values/31-function-and-bind/main.cpp            # svi ISPRAVNI slučajevi
./check_cases.sh 5-functions-as-values/31-function-and-bind               # svi POGREŠNI slučajevi
./check_exercises.sh 5-functions-as-values/31-function-and-bind           # vežbe
```

- `errors/` (e01–e04): kod koji se **ne kompajlira**.
- `ub/` (u01): `std::ref` na lokalnu promenljivu u bind-u koji nadživi funkciju.
- `runtime/` (r01): poziv praznog `std::function`-a, bez `try`.

---

# 1. `std::function`: jedan tip za sve što se poziva (kurs 161)

```cpp
std::function<int(int, int)> op;   // potpis: prima (int, int), vraća int
op = saberi;                       // funkcija
op = Puta{10};                     // funkcijski objekat
op = [](int a, int b) { return a * b; };   // lambda
```

- Svaka lambda i svaki funkcijski objekat su poseban tip (lekcija 30, sekcija 4).
  `std::function` ih "briše" (type erasure) u jedan tip -- zato može da bude
  član klase, vrednost u `std::map` (tabela operacija u `main.cpp`) ili
  element vektora (lanac obrade, zadatak ex1).
- **Prazan** `std::function` (podrazumevano napravljen, ili `= nullptr`):
  `if (f)` je `false`. ❌ Poziv praznog baca `std::bad_function_call` --
  uhvaćen u `main.cpp`, neuhvaćen završi program (`runtime/r01`).
- ✅ Za callback koji može da ne postoji: `if (naKlik) naKlik();`, ili
  podrazumevana prazna lambda `naKlik = [] {};`.

---

# 2. `std::function`: konverzije, metode, cena (kurs 162)

- Argumenti i povratna vrednost se **konvertuju** kao pri običnom pozivu:
  `std::function<double(int)>` prima lambdu koja vraća `int` (test:
  `pola(7) = 3`, jer je deljenje celobrojno već u lambdi). Tip koji se ne
  konvertuje je greška (`errors/e02`).
- **Metoda**: objekat je prvi argument.

  ```cpp
  std::function<int(const Senzor&)> dajId = &Senzor::id;
  std::function<void(Senzor&, int)> postavi = &Senzor::postavi;
  std::invoke(&Senzor::id, s);    // C++17: isti "univerzalni" poziv i za metode
  ```

- Rekurzivna lambda: lambda ne može da imenuje sebe, ali može da zarobi
  `std::function` u koji je smeštena: `[&fakt](int n) { ... fakt(n - 1); }`.
- ⚠️ **Cena** (test, libstdc++ x86-64):
  - `sizeof(std::function<...>)` je 32 bajta, bez obzira šta drži;
  - poziv ide indirektno (kroz pokazivač), pa kompajler ne može da ga
    inline-uje;
  - malo stanje (lambda sa 8 B capture-a) staje u sam objekat, 0
    alokacija; veliko (256 B) ide na **heap**, 1 alokacija. Granica zavisi
    od biblioteke.
- ✅ Kad tip može da ostane poznat, bez `std::function`: `auto f = [...]`
  ili parametar šablona `template <typename F> void primeni(F f)` (EMC
  Item 5, T.49). `std::function` kad baš treba **jedan tip** za različite
  callback-ove (član klase, kontejner, granica API-ja).
- Na mikrokontroleru bez heap-a: pazi na veličinu capture-a, ili koristi
  pokazivač na funkciju + `void* kontekst`, kao C API-ji.

---

# 3. `std::bind`: fiksiranje i preuređivanje argumenata (kurs 163)

```cpp
using namespace std::placeholders;              // _1, _2, ... (errors/e03 bez ovoga)
auto dodaj10 = std::bind(saberi, _1, 10);       // dodaj10(5) == saberi(5, 10)
auto obrnuto = std::bind(oduzmi, _2, _1);       // obrnuto(10, 3) == oduzmi(3, 10)
auto uvek    = std::bind(saberi, 2, 3);         // uvek() == 5
```

- `_1` je "prvi argument poziva", `_2` drugi... Sve što nije placeholder
  je **fiksirana vrednost** (delimična primena, "partial application").
- Isto lambdom: `[](int x) { return saberi(x, 10); }` -- običan poziv
  funkcije, bez posebnih pravila.

---

# 4. `std::bind`: metode, reference, `mem_fn` (kurs 164)

```cpp
std::bind(&Senzor::postavi, &s, _1)    // pokazivač: poziv menja s
std::bind(&Senzor::saPomakom, s, _1)   // KOPIJA s-a, napravljena pri bind-u
std::bind(dodaj, std::ref(brojac), 1)  // referenca na brojac
std::mem_fn(&Senzor::id)               // metoda kao objekat: id(s)
```

- Za metodu, drugi argument bind-a je objekat: pokazivač (`&s`), referenca
  (`std::ref(s)`) ili vrednost (kopija).
- ⚠️ **bind KOPIRA sve argumente** u sebe. Funkcija koja prima `int&`
  dobija referencu na tu unutrašnju kopiju, pa se original ne menja --
  bez greške i bez upozorenja (test: `brojac` posle jednog poziva sa
  kopijom i dva sa `std::ref` je 2; zadatak ex2).
- `std::ref(x)` / `std::cref(x)` kažu bind-u "čuvaj referencu". Tada `x`
  mora da živi koliko i bind objekat (`ub/u01`: referenca na lokalnu
  promenljivu funkcije iz koje je bind vraćen).
- `std::mem_fn(&Senzor::id)` pravi funkcijski objekat koji prima objekat
  kao argument -- zgodno za algoritme.

---

# 5. `std::bind`: zamke, i zašto lambda (kurs 165)

- ⚠️ **Višak argumenata se tiho ignoriše**: `dodaj10(5, 99, 100)` vrati 15
  (test). Lambda sa pogrešnim brojem argumenata je greška.
- ⚠️ **Argumenti se računaju pri bind-u, ne pri pozivu**:
  `std::bind(postaviAlarm, trenutnoVreme() + 1)` izračuna vreme jednom,
  kad se pravi callback (test: napravljen u 8h, pozvan u 12h, alarm na 9h;
  lambda daje 13h; zadatak ex3, EMC Item 34).
- ❌ Preopterećena funkcija ne može direktno u bind: ime nema jedan tip
  (`errors/e01`). Treba cast na tačan pokazivač ili lambda.
- ❌ Premalo argumenata pri pozivu daje grešku duboko u `<functional>`, sa
  tipovima od nekoliko redova (`errors/e04`).
- C++20 ima `std::bind_front(f, a)` (fiksira prve argumente, bez
  placeholder-a i bez ignorisanja viška); ovaj repozitorijum je C++17.

| | `std::bind` | lambda |
|---|---|---|
| kada se računaju fiksirani argumenti | pri bind-u | pri pozivu (ili pri pravljenju, ako su u capture-u) |
| višak argumenata pri pozivu | ignoriše se | greška |
| kopija ili referenca | kopija, osim `std::ref` (ne vidi se na mestu poziva) | vidi se u capture listi |
| preopterećena funkcija | cast | radi |
| poruke o greškama | duge, iz `<functional>` | kratke |

✅ EMC Item 34: lambda umesto `std::bind`. Bind ćeš sretati u starijem kodu
(C++11 i ranije), pa ga treba razumeti -- ali nov kod piši lambdama.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 161 | std::function - I | sekcija 1; `runtime/r01` |
| 162 | std::function - II | sekcija 2; `errors/e02`; zadatak ex1 |
| 163 | std::bind - I | sekcija 3; `errors/e03` |
| 164 | std::bind - II | sekcija 4; `ub/u01`; zadatak ex2 |
| 165 | std::bind - III | sekcija 5; `errors/e01`, `e04`; zadatak ex3 |

---

# Pravilo za praksu

✅ `std::function` kad treba jedan tip za različite callback-ove (član,
kontejner, API). Inače `auto` ili parametar šablona.

✅ Proveri prazan `std::function` pre poziva, ili mu daj podrazumevanu
praznu lambdu.

✅ Lambda umesto `std::bind` (EMC Item 34).

⚠️ `std::bind` kopira argumente (osim `std::ref`), računa ih odmah i
ignoriše višak argumenata pri pozivu.

⚠️ `std::ref`, `&objekat` u bind-u i `[&]` u lambdi: referenca mora da živi
koliko i pozivni objekat.

⚠️ `std::function` ima cenu: 32 B, indirektan poziv, heap za veliko
stanje.

**Rezime:** `std::function` sakrije konkretan tip pozivnog objekta iza
potpisa, po cenu veličine, indirekcije i ponekad alokacije. `std::bind` je
C++11 način da se argumenti fiksiraju i preurede. Lambda radi isto,
čitljivije i bez njegovih zamki (kopiranje, računanje unapred,
ignorisanje viška), pa je ona izbor u novom kodu.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVE`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_lanac_obrade`](exercises/ex1_lanac_obrade.cpp) | usage | std::function kao "bilo šta što se poziva", std::bind za delimičnu primenu i metode (sekcije 1, 3, 4) | — |
| [`ex2_bind_kopira`](exercises/ex2_bind_kopira.cpp) | why | zašto bind "ne menja" promenljivu (sekcija 4) | `-DNAIVE` |
| [`ex3_bind_racuna_odmah`](exercises/ex3_bind_racuna_odmah.cpp) | why | zašto bind računa argumente ODMAH, a lambda pri pozivu (sekcija 5, EMC Item 34) | `-DNAIVE` |

## Zapažanja posle vežbe
