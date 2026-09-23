# Sesija 11 — Izuzeci (kurs 114–119)

Izuzetak prekida normalan tok i prenosi grešku do prvog `catch`-a koji
može da je obradi, uz uništavanje svih lokalnih objekata usput. Ova
sesija pokriva mehaniku: kako se baca i hvata, kojim redom se probaju
`catch` blokovi, kako se izuzetak prosleđuje i "zamotava", šta se dešava
u konstruktoru i destruktoru i šta tačno obećava `noexcept`.

Delovi teme su već obrađeni ranije, pa ovde samo upućujemo:

- stack unwinding i RAII, garancije (basic/strong/nothrow), destruktor ne
  baca: week1 s03;
- izuzetak u konstruktoru i životni vek objekta: week1 s01, sekcija 4;
- `noexcept` na move operacijama i `std::vector`: week2 s06, sekcija 4.

**Izvori:** standard, deo `[except]` (`[except.throw]`, `[except.handle]`,
`[except.ctor]` za unwinding, `[except.spec]` za `noexcept`,
`[except.terminate]`), `[support.exception]` (`std::exception`,
`std::nested_exception`, `std::exception_ptr`). *Effective C++* **Item 8**
(destruktor ne baca) i **Item 29** (garancije); *Effective Modern C++*
**Item 14** (`noexcept`). Core Guidelines **E.2** (izuzetak za grešku koju
funkcija ne može da reši), **E.14** (sopstveni tip izuzetka), **E.15**
(hvataj po referenci), **E.16** (destruktor, dealokacija i `swap` ne
bacaju).

**Kako vežbati:**

```
./build.sh week3-advanced/s11-exceptions/main.cpp                  # svi ISPRAVNI slučajevi
./check_cases.sh week3-advanced/s11-exceptions                     # svi POGREŠNI slučajevi
./check_exercises.sh week3-advanced/s11-exceptions                 # vežbe
```

- `errors/` (e01–e05): kod koji se **ne kompajlira**.
- `runtime/` (r01–r05): kod koji se kompajlira, a program se **prekine**
  (`std::terminate`). To nije UB (ponašanje je definisano), pa ga ne hvata
  sanitizer, nego `check_cases.sh` proverava poruku i izlazni kod, sa g++ i
  clang. Poruka "terminate called..." je od libstdc++; libc++ (MSYS2
  clang64) ima drugačiji tekst.

---

# 1. `throw`, `try`, `catch`

```cpp
double podeli(double a, double b) {
    if (b == 0.0) throw std::invalid_argument("delilac je 0");
    return a / b;
}

try {
    podeli(10, 0);                          // baca: ostatak try bloka se preskače
} catch (const std::invalid_argument& e) {  // ✅ po const&
    std::cout << e.what();
}
```

- `throw izraz` napravi **objekat izuzetka** (kopija ili move izraza, ili
  direktno iz prvalue) i počne da traži `catch`.
- ✅ **Baca se po vrednosti, hvata po `const&`** (E.15). Hvatanje po
  vrednosti pravi kopiju i **seče** izvedeni tip (slicing, zadatak z2).
  g++ `-Wall` upozori (`-Wcatch-value`), clang ne.
- ⚠️ Izuzetak prekida **izraz na mestu gde je bačen**. Test: u
  `std::cout << "10 / 0 = " << podeli(10, 0)` tekst "10 / 0 = " je već
  ispisan (od C++17 `<<` se računa sleva nadesno).
- ✅ Baci tip iz hijerarhije `std::exception`, ne `int` ili `const char*`:
  pozivalac tada može da uhvati sve sa `catch (const std::exception&)` i
  dobije poruku kroz `what()`.

Standardna hijerarhija (`<stdexcept>`):

| Klasa | Znači | Primer |
|---|---|---|
| `std::logic_error` | greška u programu, mogla je da se izbegne | `invalid_argument`, `out_of_range`, `length_error`, `domain_error` |
| `std::runtime_error` | greška iz okoline, ne može da se predvidi | `range_error`, `overflow_error`, `system_error` |
| ostale iz `std::exception` | | `bad_alloc`, `bad_cast`, `bad_variant_access`, `bad_weak_ptr` |

---

# 2. Više `catch` blokova

```cpp
try { ... }
catch (const std::out_of_range& e) { ... }   // izvedena
catch (const std::logic_error& e)  { ... }   // njena baza
catch (const std::exception& e)    { ... }   // sve standardne
catch (...)                        { ... }   // sve ostalo
```

- `catch` blokovi se probaju **redom**, i pobeđuje **prvi** koji odgovara
  (ne "najbolji", kao kod overload-a). Test: `out_of_range` ode u prvi,
  `logic_error` u drugi, `runtime_error` u treći, `throw 42` u četvrti.
- ❌ Baza ispred izvedene klase: izvedeni `catch` je mrtav kod. Oba
  kompajlera upozore (`-Wexceptions`), sa `-Werror` greška (`errors/e01`).
- ❌ `catch (...)` mora biti **poslednji** (`errors/e02`) -- tu je standard
  strožiji: greška, ne upozorenje.
- Između tipa izuzetka i `catch`-a nema običnih konverzija: `throw 42`
  (int) ne hvata ni `catch (long)` ni `catch (double)` (test: prođe do
  spoljnog `catch (int)`). Dozvoljeno je samo izvedena → bazna klasa,
  dodavanje `const` i pokazivačke konverzije.

---

# 3. Sopstvena klasa izuzetka (E.14)

```cpp
class GreskaSenzora : public std::runtime_error {
public:
    GreskaSenzora(int senzorId, const std::string& poruka)
        : std::runtime_error("senzor " + std::to_string(senzorId) + ": " + poruka), id_(senzorId) {}
    int id() const noexcept { return id_; }
private:
    int id_;
};
```

- ✅ Nasledi `std::runtime_error` (ili `logic_error`): on čuva poruku, a
  `what()` je `virtual`, pa radi i kad se hvata kao `std::exception&`.
- ✅ Dodaj podatke koji pomažu pozivaocu da reaguje (id senzora, broj
  reda), ne samo tekst.
- ⚠️ Objekat izuzetka se može kopirati dok putuje (npr. `throw e;`,
  `std::exception_ptr`). Standard traži da kopiranje standardnih
  izuzetaka ne baca, pa poruku drži bazna klasa (`runtime_error`), a ne
  sopstveni `std::string` član, čija kopija alocira i može da baci.

---

# 4. Stack unwinding

```
 a() b() c() ~c() ~b() ~a() | uhvaćeno: duboko
```

Izuzetak bačen u `unutrasnja()` prolazi kroz `srednja()` do `main`-ovog
`catch`-a. Usput se uništavaju **svi** lokalni objekti, obrnutim redom
(test). Zato RAII radi i kad nešto baci: resurs oslobađa destruktor,
a ne kod posle poziva (week1 s03).

- ⚠️ Ako izuzetak **niko ne uhvati**, poziva se `std::terminate`, a da li
  se stek pre toga odmotava nije određeno. Sa g++ i clang se NE odmotava:
  destruktori lokalnih objekata se ne pozovu (test, `runtime/r01`).
- ✅ Zato `main` (i funkcija svake niti) ima `try`/`catch` oko celog posla,
  bar za `std::exception`.

---

# 5. Ugnežđeni `try` i ponovno bacanje

```cpp
try {
    ...
} catch (const std::exception& e) {
    log(e.what());
    throw;          // ✅ ISTI objekat, isti dinamički tip
}
```

- ✅ `throw;` (bez izraza) ponovo baca **trenutni** izuzetak. Spoljni
  `catch (const GreskaSenzora&)` ga i dalje prepozna (test).
- ❌ `throw e;` baca **kopiju** promenljive `e`, statičkog tipa iz
  `catch`-a. Ako je `e` tipa `std::exception&`, a stvarni izuzetak
  `GreskaSenzora`, kopija je samo `std::exception` (zadatak z2).
- ❌ `throw;` kad nema trenutnog izuzetka: `std::terminate`
  (`runtime/r05`).
- `try` blokovi mogu da se ugnežđuju: unutrašnji `catch` obradi šta ume,
  ostalo prođe do spoljašnjeg.

---

# 6. `std::nested_exception`: lanac uzroka

Kad greška prolazi kroz slojeve, svaki sloj želi da doda kontekst ("koji
fajl", "koji blok"), a da ne izgubi originalni uzrok:

```cpp
} catch (...) {
    std::throw_with_nested(std::runtime_error("fajl config.bin"));
}
...
std::rethrow_if_nested(e);   // baci unutrašnji izuzetak, ako postoji
```

Test (`main.cpp`, sekcija 6):

```
fajl config.bin
  blok 3
    CRC ne odgovara
```

- `std::throw_with_nested(x)` baci objekat koji je **i** tipa `x` **i**
  `std::nested_exception`, koji u sebi čuva trenutni izuzetak.
- `std::rethrow_if_nested(e)` baci taj sačuvani izuzetak (ako ga ima), pa
  rekurzivni `catch` ispiše ceo lanac.
- Alternativa bez lanca: **prevedi** izuzetak u tip svog sloja (zadatak z1:
  `std::invalid_argument` iz `stoi` postane `GreskaKonfiguracije`).

---

# 7. Konstruktor i destruktor

```
 trag() Bafer(16) telo | ~Uredjaj ~Bafer ~trag()
 trag() ~trag() | Uredjaj: bafer prevelik | spolja: senzor 0: Uredjaj nije napravljen
```

- **Konstruktor koji baci** ostavlja objekat koji nikad nije postojao:
  njegov destruktor se NE poziva, ali već napravljeni članovi i baze se
  uništavaju, obrnutim redom (test: `~trag()` bez `~Uredjaj`). Detaljno u
  week1 s01, sekcija 4.
- ✅ Konstruktor koji ne može da uspostavi invarijantu **baca**, umesto da
  ostavi "napola napravljen" objekat sa `bool init()` (zadatak z3).
- **function-try-block** hvata i izuzetke iz init liste:

  ```cpp
  Uredjaj(std::size_t n) try : trag_("trag"), bafer_(n) {
      ...
  } catch (const std::length_error& e) {
      throw GreskaSenzora(0, "Uredjaj nije napravljen");   // prevedi
  }
  ```

  Kad handler počne, članovi su **već uništeni** (test). Handler ne može
  da "proguta" izuzetak: `return` je greška (`errors/e03`), a na kraju
  handler-a izuzetak se sam baca dalje. Koristi se za prevođenje ili
  beleženje.
- ❌ **Destruktor ne baca** (EC++ Item 8, E.16). Od C++11 je implicitno
  `noexcept`, pa izuzetak iz njega poziva `std::terminate` -- i `catch`
  oko bloka ne pomaže (`runtime/r02`).
- ❌ Destruktor sa `noexcept(false)` koji baci **dok traje unwinding**
  (drugi izuzetak dok je prvi aktivan): `std::terminate` (`runtime/r04`).

---

# 8. `noexcept` (C++11)

| Oblik | Značenje |
|---|---|
| `void f() noexcept;` | specifikator: obećanje da `f` ne baca |
| `void f() noexcept(uslov);` | uslovni: ne baca ako je `uslov` (konstantni izraz) tačan |
| `noexcept(izraz)` | **operator**: `true` ako izraz ne može da baci; izraz se NE izvršava |
| (ništa) | može da baci; osim destruktora i `= default` funkcija, koji su `noexcept` ako im članovi ne bacaju |

Test (`main.cpp`, sekcija 8):

```cpp
noexcept(bezbedna(1))       // true
noexcept(mozeDaBaci(1))     // false
template <typename T>
void zameni(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                 std::is_nothrow_move_assignable_v<T>);
noexcept(zameni(a, b))      // true za int, false za tip čiji move može da baci
```

- ⚠️ `noexcept` se **ne proverava pri kompajliranju** (samo upozorenje za
  očigledan `throw` u telu). Ako izuzetak ipak izleti:
  `std::terminate`, bez odmotavanja do pozivaoca (`runtime/r03`: baca
  `std::stoi` unutar `noexcept` funkcije, i nema ni upozorenja).
- ✅ Gde je `noexcept` bitan: move konstruktor i move dodela (vector ih
  inače ne koristi, week2 s06), `swap`, destruktori, funkcije koje se
  zovu iz destruktora.
- ❌ Izvedena klasa ne sme da oslabi `noexcept` bazne virtualne funkcije
  (`errors/e04`).
- ❌ Dinamička specifikacija `throw(int)` iz C++98 je uklonjena u C++17
  (`errors/e05`).

---

# 9. `std::exception_ptr`

```cpp
std::exception_ptr p = std::current_exception();   // u catch bloku: sačuvaj izuzetak kao vrednost
...
std::rethrow_exception(p);                          // kasnije, drugde: baci ga ponovo
```

- Služi da se izuzetak prenese **tamo gde ne može da "izleti"**: iz druge
  niti (to radi `std::future`/`std::async`), iz callback-a C biblioteke,
  iz reda poslova.
- `exception_ptr` je kao `shared_ptr` na objekat izuzetka; prazan
  (`nullptr`) znači "nije bilo greške" (test: `posao 0: ok`, `posao 1: x <= 0`).

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 114 | Exception Handling I (Basics) | sekcija 1, 3 |
| 115 | II (Multiple Catch Blocks) | sekcija 2, `errors/e01`, `e02` |
| 116 | III (Stack Unwinding) | sekcija 4, `runtime/r01`; week1 s03 |
| 117 | IV (Nested Exceptions) | sekcija 5 (ugnežđeni `try`, `throw;`), sekcija 6 (`std::nested_exception`) |
| 118 | V (Constructor & Destructor) | sekcija 7, `errors/e03`, `runtime/r02`, `r04`; week1 s01 |
| 119 | VI (`noexcept` u C++11) | sekcija 8, `errors/e04`, `e05`, `runtime/r03`; week2 s06 |

---

# Pravilo za praksu

✅ Baci po vrednosti, hvataj po `const&`, prosleđuj sa `throw;`.

✅ Tipovi izuzetaka iz hijerarhije `std::exception`; sopstveni nasleđuju
`runtime_error`/`logic_error` i nose podatke za pozivaoca.

✅ `catch` od najspecifičnijeg ka najopštijem, `catch (...)` poslednji.

✅ Konstruktor koji ne može da napravi ispravan objekat baca. Destruktor
nikad ne baca.

✅ `noexcept` na move operacijama, `swap`-u i svemu što mora da uspe;
nigde gde bi obećanje moglo da bude lažno.

⚠️ Izuzetak koji izleti iz `main`-a, destruktora ili `noexcept` funkcije
ne stiže ni do jednog `catch`-a: `std::terminate`.

⚠️ Na mikrokontrolerima se izuzeci često isključuju (`-fno-exceptions`)
zbog veličine koda i nepredvidivog vremena. Tada greška ide kroz povratnu
vrednost (`std::optional`, kod greške, `[[nodiscard]]`) -- vidi zadatak z3,
korak 3.

**Rezime:** izuzetak nosi grešku od mesta gde je nastala do mesta gde
može da se reši, a RAII garantuje da se sve usput počisti. Hvata se po
referenci da bi se sačuvao pravi tip, prosleđuje sa `throw;`, a kontekst se
dodaje prevođenjem ili `std::throw_with_nested`. `noexcept` je obećanje:
kompajler ga koristi za optimizacije (i `vector` za move), a prekršeno
obećanje završava program.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_konfiguracija_greske`](exercises/z1_konfiguracija_greske.cpp) | upotreba | sopstvena klasa izuzetka, prevođenje izuzetaka i lanac uzroka (sekcije 2, 3, 6) | — |
| [`z2_hvatanje_po_vrednosti`](exercises/z2_hvatanje_po_vrednosti.cpp) | zašto | zašto catch po const& i "throw;" (sekcije 1, 5) | `-DNAIVNO` |
| [`z3_konstruktor_baca`](exercises/z3_konstruktor_baca.cpp) | zašto | zašto konstruktor baca, umesto init() koji vraća bool (sekcija 7) | `-DNAIVNO` |

## Zapažanja posle vežbe
