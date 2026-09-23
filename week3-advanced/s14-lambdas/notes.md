# Sesija 14 — Lambda izrazi (kurs 151–159)

Lambda je kratak zapis za funkcijski objekat: kompajler od nje napravi
klasu sa `operator()` i članovima za sve što je "zarobljeno" (capture).
Ova sesija prati kurs: od callback-a preko pokazivača na funkciju i
funkcijskog objekta, do lambdi, njihovog unutrašnjeg izgleda, capture
liste (po vrednosti, po referenci, `this`) i generalizovanog capture-a.

Već obrađeno, ovde samo upućujemo:

- pokazivači na funkcije i callback-ovi: lekcija 09, sekcija 7;
- `operator()` i funkcijski objekti: lekcija 12, sekcija 9;
- `operator()` lambde je `const`, `mutable`: lekcija 07, sekcija 11;
- viseće reference u lambdi, init capture (EMC Item 31, 32): week2 s09,
  sekcija 7.

**Izvori:** standard, `[expr.prim.lambda]` (closure tip, `operator()`,
konverzija u pokazivač na funkciju) i `[expr.prim.lambda.capture]`
(capture lista), `[func.wrap.func]` (`std::function`). *Effective Modern
C++* **Item 31** (izbegavaj podrazumevani capture), **Item 32** (init
capture za premeštanje objekata u lambdu), **Item 34** (lambda umesto
`std::bind`). Core Guidelines **F.50** (lambda kad funkcija ne može),
**F.52** (capture po referenci za lokalnu upotrebu, npr. algoritme),
**F.53** (izbegavaj capture po referenci za lambde koje se ne koriste
lokalno), **F.54** (ako zarobljavaš `this`, zarobi sve eksplicitno).

**Kako vežbati:**

```
./build.sh week3-advanced/s14-lambdas/main.cpp                      # svi ISPRAVNI slučajevi
./check_cases.sh week3-advanced/s14-lambdas                         # svi POGREŠNI slučajevi
./check_exercises.sh week3-advanced/s14-lambdas                     # vežbe
```

- `errors/` (e01–e06): kod koji se **ne kompajlira**.
- `ub/` (u01): `[=]` u metodi, a lambda nadživi objekat.

---

# 1. Callback: pokazivač na funkciju (kurs 152)

```cpp
int prebroj(const std::vector<int>& v, bool (*uslov)(int));
prebroj(v, paran);
```

Radi, ali funkcija **nema stanje**: "veći od praga" bi tražio globalnu
promenljivu za prag. Detaljno: lekcija 09, sekcija 7.

---

# 2. Callback: funkcijski objekat (kurs 153)

```cpp
class VeciOd {
public:
    explicit VeciOd(int prag) : prag_(prag) {}
    bool operator()(int x) const { return x > prag_; }
private:
    int prag_;
};
std::count_if(v.begin(), v.end(), VeciOd(4));
```

Objekat nosi **stanje** (prag), a poziva se kao funkcija. Mana: posebna
klasa, daleko od mesta upotrebe.

---

# 3. Lambda izraz (kurs 154)

```
[capture](parametri) -> povratni_tip { telo }
```

```cpp
std::count_if(v.begin(), v.end(), [prag](int x) { return x > prag; });
std::sort(v.begin(), v.end(), [](int a, int b) { return std::abs(a) < std::abs(b); });
```

- Isto što i `VeciOd(prag)`, ali na mestu upotrebe.
- Povratni tip se izvodi iz `return`-a. Kad dva `return`-a daju različite
  tipove (`0` i `double(a) / b`), napiši `-> double`.
- ✅ Najčešća upotreba: argument STL algoritmu (`sort`, `find_if`,
  `count_if`, `remove_if`...) i callback.

---

# 4. Kako lambda radi iznutra (kurs 155)

Od `[prag](int x) { return x > prag; }` kompajler napravi, otprilike:

```cpp
class __lambda_1 {
    int prag;                                           // zarobljena kopija
public:
    bool operator()(int x) const { return x > prag; }   // const po podrazumevanju
};
```

- Svaka lambda ima **svoj jedinstven tip** (closure type), čak i dve
  napisane isto: jedna se ne može dodeliti drugoj (`errors/e03`). Zato se
  lambda čuva u `auto`, prosleđuje šablonu, ili pakuje u `std::function`
  (sekcija 9).
- `sizeof` lambde je veličina zarobljenog stanja (test na x86-64): bez
  capture-a 1, `[a]` (int) 4, `[a, d]` (int, double) 16, `[&a, &d]` 16
  (dve reference, tj. dva pokazivača).
- Lambda **bez capture-a** se konvertuje u običan pokazivač na funkciju
  (tada nema stanja): `int (*fp)() = [] { return 1; };`. Sa capture-om ne
  može (lekcija 09, `errors/e09`).
- `operator()` je `const`: kopije ne mogu da se menjaju bez `mutable`
  (lekcija 07, sekcija 11).
- Od C++17 lambda može biti `constexpr` (i jeste, kad telo to dozvoljava):
  `static_assert(kvadrat(3) == 9)`.

---

# 5. Capture: po vrednosti i po referenci (kurs 156)

```cpp
int x = 1;
auto poVrednosti = [x] { return x; };    // kopija U TRENUTKU pravljenja lambde
auto poReferenci = [&x] { return x; };   // referenca: vidi kasnije promene
x = 2;
poVrednosti();   // 1
poReferenci();   // 2
```

| Capture | Lambda ima | Menja original | Opasnost |
|---|---|---|---|
| `[x]` | kopiju | ne (kopiju samo sa `mutable`) | kopija može biti skupa |
| `[&x]` | referencu | da | visi ako `x` nestane pre poziva lambde |

- ✅ Lambda koja se izvrši odmah (algoritam, `for_each`): `[&]` je u redu
  (F.52).
- ❌ Lambda koja se **čuva za kasnije** (callback, zadatak, nit): ne hvataj
  lokalne promenljive po referenci (F.53; zadatak z3, week2 s09).

---

# 6. Podrazumevani capture: `[=]` i `[&]` (kurs 157)

```cpp
[=]         // sve korišćeno, po vrednosti
[&]         // sve korišćeno, po referenci
[=, &zbir]  // sve po vrednosti, osim zbir
[&, a]      // sve po referenci, osim a
```

- Posle podrazumevanog navodi se samo ono što se zarobljava **drugačije**:
  `[=, x]` je greška (`errors/e02`).
- ❌ Bez capture-a lokalna promenljiva se ne vidi (`errors/e01`).
- Globalne i `static` promenljive se **ne zarobljavaju** -- lambda ih
  koristi direktno i vidi trenutnu vrednost (test: `globalni` promenjen
  posle pravljenja lambde, lambda vrati novu vrednost). `[globalna]` je
  greška (`errors/e06`).
- ⚠️ EMC Item 31: podrazumevani capture sakrije šta se sve zarobljava, i
  koliko dugo to mora da živi. Eksplicitna lista je bolja kad lambda
  izlazi iz funkcije.

---

# 7. Capture i `this` (kurs 158)

U metodi, član `prag_` je u stvari `this->prag_`, pa lambda koja ga
koristi mora da zarobi `this`:

| Capture | Zarobi | Vidi promene člana | Objekat mora da živi |
|---|---|---|---|
| `[this]` | pokazivač | da | ✅ da |
| `[=]` (u metodi) | **pokazivač** (implicitno), ne kopiju članova | da | ✅ da |
| `[*this]` (C++17) | kopiju celog objekta | ne | ne |
| `[prag = prag_]` | kopiju jednog člana | ne | ne |

Test (prag 20, pa promenjen na 10; da li je 15 ispod praga):
`[this]` → false, `[*this]` → true, `[prag = prag_]` → true.

- ⚠️ `[=]` u metodi **nije** kopija objekta: zarobi `this` (zadatak z2). Ako
  lambda nadživi objekat, čita oslobođenu memoriju (`ub/u01`,
  heap-use-after-free). U C++20 je implicitni capture `this` kroz `[=]`
  zastareo, i oba kompajlera upozore (`-Wdeprecated` / clang
  `-Wdeprecated-this-capture`).
- ✅ F.54: `[this]` napiši eksplicitno, ili kopiraj samo ono što treba.

---

# 8. Generalizovani capture (C++14, kurs 159)

```cpp
[tekst = ime + "-01", duzina = ime.size()] { ... }   // nove promenljive u lambdi
[q = std::move(p)] { return *q; }                     // premesti unique_ptr u lambdu
```

- **Init capture**: ime u lambdi = bilo koji izraz. Tako se u lambdu
  **premešta** objekat (EMC Item 32) -- `[p]` za `unique_ptr` ne radi, jer
  bi tražio kopiju (`errors/e04`).
- Posle `[q = std::move(p)]`, `p` je prazan, a lambda je vlasnik (test).
- ⚠️ Lambda koja drži move-only objekat ni sama nema kopiju, pa ne može u
  `std::function` (`errors/e05`: "std::function target must be
  copy-constructible"). Čuvaj je kao `auto`, ili je prosledi šablonu. (C++23
  ima `std::move_only_function`.)

---

# 9. Generičke lambde, `std::function`, IIFE

- **Generička lambda** (C++14): parametar `auto` znači da je `operator()`
  šablon -- `saberi(1, 2)`, `saberi(1.5, 2)`, `saberi(std::string("a"), "b")`.
- **`std::function<R(Args...)>`**: jedan tip za SVE što se može pozvati
  tim potpisom (funkcija, funkcijski objekat, lambda) -- zato može u
  vektor ili kao član klase (zadatak z1). Cena: veći objekat (`sizeof` je
  32 u libstdc++, test), indirektan poziv, a za veliko stanje i alokacija.
  Kad tip može da ostane poznat (`auto`, šablon), koristi to.
- **IIFE** (immediately invoked function expression): lambda pozvana
  odmah, `const auto v = [] { ... }();` -- za `const` promenljivu koja se
  računa u više koraka.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 151 | Source Code | — |
| 152 | Callbacks Revisited - Function Pointers | sekcija 1; lekcija 09 |
| 153 | Callbacks - Function Objects | sekcija 2; lekcija 12 |
| 154 | Lambda Expressions | sekcija 3; zadatak z1 |
| 155 | Lambda Expressions - Internals | sekcija 4; `errors/e03` |
| 156 | Capture List I | sekcija 5; `errors/e01`; zadatak z3 |
| 157 | Capture List II | sekcija 6; `errors/e02`, `e06` |
| 158 | Capture List III | sekcija 7; `ub/u01`; zadatak z2 |
| 159 | Generalized Lambda Capture | sekcija 8; `errors/e04`, `e05` |

---

# Pravilo za praksu

✅ Lambda za kratku logiku na mestu upotrebe (algoritmi, callback-ovi);
imenovana funkcija kad logika ima ime i koristi se na više mesta.

✅ Lambda koja se izvrši odmah: `[&]` je u redu. Lambda koja se čuva:
eksplicitna lista, po vrednosti ili init capture.

✅ U metodi: `[this]` eksplicitno (F.54), ili `[clan = clan_]` / `[*this]`
kad treba snimak.

✅ Move-only objekat u lambdu: `[x = std::move(x)]`.

✅ `auto` ili šablon za lambdu kad god može; `std::function` kad treba
jedan tip za različite callback-ove.

⚠️ `[=]` u metodi zarobljava `this`, ne kopije članova.

⚠️ Svaka lambda je poseban tip.

**Rezime:** lambda je funkcijski objekat koji kompajler napiše umesto
tebe. Capture lista određuje članove te klase: kopije ili reference,
napravljene u trenutku pravljenja lambde. Najveći izvor grešaka je
životni vek: referenca (i `this`) mora da živi koliko i lambda, a ta
pravila jezik ne proverava.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_slusaoci`](exercises/z1_slusaoci.cpp) | upotreba | tri vrste callback-a u std::function, i lambde sa STL algoritmima (sekcije 1, 2, 3, 5, 9) | — |
| [`z2_this_nije_kopija`](exercises/z2_this_nije_kopija.cpp) | zašto | zašto [=] u metodi ne pravi snimak članova (sekcija 7) | `-DNAIVNO` |
| [`z3_referenca_u_petlji`](exercises/z3_referenca_u_petlji.cpp) | zašto | zašto [&] za callback koji se čuva za kasnije visi (sekcije 5, 6; week2 s09, sekcija 7) | `-DNAIVNO` |

## Zapažanja posle vežbe
