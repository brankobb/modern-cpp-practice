# Lekcija 26 — Funkcijski šabloni (kurs 130–137)

Šablon (template) je recept po kome kompajler pravi funkcije za tipove
sa kojima ga pozoveš. Ova lekcija pokriva funkcijske šablone: kako se
dedukuju argumenti, šta je instancijacija i zašto definicija mora u
header, eksplicitnu specijalizaciju (i zašto je overload obično bolji) i
ne-tipske parametre. Klasni šabloni, variadic šabloni, specijalizacija
klasa i type traits su u lekciji 28.

Već obrađeno, ovde samo upućujemo:

- pravila dedukcije za `T`, `T&`, `T&&` (tri slučaja, EMC Item 1): lekcija
  10, sekcija 2;
- overload sa univerzalnom referencom (EMC Item 26) i `= delete` za
  šablon: lekcija 11, sekcije 4 i 5;
- `constexpr` šabloni, `if constexpr`, `static_assert`: lekcija 12.

**Izvori:** standard, deo `[temp]` (`[temp.deduct]` dedukcija,
`[temp.inst]` implicitna instancijacija, `[temp.explicit]` eksplicitna
instancijacija, `[temp.expl.spec]` eksplicitna specijalizacija,
`[temp.param]` ne-tipski parametri, `[temp.res]` two-phase lookup),
`[over.match.best]` (šablon vs ne-šablon). *Effective Modern C++* **Item 1**.
Core Guidelines **T.1** (šabloni za podizanje nivoa apstrakcije), **T.144**
(ne specijalizuj funkcijske šablone). H. Sutter, "Why Not Specialize
Function Templates?".

**Kako vežbati:**

```
./build.sh 4-sabloni/26-funkcijski-sabloni/main.cpp            # svi ISPRAVNI slučajevi
./check_cases.sh 4-sabloni/26-funkcijski-sabloni               # svi POGREŠNI slučajevi
./check_exercises.sh 4-sabloni/26-funkcijski-sabloni           # vežbe
```

- `errors/` (e01–e07): kod koji se **ne kompajlira**; e07 je greška
  **linkera** (šablon definisan u `.cpp`, fajlovi u `errors/support/`).

---

# 1. Šablon i instancijacija

```cpp
template <typename T>          // "typename" i "class" su ovde isto
T maks(T a, T b) {
    return b < a ? a : b;      // traži samo operator<
}

maks(3, 7);                    // kompajler napravi int maks<int>(int, int)
maks(2.5, 1.5);                // i double maks<double>(double, double)
```

- Šablon **nije funkcija**. Funkcija nastaje **instancijacijom**: kad
  kompajler vidi poziv, zameni `T` pravim tipom i prevede telo.
- Šablon ne postavlja uslove unapred: `maks` radi za svaki tip za koji
  `b < a` ima smisla (`int`, `double`, `std::string`...). Za tip bez
  `operator<` greška je pri instancijaciji (sekcija 3).
- ⚠️ "Ima smisla" je za kompajler samo sintaksa: za `const char*`
  `operator<` postoji, ali poredi **adrese** (zadatak z2).

---

# 2. Dedukcija argumenata

- Kompajler izvede `T` iz argumenata (`[temp.deduct]`), po pravilima za
  parametar po vrednosti, referencu ili forwarding referencu (lekcija 10,
  sekcija 2). Za `T` po vrednosti: `const` i referenca se odbacuju, niz i
  funkcija postaju pokazivač.
- ❌ Svaki argument mora da da **isti** `T`, a dedukcija ne radi
  konverzije: `maks(1, 2.5)` ne prolazi (`errors/e01`: "deduced
  conflicting types for parameter 'T'").
- ✅ Eksplicitan argument: `maks<double>(1, 2.5)`. Kad je `T` zadat, na
  argumentima rade obične konverzije (test).
- Povratni tip se ne koristi za dedukciju. Parametar koji se ne može
  dedukovati mora da se navede (ili da ima podrazumevanu vrednost):

  ```cpp
  template <typename Izlaz = double, typename T>
  Izlaz prosek(const T* niz, std::size_t n);
  prosek(niz, 3);        // Izlaz = double (podrazumevano), T = int (dedukcija)
  prosek<int>(niz, 3);   // Izlaz = int, T i dalje iz argumenta
  ```

---

# 3. Instancijacija i two-phase lookup

- Svaka kombinacija argumenata šablona je **posebna funkcija**, sa svojim
  kodom i svojim `static` promenljivama (test: `brojPoziva<int>` broji
  do 3, a `brojPoziva<double>` je posebno na 1).
- ⚠️ Posledica je veći izvršni fajl ("code bloat"): `kvadrati<5>` i
  `kvadrati<6>` su dve funkcije. Na mikrokontroleru sa malo flash-a to se
  meri.
- **Two-phase lookup** (`[temp.res]`): delovi tela koji ne zavise od `T`
  proveravaju se odmah, a izrazi koji zavise od `T` tek pri
  instancijaciji. Zato je `duzina(const T& t) { return t.size(); }`
  ispravan šablon, a greška se javi tek za `duzina(5)`, i to **unutar
  šablona** (`errors/e02`). Kod velikih šablona to su poruke od stotinu
  redova -- zato se šabloni ograničavaju (`static_assert`, lekcija 28; C++20
  concepts).

**Definicija u header-u.** Da bi instancirao `maks<int>`, kompajler mora
da vidi **telo** šablona na mestu poziva:

- ❌ Deklaracija u `.h`, definicija u `.cpp`: fajl koji poziva `maks<int>`
  očekuje je negde drugde, a `.cpp` sa definicijom je ne instancira jer
  je ne koristi. Linker: "undefined reference to `int maks<int>(int,
  int)'" (`errors/e07`).
- ✅ Ceo šablon u header-u. Šabloni su izuzetak od ODR-a kao `inline`
  (lekcija 08): ista instancijacija u više `.cpp` fajlova nije greška.
- ✅ Za unapred poznat skup tipova: **eksplicitna instancijacija** u `.cpp`
  (`template int maks<int>(int, int);`), a u header-u
  `extern template int maks<int>(int, int);` da je drugi fajlovi ne
  instanciraju ponovo.

---

# 4. Eksplicitni i podrazumevani argumenti

```cpp
maks<double>(1, 2.5);    // T zadat, argumenti se konvertuju
maks<>(3, 4);            // <> -- samo šablon, ne i ne-šablon overload (sekcija 5)
prosek<int>(niz, 3);     // zadat prvi parametar, ostali se dedukuju
```

Parametri šablona se zadaju sleva nadesno; oni koje ne zadaš se
dedukuju ili uzimaju podrazumevanu vrednost.

---

# 5. Overload i eksplicitna specijalizacija (kurs 135)

**Eksplicitna (potpuna) specijalizacija** daje drugo telo za jedan tip:

```cpp
template <typename T> std::string opisi(const T&) { return "nešto"; }
template <> std::string opisi<bool>(const bool& b) { return b ? "da" : "ne"; }
```

**Overload** (obična funkcija ili drugi šablon istog imena) je drugi
način da jedan tip dobije drugačije ponašanje:

```cpp
const char* maks(const char* a, const char* b);   // ne-šablon, poredi strcmp-om
```

Kako se bira (`[over.match.best]`):

1. Učestvuju ne-šabloni i **primarni** šabloni (ne specijalizacije).
2. Bolja konverzija pobeđuje; na izjednačenju ne-šablon pobeđuje šablon,
   a specijalniji šablon (`T*`) opštiji (`T`).
3. Tek kad je izabran primarni šablon, proverava se da li za njega postoji
   eksplicitna specijalizacija za te argumente.

⚠️ Zato specijalizacija funkcijskog šablona iznenađuje: ona pripada šablonu
koji je bio **vidljiv kad je napisana**. Sa `obradi(T)` i `obradi(T*)`,
specijalizacija `template<> void obradi<>(int*)` napisana između njih
specijalizuje `obradi(T)`, a poziv `obradi(&x)` izabere `obradi(T*)` -- i
specijalizacija se ne pozove. Ista specijalizacija napisana posle
`obradi(T*)` se pozove (test, oba kompajlera; zadatak z3).

- ✅ Za funkcije: **overload** umesto specijalizacije (T.144). Ne-šablon je
  predvidljiv i ne zavisi od redosleda.
- ❌ Delimična specijalizacija funkcijskog šablona ne postoji
  (`errors/e05`); isti efekat daje overload `template <typename T> void
  f(T*)`.
- ❌ Specijalizacija posle prve upotrebe tog tipa (`errors/e06`).
- Specijalizacija ima smisla kod **klasnih** šablona (lekcija 28), gde overload
  ne postoji.

---

# 6. Ne-tipski parametri (kurs 136)

```cpp
template <typename T, std::size_t N>
constexpr std::size_t velicina(const T (&)[N]) { return N; }   // N iz tipa niza

template <int Min, int Max>
int ogranici(int x);                                           // ogranici<0, 100>(150) == 100

template <auto V>                                              // C++17: tip iz vrednosti
constexpr auto vrednost() { return V; }                        // vrednost<'x'>(), vrednost<42u>()
```

- Parametar šablona može biti i **vrednost**: celobrojna, `enum`,
  pokazivač, referenca, `nullptr_t`. Od C++20 i `double` i jednostavne
  klase; u C++17 `double` nije dozvoljen (`errors/e04`).
- ❌ Argument mora biti **konstantni izraz**: `puta<n>(3)` sa `n` iz
  `argc` ne prolazi (`errors/e03`).
- ✅ Zašto vrednost u šablonu, a ne parametar funkcije: poznata je pri
  kompajliranju, pa može da bude veličina niza (`std::array<int, N>`), da
  se proveri `static_assert`-om (`static_assert(Min <= Max)`) i da
  kompajler optimizuje kao konstantu. Cena: svaka vrednost je posebna
  instancijacija (sekcija 3).

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 130 | Source Code | — |
| 131 | Introduction to Templates | sekcija 1 |
| 132 | Assignment I | zadatak z1 |
| 133 | Template Argument Deduction & Instantiation | sekcije 2, 3, 4; `errors/e01`, `e02`, `e07` |
| 134 | Assignment II | zadatak z2 |
| 135 | Explicit Specialization | sekcija 5; `errors/e05`, `e06`; zadatak z3 |
| 136 | Non-type Template Arguments | sekcija 6; `errors/e03`, `e04` |
| 137 | Assignment III | zadatak z1, korak 3 |

---

# Pravilo za praksu

✅ Šablon u header-u, ceo. Ili eksplicitna instancijacija za tačno
određene tipove.

✅ Pusti dedukciju da radi; eksplicitan argument kad tipovi argumenata
treba da se konvertuju.

✅ Drugačije ponašanje za jedan tip: overload (ne-šablon), ne eksplicitna
specijalizacija funkcijskog šablona.

✅ Ne-tipski parametar kad je vrednost poznata pri kompajliranju i treba da
bude deo tipa ili provere (`static_assert`).

⚠️ Šablon prihvata svaki tip za koji se telo kompajlira -- i pokazivače,
kod kojih `<` poredi adrese.

⚠️ Greška u šablonu se javlja pri instancijaciji, unutar šablona. Čitaj
poruku odozdo: "required from here" (g++) / "in instantiation of" (clang)
pokazuje poziv u tvom kodu.

**Rezime:** šablon je recept, a instancijacija pravi posebnu funkciju za
svaki skup argumenata -- zato telo mora biti vidljivo na mestu poziva.
Dedukcija izvodi tipove bez konverzija. Za poseban slučaj jednog tipa
koristi overload, jer se specijalizacija funkcije bira tek posle izbora
primarnog šablona.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_genericke_funkcije`](exercises/z1_genericke_funkcije.cpp) | upotreba | funkcijski šabloni, dedukcija i ne-tipski parametri (sekcije 1, 2, 4, 6) | — |
| [`z2_pokazivaci_u_sablonu`](exercises/z2_pokazivaci_u_sablonu.cpp) | zašto | zašto opšti šablon "radi" i kad ne treba (sekcije 1, 5) | `-DNAIVNO` |
| [`z3_specijalizacija_ili_overload`](exercises/z3_specijalizacija_ili_overload.cpp) | zašto | zašto overload umesto eksplicitne specijalizacije funkcijskog šablona (sekcija 5) | `-DNAIVNO` |

## Zapažanja posle vežbe
