# Sesija 13 — Klasni šabloni, variadic, specijalizacija, type traits (kurs 138–150)

Nastavak s12. Tamo su bili funkcijski šabloni; ovde su klasni šabloni i
alati oko njih: variadic šabloni i fold izrazi, potpuna i delimična
specijalizacija klase, alias šabloni, type traits (pitanja o tipovima
pri kompajliranju) i `static_assert` kao jasna poruka kad šablon dobije
pogrešan tip.

Već obrađeno, ovde samo upućujemo:

- forwarding reference, reference collapsing, `std::forward` (kurs
  138–139): week2 s09, sekcije 1–3;
- `if constexpr` i `static_assert` u `constexpr` kodu: lekcija 16;
- `using` vs `typedef` za obične aliase (EMC Item 9): lekcija 05, sekcija 7.

**Izvori:** standard, deo `[temp]`: `[temp.variadic]` (paketi), `[expr.prim.fold]`
(fold izrazi), `[temp.class]` i `[temp.inst]` (klasni šabloni i lenja
instancijacija metoda), `[temp.deduct.guide]` (CTAD), `[temp.expl.spec]` i
`[temp.spec.partial]` (specijalizacija), `[temp.alias]`, `[temp.res]`
(`typename`), `[meta]` (`<type_traits>`). *Effective Modern C++* **Item 9**
(alias šabloni umesto typedef-a) i **Item 27**. Core Guidelines **T.43**
(`using` umesto `typedef`), **T.100** (variadic šabloni za promenljiv broj
argumenata različitih tipova), **T.150** (proveri svojstva tipa
`static_assert`-om).

**Kako vežbati:**

```
./build.sh week3-advanced/s13-class-templates-traits/main.cpp             # svi ISPRAVNI slučajevi
./check_cases.sh week3-advanced/s13-class-templates-traits                # svi POGREŠNI slučajevi
./check_exercises.sh week3-advanced/s13-class-templates-traits            # vežbe
```

- `errors/` (e01–e07): kod koji se **ne kompajlira**.

---

# 1. Savršeno prosleđivanje + variadic (kurs 138–139)

```cpp
template <typename... Args>
T& napravi(Args&&... args) {
    elementi_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    return *elementi_.back();
}
```

Ovako rade `emplace_back`, `std::make_unique` i `std::thread`: primaju
bilo koji broj argumenata bilo kog tipa i predaju ih konstruktoru tačno
onakve kakve su stigle. Test: `napravi(ime, 1)` kopira `ime` (lvalue), a
`napravi(std::string("pritisak"), 2)` pomera (rvalue). Detaljno o
`Args&&` i `std::forward`: week2 s09.

---

# 2. Variadic šabloni (kurs 140–141)

```cpp
template <typename... Args>        // Args je PAKET tipova
void f(const Args&... args);       // args je PAKET vrednosti
sizeof...(args)                    // broj elemenata paketa, pri kompajliranju
```

Paket se koristi samo **proširen** sa `...` (`errors/e06`):

| Oblik | Proširi se u |
|---|---|
| `g(args...)` | `g(a1, a2, a3)` |
| `g(kvadrat(args)...)` | `g(kvadrat(a1), kvadrat(a2), kvadrat(a3))` |
| `std::forward<Args>(args)...` | svaki sa svojim tipom |

**Dva načina da se obradi svaki element:**

1. **Rekurzija** (C++11): "prvi" + "ostali", pa poziv za ostale. Mora da
   postoji osnovni slučaj, ne-šablon `ispisiRek()` za prazan paket --
   inače poslednja instancijacija zove funkciju koja ne postoji (zadatak
   z3). Obični `if (sizeof...(ostali) > 0)` ne pomaže: poziv unutar
   njega se i dalje kompajlira (test); pomaže `if constexpr`.
2. **Fold izrazi** (C++17): bez rekurzije.

| Fold | Oblik | Primer |
|---|---|---|
| unarni desni | `(args op ...)` | `(args + ...)` |
| unarni levi | `(... op args)` | `(... && (args > 0))` |
| binarni | `(init op ... op args)` | `(0 + ... + args)` |
| preko zareza | `(izraz(args), ...)` | `((std::cout << args), ...)` |

- ❌ Unarni fold **praznog** paketa nema vrednost za `+`, `*`... (`errors/e05`).
  Samo `&&` (true), `||` (false) i zarez (`void()`) imaju vrednost za
  prazan paket. Rešenje: binarni fold sa početnom vrednošću, `(0 + ... +
  args)` (test: `zbir() = 0`, `sviPozitivni() = true`).
- Fold preko zareza izvršava izraze **redom, sleva nadesno** -- to je
  zamena za petlju po paketu (zadatak z1).

---

# 3. Klasni šabloni (kurs 143)

```cpp
template <typename T, std::size_t N>
class Stek {
public:
    void push(const T& v);
    T pop();
    T najveci() const;
private:
    std::array<T, N> podaci_{};
    std::size_t vel_ = 0;
};

template <typename T, std::size_t N>   // definicija van klase
T Stek<T, N>::pop() { ... }
```

- `Stek<int, 4>` i `Stek<int, 8>` su **različiti tipovi** (sekcija 3 u s12:
  svaka instancijacija je posebna).
- Definicija metode van klase ponavlja listu parametara i piše
  `Stek<T, N>::`. Sve ide u header (s12, sekcija 3).
- **Lenja instancijacija** (`[temp.inst]`): metoda klasnog šablona se
  prevodi tek kad se **pozove**. `Stek<Tacka, 2>` radi iako `Tacka` nema
  `operator<` -- dok se ne pozove `najveci()` (test; `errors/e04`). Zato
  jedan šablon može da ima metode koje rade samo za neke tipove.
- **Zavisni tipovi traže `typename`**: `typename C::value_type x;`. Bez
  toga kompajler pretpostavi da `C::value_type` nije tip (`errors/e07`).

**CTAD** (class template argument deduction, C++17): argumenti šablona se
izvode iz argumenata konstruktora.

```cpp
std::pair p{1, 2.5};      // std::pair<int, double>
std::array a{1, 2, 3};    // std::array<int, 3>
Omotac o{"tekst"};        // Omotac<std::string> -- zbog deduction guide-a:
Omotac(const char*) -> Omotac<std::string>;
```

- Bez guide-a bi `Omotac{"tekst"}` bio `Omotac<const char*>`. Deduction
  guide kaže kompajleru "za ove argumente izvedi ovaj tip".
- ❌ Bez argumenata nema dedukcije: `std::vector v;` (`errors/e01`).

---

# 4. Eksplicitna specijalizacija klase (kurs 144–145)

```cpp
template <typename T> struct Opis { static std::string ime() { return "nešto"; } };
template <> struct Opis<bool> { static std::string ime() { return "bool"; } };
```

- Specijalizacija je **potpuno druga klasa**: ne nasleđuje ništa od
  primarnog šablona, može da ima druge članove.
- Kod klasa (za razliku od funkcija, s12 sekcija 5) specijalizacija je
  pravi alat: overload za klase ne postoji.
- Može da se specijalizuje i **samo jedna metoda**, a ostatak klase ostane
  opšti:

  ```cpp
  template <> std::string Kutija<std::string>::prikazi() const { return '"' + v + '"'; }
  ```

  Test: `Kutija<std::string>` dobija novi `prikazi()`, a `prazna()` iz
  opšteg šablona.

---

# 5. Delimična specijalizacija (kurs 147)

Specijalizacija za **familiju** tipova, sa sopstvenim parametrima:

```cpp
template <typename T> struct Opis<T*>            { ... "pokazivač na " + Opis<T>::ime() };
template <typename T> struct Opis<std::vector<T>> { ... };
template <typename T, std::size_t N> struct Opis<T[N]> { ... };
```

Test: `Opis<std::vector<bool*>>` → "vektor od pokazivač na bool",
`Opis<double**>` → "pokazivač na pokazivač na nešto".

- Bira se **najspecijalnija** delimična specijalizacija koja odgovara.
- ❌ Ako odgovaraju dve, a nijedna nije specijalnija, greška
  (`errors/e02`: `Par<A*, B>` i `Par<A, B*>` za `Par<int*, int*>`).
- Funkcijski šabloni nemaju delimičnu specijalizaciju (s12, `errors/e05`).
- Ovo je osnova za **type traits** (sekcija 7).

---

# 6. Alias šabloni (kurs 148)

```cpp
template <std::size_t N>
using Bajtovi = std::array<std::uint8_t, N>;
template <typename T>
using PoImenu = std::map<std::string, T>;
using Paket = Bajtovi<8>;
```

- `typedef` ne može da ima parametre (lekcija 05, `errors/e13`), `using`
  može (EMC Item 9, T.43).
- Alias **nije nov tip**: `Paket` i `std::array<std::uint8_t, 8>` su isti
  tip (test, `static_assert`). Za zaista nov tip treba `struct` ili `enum
  class`.

---

# 7. Type traits (kurs 149)

`<type_traits>` postavlja pitanja o tipovima i pravi nove tipove, sve pri
kompajliranju:

| Vrsta | Primeri | Rezultat |
|---|---|---|
| pitanja | `is_integral_v<T>`, `is_pointer_v<T>`, `is_same_v<A, B>`, `is_nothrow_move_constructible_v<T>` | `bool` |
| transformacije | `remove_reference_t<T>`, `remove_cv_t<T>`, `decay_t<T>`, `add_const_t<T>` | tip |
| izbor | `conditional_t<uslov, A, B>`, `common_type_t<A, B>` | tip |

`_v` je skraćenica za `::value`, `_t` za `::type` (C++14/17).

**Sopstveni trait**: primarni šablon kaže "ne", specijalizacija "da".

```cpp
template <typename T> struct jeVektor : std::false_type {};
template <typename T> struct jeVektor<std::vector<T>> : std::true_type {};
template <typename T> inline constexpr bool jeVektor_v = jeVektor<T>::value;
```

- Trait se koristi sa `if constexpr` (grana za pogrešan tip se ne
  instancira) i sa `static_assert`.
- ⚠️ Specijalizacija se poklapa samo sa **tačno** tim tipom:
  `jeString<const std::string&>` nije `jeString<std::string>`. U šablonu sa
  forwarding referencom `T` je često referenca, pa pre pitanja skini
  referencu i `const`: `std::remove_cv_t<std::remove_reference_t<T>>` (C++20:
  `std::remove_cvref_t<T>`) ili `std::decay_t<T>` (zadatak z2).

---

# 8. `static_assert` (kurs 150)

```cpp
template <typename T>
class Merenje {
    static_assert(std::is_arithmetic_v<T>, "Merenje<T>: T mora biti brojčani tip");
    ...
};
```

- ✅ Pretpostavka šablona postaje proverena: pogrešan tip daje **tvoju**
  poruku na početku izveštaja (`errors/e03`), umesto greške duboko u
  telu (s12, `errors/e02`) -- ili, gore, šablona koji se kompajlira i radi
  besmisleno.
- Od C++17 poruka nije obavezna: `static_assert(sizeof(Merenje<std::uint16_t>) == 2);`.
- Isto na ne-tipskim parametrima: `static_assert(N > 0)` (zadatak z1).
- C++20 **concepts** (`template <std::integral T>`, `requires`) rade isto,
  ali kao deo potpisa: pogrešan tip se odbije pri izboru overload-a, a ne
  tek pri instancijaciji. Ovaj repozitorijum je C++17, pa ih ovde samo
  pominjemo.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 138–139 | Perfect Forwarding I–II | sekcija 1; week2 s09 |
| 140–141 | Variadic Templates I–II | sekcija 2; `errors/e05`, `e06`; zadatak z3 |
| 142 | Assignment IV | zadatak z1, korak 2 |
| 143 | Class Templates | sekcija 3; `errors/e01`, `e04`, `e07` |
| 144–145 | Class Template Explicit Specialization I–II | sekcija 4 |
| 146 | Assignment V | zadatak z1 |
| 147 | Class Template Partial Specialization | sekcija 5; `errors/e02` |
| 148 | Typedef, Type Alias & Alias Templates | sekcija 6; zadatak z1, korak 3 |
| 149 | Type Traits | sekcija 7; zadatak z2 |
| 150 | static_assert | sekcija 8; `errors/e03` |

---

# Pravilo za praksu

✅ Promenljiv broj argumenata različitih tipova: variadic šablon (T.100), a
obrada paketa fold izrazom; rekurzija samo sa osnovnim slučajem ili
`if constexpr`.

✅ Klasni šablon ceo u header-u. Metode koje rade samo za neke tipove su u
redu -- instanciraju se tek kad se pozovu.

✅ Poseban slučaj za jedan tip ili familiju tipova kod klasa:
specijalizacija (potpuna ili delimična). Kod funkcija: overload (s12).

✅ Alias šabloni sa `using`; zapamti da alias nije nov tip.

✅ Pretpostavke šablona o tipu zapiši kao `static_assert` sa porukom.

⚠️ Trait pitaj za "čist" tip (`remove_cv_t<remove_reference_t<T>>` ili
`decay_t<T>`), posebno u šablonu sa `T&&`.

⚠️ Unarni fold praznog paketa ne postoji za `+`, `*`...; koristi binarni sa
početnom vrednošću.

**Rezime:** klasni šablon je familija klasa, a njegove metode se prave
tek kad se koriste. Variadic šabloni primaju proizvoljan broj tipova, a
fold izrazi ih obrađuju bez rekurzije. Specijalizacija daje drugu
implementaciju za tip ili familiju tipova, i na njoj počivaju type traits
-- pitanja o tipovima pri kompajliranju, koja se koriste sa `if
constexpr` i `static_assert`.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_kruzni_bafer`](exercises/z1_kruzni_bafer.cpp) | upotreba | klasni šablon sa ne-tipskim parametrom, variadic metoda sa fold izrazom, static_assert i alias šablon (sekcije 2, 3, 6, 8) | — |
| [`z2_trait_i_referenca`](exercises/z2_trait_i_referenca.cpp) | zašto | zašto se tip "očisti" pre pitanja traitu (sekcija 7) | `-DNAIVNO` |
| [`z3_rekurzija_bez_kraja`](exercises/z3_rekurzija_bez_kraja.cpp) | zašto | zašto variadic rekurzija mora da ima kraj, i zašto je fold jednostavniji (sekcija 2) | `-DNAIVNO` |

## Zapažanja posle vežbe
