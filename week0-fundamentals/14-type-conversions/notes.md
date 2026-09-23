# 14 — Konverzije tipova

Kada kompajler sam menja tip vrednosti (implicitne konverzije), kako se
to radi ručno (četiri imenovana cast-a) i kako sopstvena klasa učestvuje
u konverzijama: konstruktorom (iz drugog tipa u klasu) i operatorom
konverzije (iz klase u drugi tip).

**Izvori:** standard, delovi `[conv]` (standardne konverzije),
`[expr.static.cast]`, `[expr.const.cast]`, `[expr.reinterpret.cast]`,
`[expr.dynamic.cast]`, `[expr.cast]` (C-cast), `[class.conv.ctor]`,
`[class.conv.fct]` i `[over.best.ics]`. Uz to *Effective C++* **Item 27**
(što manje cast-ova), i C++ Core Guidelines **ES.46–ES.50**, **C.46** i
**C.164**.

**Kako vežbati:**

```
./build.sh week0-fundamentals/14-type-conversions/main.cpp     # ISPRAVNI slučajevi
./check_cases.sh week0-fundamentals/14-type-conversions        # POGREŠNI slučajevi
```

- `errors/` (e01–e10): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): kod koji se kompajlira, a UBSan/ASan ga hvata.
- Srodno iz ranijih lekcija: lekcija 01 (promocije, signed/unsigned),
  03 (`explicit`, narrowing, `errors/e08` dve korisničke konverzije),
  05 (`enum class`), 07 (`const_cast`, `ub/u01`), 13 (`static_cast`
  naniže, `ub/u03`).

---

# 1. Implicitne konverzije

Kompajler sam menja tip kad izraz to traži (`[conv]`):

| Konverzija | Primer | Napomena |
|---|---|---|
| integralna promocija | `char + char` → `int` | `'a' + 'b'` je `int` 195 |
| celobrojni → realni | `double d = 7;` | bez gubitka za `int` |
| realni → celobrojni | `int n = 3.99;` | **odseca** prema nuli: 3, a `-3.99` → -3 |
| signed → unsigned | `unsigned(-1)` | definisano, modulo 2ⁿ: 4294967295 |
| veći → manji ceo broj | `short(100000)` | C++20: modulo (-31072); ranije zavisi od implementacije |
| realni van opsega → ceo | `int(1e10)` | ❌ **UB** (`ub/u02`) |
| pokazivač → `bool` | `if (ptr)` | `nullptr` je `false` |
| izvedena\* → bazna\* | `Base* b = &derived;` | uvek bezbedno |
| niz → pokazivač, funkcija → pokazivač | | lekcije 05 i 09 |

⚠️ `-1 < 1u` je **netačno**: `-1` se pretvori u `unsigned` (4294967295)
pre poređenja. `-Wall -Wextra` upozori (`-Wsign-compare`), a C++20 ima
`std::cmp_less(-1, 1u)`, koje poredi matematički ispravno.

⚠️ `{}` inicijalizacija zabranjuje konverzije koje gube vrednost
(narrowing, lekcija 03). `=` i `()` ih tiho dozvoljavaju.

---

# 2. Četiri imenovana cast-a

| Cast | Za šta | Provera |
|---|---|---|
| `static_cast<T>(x)` | brojevi, `enum`, `void*` → `T*`, gore/dole kroz nasleđivanje, eksplicitni konstruktor/operator | pri kompajliranju; naniže **bez** provere tipa |
| `dynamic_cast<T>(x)` | naniže i "popreko" kroz polimorfnu hijerarhiju | **pri izvršavanju**: `nullptr` ili `std::bad_cast` |
| `const_cast<T>(x)` | skida `const` (lekcija 07) | nikakva; pisanje u pravi `const` objekat je UB |
| `reinterpret_cast<T>(x)` | pokazivač ↔ ceo broj, pogled na bajtove objekta | nikakva |

Svaki radi **jednu** vrstu posla, pa ostali prijave grešku:

- ❌ `static_cast` između nepovezanih pokazivača (`errors/e01`), iz
  pokazivača u broj (`errors/e08`), naniže kroz virtual bazu
  (`errors/e07`), niti skida `const` (lekcija 07, `errors/e16`).
- ❌ `reinterpret_cast` ne skida `const` (`errors/e02`).
- ❌ `dynamic_cast` naniže traži polimorfnu baznu klasu, tj. bar jednu
  virtual funkciju (`errors/e03`).

**C-cast `(T)x` i funkcijski `T(x)`** pokušavaju redom `const_cast`,
`static_cast`, `static_cast` + `const_cast`, `reinterpret_cast`,
`reinterpret_cast` + `const_cast`, i uzmu **prvi koji prolazi**. Test (g++
i clang, `-Wall -Wextra -pedantic-errors`, **bez ijednog upozorenja**):

```cpp
Account* a = (Account*)&celsius;   // nepovezani tipovi -> reinterpret_cast
int* px = (int*)&constX;           // skida const
Engine* e = (Engine*)&car;         // PRIVATE bazna klasa: ovo ne može ni jedan imenovani cast
```

✅ Zato (ES.48, ES.49): imenovani cast-ovi, jer se vidi namera i jer se
lako pronađu u kodu. Flag `-Wold-style-cast` (g++ i clang) upozori na
svaki C-cast.

✅ EC++ Item 27: svaki cast je mesto gde se zaobilazi sistem tipova.
Najbolji cast je onaj koji nije potreban: virtual funkcija umesto
`dynamic_cast`, pravi tip umesto `void*`, `enum class` sa funkcijom umesto
`static_cast<int>` svuda.

---

# 3. `reinterpret_cast` i bajtovi

- ✅ Čitanje bajtova **bilo kog** objekta kroz `unsigned char*`, `char*`
  ili `std::byte*` je dozvoljeno. Test: prvi bajt od `0x11223344` je
  `0x44`, dakle little-endian.
- ✅ Pokazivač → `std::uintptr_t` → pokazivač vraća isti pokazivač.
- ❌ **Strict aliasing** (`[basic.lval]`): objekat se ne sme čitati kroz
  pokazivač na **drugi** tip (osim `char`/`unsigned char`/`std::byte`).
  `*reinterpret_cast<int*>(&f)` za `float f` je UB. U testu vraća tačne
  bitove, ali g++ `-O2` upozori da je `f` **neinicijalizovan**, što znači
  da optimizator ne vidi vezu između `f` i čitanja. Nijedan sanitizer ovo
  ne hvata, pa nema primera u `ub/`.
  ✅ Ispravno: `std::memcpy(&bits, &f, sizeof f)`, ili C++20
  `std::bit_cast<std::uint32_t>(f)`.
- ❌ Pokazivač mora biti **poravnat** za tip: `reinterpret_cast<int*>(buffer + 1)`
  pa čitanje je UB (`ub/u03`, UBSan `misaligned address`).

---

# 4. `dynamic_cast`

```cpp
if (Dog* dog = dynamic_cast<Dog*>(&animal)) { ... }   // pokazivač: nullptr ako nije Dog
Dog& dog = dynamic_cast<Dog&>(animal);                // referenca: baca std::bad_cast
Pet* pet = dynamic_cast<Pet*>(&animal);               // cross-cast: druga grana hijerarhije
```

- Proverava stvarni tip pri izvršavanju, preko vptr-a (lekcija 13,
  sekcija 7). Zato radi samo za polimorfne tipove.
- ⚠️ Rezultat za pokazivač **mora** da se proveri. `d->volume` na
  `nullptr` je UB (`ub/u01`).
- `static_cast` naniže je brži, ali bez provere. Kad tip nije onaj koji
  misliš, to je UB (lekcija 13, `ub/u03`).
- ⚠️ Mnogo `dynamic_cast`-ova u kodu obično znači da fali virtual
  funkcija.

---

# 5. Konstruktor kao konverzija (primitivni → korisnički tip)

Konstruktor koji se može pozvati sa jednim argumentom je i **konverzija**
iz tipa tog argumenta:

```cpp
class Percent { public: Percent(int value); };
int half(Percent p);
half(50);                       // 50 -> Percent(50), tiho
```

| | `Percent p = 50;` | `f(50)` za `f(Percent)` | `Percent p(50);` |
|---|---|---|---|
| bez `explicit` | ✅ | ✅ | ✅ |
| sa `explicit` | ❌ (lekcija 03) | ❌ (`errors/e04`) | ✅ |

✅ C.46: konstruktor sa jednim argumentom je `explicit`, osim kad je
konverzija **bez gubitka i očigledna** (`Rational(int)` u lekciji 12,
`std::string(const char*)`).

U jednom implicitnom nizu konverzija sme najviše **jedna korisnička**:
`"Marko"` → `std::string` → `Name` je previše (lekcija 03, `errors/e08`).

---

# 6. Operator konverzije (korisnički → primitivni tip)

```cpp
explicit operator double() const;   // static_cast<double>(fraction)
explicit operator bool() const;     // if (connection)
```

- Nema povratni tip ni parametre: ime **je** tip (`errors/e09`).
- ⚠️ Bez `explicit` konverzija se dešava tiho i na neočekivanim mestima.
  Običan `operator bool()` dozvoljava `int n = conn;` i `conn + 1`, jer se
  `bool` dalje promoviše u `int`. Zato C.164: izbegavaj implicitne
  operatore konverzije.
- **`explicit operator bool`** radi u "kontekstu bool-a": `if`, `while`,
  `!`, `&&`, `||`, `?:`. U `bool b = conn;` ne radi (`errors/e05`), a
  `bool b(conn);` i `static_cast<bool>(conn)` rade. Tako rade
  `std::unique_ptr`, `std::optional` i stream-ovi.

---

# 7. Korisnički → korisnički tip

`Fahrenheit` → `Celsius` može na dva mesta:

- konstruktor `Celsius(const Fahrenheit&)` u **odredišnoj** klasi, ili
- `Fahrenheit::operator Celsius()` u **izvornoj** klasi.

❌ **Oba odjednom** su dvosmislena: `Celsius c = f;` se ne kompajlira
(`errors/e06`). `Celsius c(f);` se kompajlira (direct-init bira
konstruktor), pa greška ostaje skrivena do prvog poziva funkcije ili
dodele.

✅ Konverzija na **jednom** mestu, najčešće konstruktor u odredišnoj
klasi. Operator konverzije ima smisla kad odredišnu klasu ne možeš da
menjaš (npr. tip iz biblioteke).

---

# 8. Proverena konverzija brojeva

`static_cast<short>(100000)` tiho daje -31072. Kad vrednost dolazi
spolja (fajl, mreža, korisnik), proveri je:

```cpp
template <typename To, typename From>
To narrow(From value) {            // kao gsl::narrow iz Core Guidelines
    To result = static_cast<To>(value);
    if (static_cast<From>(result) != value || ((result < To{}) != (value < From{})))
        throw std::range_error("narrow");
    return result;
}
```

Druga provera hvata promenu znaka: `narrow<unsigned>(-1)` bi inače
prošao prvu proveru, jer `unsigned(-1)` vraćen u `int` opet daje -1.
`std::numeric_limits<T>::min()`/`max()` daju granice tipa.

---

# 9. `typeid` i RTTI (kurs 110)

```cpp
typeid(asAnimal)            // std::type_info DINAMIČKOG tipa (Animal je polimorfan)
typeid(*pointer)            // isto, preko pokazivača
typeid(pointer)             // tip POKAZIVAČA: Animal*
typeid(Dog) == typeid(x)    // tačno isti tip?
```

| Test (`main.cpp`, sekcija 9) | Rezultat |
|---|---|
| `typeid(asAnimal).name()` | `"3Dog"`: ime koje bira kompajler (Itanium ABI); čitljivo tek posle `abi::__cxa_demangle` (GNU proširenje) |
| `typeid(Plain&)` na `PlainChild` objektu | `Plain`: bez virtual funkcija nema dinamičkog tipa, pa je rezultat statički tip |
| `typeid(pointer)` / `typeid(*pointer)` | `Animal*` / `Dog` |
| `typeid(const int&) == typeid(int)` | `true`: const i referenca na vrhu se ignorišu |
| `Puppy` kao `Animal&`: `typeid == typeid(Dog)` | `false`, a `dynamic_cast<Dog*>` uspeva |
| `typeid(*nullptr)` polimorfnog tipa | baca `std::bad_typeid` |
| `std::map<std::type_index, int>` | brojanje po tipu; `type_info` se ne kopira, `type_index` je omotač za kontejnere |

**`typeid` vs `dynamic_cast`:**

- `typeid` pita "da li je objekat **tačno** ovog tipa".
- `dynamic_cast` pita "da li je objekat **ovog tipa ili izveden iz njega**"
  (is-a), što je skoro uvek pravo pitanje.
- Oba koriste **RTTI**: podatke o tipu koje kompajler čuva uz vtable.
  Zato rade samo za polimorfne tipove, a sa `-fno-rtti` ne postoje
  (`errors/e10`; poruke se razlikuju: g++ `cannot use 'typeid' with
  '-fno-rtti'`, clang `use of typeid requires -frtti`).

⚠️ Za polimorfni tip se izraz u `typeid(...)` **izvršava** (mora da se
nađe objekat). clang upozori kad izraz ima sporedne efekte
(`-Wpotentially-evaluated-expression`); za nepolimorfni tip se ne izvršava.

⚠️ `name()` nije prenosiv: format zavisi od kompajlera. Za ispis i
logovanje je u redu, za poređenje i ključeve koristi `type_info` ili
`type_index`.

✅ Najčešće ni `typeid` ni `dynamic_cast` nisu potrebni: virtual funkcija
(ili `std::variant` + `std::visit`) iskazuje isto bez pitanja o tipu.

---

# Mapa na kurs

| Nastavak kursa | Sekcija |
|---|---|
| 67 Basics | 1, 2, 3, 4 |
| 68 Primitive to User Type | 5 |
| 69 User to Primitive Type | 6 |
| 70 User Defined to User Defined | 7 |
| 110 `typeid` Operator | 9 |
| 111 `dynamic_cast` Operator | 4 |

---

# Pravilo za praksu

✅ Imenovani cast-ovi, nikad C-cast; `-Wold-style-cast` pomaže da se nađu.

✅ `explicit` na konstruktorima sa jednim argumentom i na operatorima
konverzije; `explicit operator bool` za "da li je važeće".

✅ Posle `dynamic_cast` na pokazivač uvek proveri `nullptr`.

✅ Bitovi drugog tipa: `std::memcpy` ili `std::bit_cast`, ne
`reinterpret_cast` na pokazivaču.

✅ Vrednost spolja: proverena konverzija (`narrow`), ne `static_cast`.

⚠️ Svaki cast je mesto gde je sistem tipova isključen. Pre cast-a
proveri da li problem rešava bolji tip ili virtual funkcija (EC++ Item 27).

**Rezime:** implicitne konverzije su zgodne dok ne gube vrednost; kad
gube (double → int, signed → unsigned, veći → manji), neka to bude
vidljivo u kodu. Imenovani cast kaže tačno šta se radi i odbija sve
ostalo, a C-cast uradi bilo šta što prolazi. Sopstvena klasa ulazi u
konverzije kroz konstruktor i operator konverzije, i oba treba da budu
`explicit`, osim kad je konverzija zaista očigledna.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_cast_dynamic`](exercises/z1_cast_dynamic.cpp) | upotreba | static_cast, dynamic_cast i konverzija između tipova (sekcije 2, 4, 5, 7) | — |
| [`z2_explicit_bool`](exercises/z2_explicit_bool.cpp) | zašto | zašto explicit operator bool (sekcija 6, C.164) | `-DNAIVNO`, `-DEXPLICIT` |
| [`z3_provereni_narrow`](exercises/z3_provereni_narrow.cpp) | zašto | zašto static_cast nije provera (sekcija 8) | `-DNAIVNO` |

## Zapažanja posle vežbe

