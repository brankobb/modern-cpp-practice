# 05 — Složeni tipovi (compound types)

Standard deli tipove na dve grupe. **Fundamentalni** tipovi su ugrađeni u
jezik. **Složeni** (compound) tipovi se prave od drugih tipova. Pokazivače
i reference smo prošli u lekciji 04, a klase dolaze u 10–12. Ova lekcija
pokriva ostatak: nizove, `enum`, `union`, funkcijske tipove i imena za
tipove (`using`/`typedef`), plus njihove moderne zamene `std::array` i
`std::variant`.

**Izvori:** standard, delovi `[basic.fundamental]` i `[basic.compound]`
(podela tipova), `[dcl.array]` (nizovi), `[dcl.enum]` (nabrajanja),
`[class.union]` (union), `[dcl.typedef]` (`typedef` i alias), `[dcl.fct]`
(funkcijski tipovi), `[array]` i `[variant]` (standardna biblioteka). Uz to
*Effective Modern C++* **Item 9** ("Prefer alias declarations to typedefs")
i **Item 10** ("Prefer scoped enums to unscoped enums").

**Kako vežbati:**

```
./build.sh week0-fundamentals/05-compound-types/main.cpp               # svi ISPRAVNI slučajevi
./build.sh week0-fundamentals/05-compound-types/main.cpp -std=c++20    # + std::bit_cast
./check_cases.sh week0-fundamentals/05-compound-types                  # svi POGREŠNI slučajevi
```

- `errors/` (e01–e14): kod koji se **ne kompajlira**.
- `ub/` (u01): `std::array::operator[]` van granica.

---

# 1. Podela tipova

| Fundamentalni | Složeni |
|---|---|
| `bool`, `char` (i varijante), `int` (i varijante) | nizovi `T[N]` |
| `float`, `double`, `long double` | funkcije `R(Args...)` |
| `void` | pokazivači `T*` i reference `T&`, `T&&` (lekcija 04) |
| `std::nullptr_t` | pokazivači na članove `T C::*` (lekcija 04) |
| | klase i strukture (lekcije 11–13) |
| | `union` |
| | `enum` i `enum class` |

`<type_traits>` sve to proverava pri kompajliranju: `std::is_fundamental_v`,
`std::is_compound_v`, `std::is_array_v`, `std::is_enum_v`,
`std::is_union_v`, `std::is_function_v`... `main.cpp` ih koristi za
tabelu u sekciji 1.

---

# 2. C nizovi

```cpp
int a[5] = {1, 2, 3};   // 1 2 3 0 0 -- ostatak se popuni nulama
int b[]{4, 5, 6};       // veličina 3 se dedukuje
char s[] = "abc";       // 4 elementa -- i završna '\0'
int grid[2][3];         // niz od 2 niza od po 3 int-a, redom u memoriji
```

Pravila za C nizove:

- Veličina mora biti **konstantni izraz**.
  - ❌ `int arr[n];` sa promenljivom `n` (VLA) ne radi (`errors/e01`). To
    je C99 i kompajleri ga puštaju kao ekstenziju. clang ga čak i sa
    `-pedantic-errors` samo upozori, zato build koristi i `-Werror=vla`.
  - ❌ `int arr[0];` ne radi (`errors/e02`).
- ❌ **Niz se ne dodeljuje:** `a = b;` ne radi (`errors/e03`). Kopira se
  element po element, npr. `std::copy`.
- ⚠️ **`a == b` poredi ADRESE, ne sadržaj**, jer se oba niza raspadnu u
  pokazivač. Za sadržaj koristi `std::equal`. C++20 ovo poređenje
  proglašava zastarelim.
- ❌ Funkcija **ne može da vrati niz**: `int f()[3];` (`errors/e04`).
- Kao parametar funkcije niz se **raspada u pokazivač** i gubi veličinu
  (lekcija 04, sekcija 3).
- ⚠️ Pristup van granica je UB, a C niz ga ne proverava (lekcija 04,
  `ub/u02`).

---

# 3. `std::array`: C niz bez zamki

```cpp
std::array<int, 3> a{1, 2, 3};
auto b = a;          // ✅ kopija
a == b;              // ✅ poredi SADRŽAJ
a = makeArray();     // ✅ dodela, a funkcija ga sme i vratiti
a.size();            // ✅ zna svoju veličinu, ne raspada se u pokazivač
a.at(5);             // ✅ proverava granice -> std::out_of_range
a[5];                // ⚠️ NE proverava -- UB (ub/u01)
a.data();            // pokazivač, za C API
std::array c{1, 2, 3};   // C++17: dedukuje std::array<int, 3>
```

- Nema dodatne cene: `sizeof(std::array<int, 3>) == sizeof(int[3])`, a živi
  tamo gde i C niz (na steku, u objektu), ne na heap-u.
- ❌ Veličina je deo **tipa**: `array<int, 3>` i `array<int, 4>` su
  različiti tipovi (`errors/e14`).
- Kad veličina nije poznata pri kompajliranju, koristi `std::vector`.

---

# 4. `enum` i `enum class` (EMC Item 10)

**Obična (unscoped) enum**, nasleđena iz C-a:

```cpp
enum Color { Red, Green, Blue };
int n = Green;        // 1 -- tiho se pretvara u int
Blue < 14.5;          // kompajlira se (Color -> int -> double); C++20: zastarelo, upozorenje
```

Problemi:

- Imena "cure" u okolni scope, pa se dve enum sa istim imenom elementa
  sudaraju (❌ `errors/e06`).
- Tiha konverzija u broj omogućava besmislena poređenja i računanje.
- Bez navedenog tipa ne može da se unapred deklariše (❌ `errors/e10`),
  jer kompajler ne zna koliko je velika.

**`enum class` (scoped enum, C++11)** rešava sva tri problema:

```cpp
enum class Status { Ok, Error };
Status s = Status::Error;           // ime ide sa prefiksom
int n = static_cast<int>(s);        // konverzija samo eksplicitno
enum class Later;                   // unapred deklarisana -- podrazumevani tip je int
enum class Small : std::uint8_t { A, B };   // izabran tip -> sizeof == 1
```

- ❌ `int n = Status::Ok;` (`errors/e07`)
- ❌ `Status::Ok < 1` (`errors/e08`)
- ❌ `Status s = Ok;` (`errors/e09`). C++20 ima `using enum Status;` za
  uvoz imena.

Kad ti ipak treba broj, na primer indeks u `std::tuple`, EMC Item 10 predlaže
pomoćnu funkciju koja ne mora da zna tip:

```cpp
template <typename E>
constexpr std::underlying_type_t<E> toUType(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}
std::get<toUType(UserField::Email)>(info);
```

(C++23 ovo ima kao `std::to_underlying`.)

⚠️ **UB koji alati ne hvataju uvek:** enum **bez** navedenog tipa sme da
drži samo vrednosti koje staju u bitove potrebne za njene elemente. Za
`{Red, Green, Blue}` to je 0..3. `static_cast<Color>(8)` je UB od C++17.
g++-ov UBSan to ovde **ne prijavljuje** (provereno i sa
`-fsanitize=enum`). clang ima tu proveru u UBSan-u, ali je ovde nisam mogao
da pokrenem. Kod `enum class` i enum sa navedenim tipom, svaka vrednost
tog tipa je dozvoljena.

⚠️ `switch` nad enum: `-Wall` (`-Wswitch`) upozori kad neki element nije
obrađen. Zato ne stavljaj `default:` ako želiš to upozorenje.

---

# 5. `union`

Svi članovi dele **istu memoriju**, pa je veličina jednaka najvećem članu.
U svakom trenutku je aktivan **samo jedan** član.

```cpp
union Number { int i; float f; };
Number num;
num.i = 42;      // aktivan: i
num.f = 1.5f;    // sada je aktivan f; num.i više ne sme da se čita
```

⚠️ **Čitanje neaktivnog člana je UB u C++-u** (u C-u je dozvoljeno). To je
"type punning": `num.f = 1.0f; num.i;`. Sanitizeri ga ne hvataju. Ispravni
načini da se bitovi jednog tipa pročitaju kao drugi:

```cpp
std::memcpy(&bits, &value, sizeof bits);           // radi svuda
auto bits = std::bit_cast<std::uint32_t>(value);   // C++20, i constexpr
```

❌ `union` sa članom koji ima konstruktor/destruktor (npr. `std::string`)
gubi podrazumevani konstruktor i destruktor, jer ne zna koji član da
uništi (`errors/e11`). Za takve slučajeve postoji `std::variant`.

---

# 6. `std::variant` (C++17): union koji zna šta čuva

```cpp
std::variant<int, std::string> v;     // prva alternativa, value-init: int 0
v = 42;
std::holds_alternative<int>(v);       // true
v = std::string("tekst");             // aktivna je sada string
std::get_if<int>(&v);                 // nullptr -- bezbedna provera
std::get<int>(v);                     // pogrešna alternativa -> std::bad_variant_access (izuzetak, ne UB)
std::get<double>(v);                  // ❌ double nije alternativa -- greška pri kompajliranju (errors/e12)
std::visit(visitor, v);               // pozovi funkciju za aktivnu alternativu
```

- Variant čuva i **koja alternativa je aktivna**, pa je malo veći od
  najvećeg člana (40 bajtova naspram 32 za `std::string` u libstdc++).
- `std::visit` sa "overloaded" pomoćnikom (`main.cpp`, sekcija 6) je
  moderna zamena za `switch` nad ručnim tag + union parom.

---

# 7. `using` vs `typedef` (EMC Item 9)

```cpp
typedef void (*OldHandler)(int, const std::string&);   // ime je usred deklaracije
using NewHandler = void (*)(int, const std::string&);  // ime levo, tip desno
```

Oba daju **isti** tip. `using` je čitljiviji, a jedini može biti
**template**:

```cpp
template <typename T>
using MyList = std::vector<T>;          // ✅ alias template

template <typename T>
typedef std::vector<T> MyList;          // ❌ errors/e13
```

Stari način je bio `typedef` unutar strukture, ali u zavisnom kontekstu
traži `typename`:

```cpp
template <typename T> struct MyListOld { typedef std::vector<T> type; };
typename MyListOld<T>::type legacy;     // unutar template-a -- mora typename
MyList<T> modern;                       // bez typename
```

Zato C++14 ima `std::remove_const_t<T>` umesto
`typename std::remove_const<T>::type`.

---

# 8. Funkcijski tipovi

Funkcija ima tip, na primer `void(int)`. Nije objekat, pa:

```cpp
using Handler = void(int);       // tip FUNKCIJE
Handler* ptr = onStart;          // pokazivač na funkciju (ime se raspada u pokazivač)
Handler& ref = onStop;           // referenca na funkciju
Handler* table[] = {onStart, onStop};   // ✅ niz POKAZIVAČA na funkcije
void (handlers[2])(int);         // ❌ niz funkcija ne postoji (errors/e05)
```

Pokazivači na funkcije su detaljnije obrađeni u lekciji 09. U modernom
kodu se za callback obično koriste lambde i `std::function`.

---

# Moderni C++ stil

```cpp
std::array<int, 3> rgb{255, 128, 0};          // umesto int rgb[3]
std::vector<int> data(n);                     // umesto VLA
enum class LogLevel : std::uint8_t { Debug, Info, Error };
std::variant<int, double, std::string> cell;  // umesto union + tag
using Callback = std::function<void(int)>;    // umesto typedef
```

---

# Pravilo za praksu

✅ `std::array` umesto C niza fiksne veličine, a `std::vector` kad veličina
nije poznata pri kompajliranju.

✅ `enum class` umesto obične enum. Tip navedi kad je bitna veličina
(protokoli, embedded).

✅ `std::variant` umesto `union` kad god član ima konstruktor ili kad je
potrebno znati koji je član aktivan.

✅ `using` umesto `typedef`, svuda.

⚠️ `operator[]` ne proverava granice ni kod `std::array`. Koristi `.at()`
kad indeks dolazi spolja.

⚠️ Type punning preko `union` je UB. Koristi `std::memcpy` ili
`std::bit_cast`.

**Rezime:** C++ je nasledio iz C-a nizove, obične enum i `union`, i sva tri
imaju iste slabosti: nema provere, tiho gube informaciju (veličinu, scope,
aktivni član) i lako vode u UB. Moderni C++ za svaki ima zamenu bez
dodatne cene (`std::array`, `enum class`) ili sa malom cenom
(`std::variant`). Stari oblici ostaju za rad sa C API-jem i starim kodom.

## Zapažanja posle vežbe
