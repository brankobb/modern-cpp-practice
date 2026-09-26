# Lekcija 08 — `namespace`, linkage i `inline`

Ova lekcija je o tome kako program od više `.cpp` fajlova postaje jedan
program: kako se imena grupišu (`namespace`), koja imena vidi linker
(linkage) i šta sme da se definiše u header-u (`inline`, ODR). Zato ima dva
fajla sa kodom, `main.cpp` i `util.cpp`, i zajednički `util.h`.

**Izvori:** standard, delovi `[basic.namespace]`, `[namespace.udecl]`
(using-deklaracija), `[namespace.udir]` (using-direktiva),
`[basic.lookup.argdep]` (ADL), `[basic.link]` (linkage), `[basic.def.odr]`
(One Definition Rule), `[dcl.inline]` i `[basic.start.dynamic]` (redosled
inicijalizacije). Uz to *Effective C++* **Item 4** (inicijalizacija pre
upotrebe, i između TU-ova), **Item 25** (sopstveni `swap` i
`using std::swap`) i **Item 30** (`inline`), i C++ Core Guidelines **SF.7**
i **SF.8**.

**Kako vežbati:**

```
./build.sh 1-language-basics/08-namespaces-and-linkage/main.cpp 1-language-basics/08-namespaces-and-linkage/util.cpp
./check_cases.sh 1-language-basics/08-namespaces-and-linkage
```

- `errors/` (e01–e11): kod koji se **ne kompajlira** (e01–e05) ili **ne
  linkuje** (e06–e11). Drugi `.cpp` fajlovi za linker su u `errors/support/`.
- `ub/` (u01): redosled inicijalizacije globalnih promenljivih između
  fajlova.

---

# 0. Pojmovi: TU, deklaracija, definicija

- **Translation unit (TU)**: jedan `.cpp` fajl posle preprocesora, sa svim
  uključenim header-ima. Kompajler prevodi **svaki TU posebno** i ne zna šta
  je u drugim.
- **Deklaracija** kaže da ime postoji i kog je tipa: `int add(int, int);`,
  `extern int callCount;`.
- **Definicija** pravi stvar: telo funkcije ili memoriju za promenljivu.
  Svaka definicija je i deklaracija.
- **Linker** spaja objektne fajlove i poziv `add` iz jednog TU-a povezuje sa
  definicijom iz drugog.

Zato postoje dve vrste grešaka. **Kompajler** javlja ono što se vidi u
jednom TU-u (e01–e05). **Linker** javlja ono što se vidi tek kad se spoje
svi (e06–e11). Tekst poruke zavisi od **linkera**, a ne od kompajlera:

| Linker | Dve definicije | Nema definicije |
|---|---|---|
| GNU `ld` (ovde i g++ i clang++; MSYS2 ucrt64 g++) | `multiple definition of` | `undefined reference to` |
| `lld` (MSYS2 clang64, ili `-fuse-ld=lld`) | `duplicate symbol:` | `undefined symbol:` |

`check_cases.sh` očekuje poruke GNU `ld`-a.

---

# 1. `namespace`: grupisanje imena

```cpp
namespace geo {
struct Point { int x, y; };
int manhattan(const Point& p);
}
geo::manhattan(p);                          // kvalifikovano ime

namespace company::project::detail { }     // C++17: ugnežđeni u jednom redu
namespace cpd = company::project::detail;  // alias
```

- Namespace se može **ponovo otvoriti**: `util` je u `util.h` i ponovo u
  `util.cpp`, i to je isti namespace.
- ❌ Ime iz namespace-a ne vidi se bez kvalifikacije: `cout` umesto
  `std::cout` (`errors/e01`).
- ❌ Kvalifikovana definicija `void util::b() {}` sme samo da definiše ime
  koje je već **deklarisano** u `util`, ne i da ga uvede (`errors/e05`).
  Tako se greška u kucanju ne pretvara tiho u novu funkciju.

---

# 2. using-deklaracija vs using-direktiva

| | using-deklaracija | using-direktiva |
|---|---|---|
| Oblik | `using std::cout;` | `using namespace std;` |
| Šta uvodi | **jedno** ime | **sva** imena iz namespace-a |
| Kako | kao da je ime deklarisano **ovde** | imena postaju vidljiva "sa strane" |
| Lokalna promenljiva istog imena | ❌ greška (`errors/e03`) | ✅ tiho je sakrije |
| Sukob sa drugim imenom | odmah, na `using` | tek na **mestu upotrebe** (`errors/e02`, `e04`) |

```cpp
using namespace audio;
using namespace video;
play();                 // ❌ audio::play ili video::play? (errors/e02)

using namespace std;
int count = 0;
count++;                // ❌ ::count ili std::count iz <algorithm>? (errors/e04)
```

Kod direktive greška nije na liniji `using namespace`, nego tamo gde se ime
upotrebi, i to često tek kad neko doda novo ime u jedan od namespace-a.
Kod lokalne promenljive koja sakrije ime iz direktive g++ ćuti, a clang
upozori tek sa `-Wshadow`.

✅ Pravilo (Core Guidelines **SF.7**): `using namespace` **nikad u
header-u**. U `.cpp` fajlu i u funkciji je prihvatljiv. Najbezbednije su
using-deklaracije za tačno ono što koristiš.

---

# 3. ADL (argument-dependent lookup)

Kod **nekvalifikovanog poziva funkcije**, ime se traži i u namespace-ima
tipova argumenata (`[basic.lookup.argdep]`).

```cpp
geo::Point p{1, 2};
print(p);           // nalazi geo::print, jer je p tipa geo::Point
std::cout << "x";   // operator<<(std::ostream&, const char*) iz std, nađen ADL-om
```

Bez ADL-a bi `std::cout << "x"` morao da se piše kao
`std::operator<<(std::cout, "x")`.

**Idiom za swap (EC++ Item 25):**

```cpp
namespace geo { void swap(Buffer& a, Buffer& b) noexcept; }   // uz tip, u ISTOM namespace-u

using std::swap;   // rezervna opcija
swap(a, b);        // ADL nađe geo::swap za Buffer; za int se koristi std::swap
```

⚠️ `std::swap(a, b)` bi uvek pozvao generički `std::swap` (tri move-a),
jer **kvalifikovan poziv isključuje ADL**. Standardna biblioteka zove
`swap` nekvalifikovano, pa i ona nalazi tvoj. Test: `std::reverse` na
`std::vector<geo::B>` sa 4 elementa pozove `geo::swap` 2 puta, a
`std::iter_swap` još jednom.

---

# 4. Linkage: koja imena vidi linker

| Linkage | Šta znači | Kako se dobija |
|---|---|---|
| **external** | isto ime u svim TU-ovima je **ista stvar** | podrazumevano za funkcije i ne-const promenljive na nivou namespace-a |
| **internal** | ime postoji samo u svom TU-u | `static` na nivou namespace-a, **anonimni namespace**, `const`/`constexpr` promenljiva na nivou namespace-a |
| **nema** | samo u svom scope-u | lokalne promenljive |

```cpp
// main.cpp                                   // util.cpp
namespace { std::string describe(); }         namespace { std::string describe(); }
// dve RAZLIČITE funkcije -- nema sukoba, linker ih ne vidi
```

- ✅ Pomoćne funkcije koje koristi samo jedan `.cpp` stavi u **anonimni
  namespace**. Tako se ne sudaraju sa istim imenima u drugim fajlovima.
  `static` radi isto za funkcije i promenljive, a anonimni namespace radi i
  za tipove.
- ❌ Funkcija iz anonimnog namespace-a nije dostupna iz drugog TU-a:
  `undefined reference to helper(int)` (`errors/e08`).
- **`extern`** na promenljivoj bez inicijalizatora je samo deklaracija:
  `extern int callCount;` ide u header, a `int callCount = 0;` u tačno
  jedan `.cpp`.
- ⚠️ **`const` na nivou namespace-a ima internal linkage** (razlika od C-a).
  `extern const int limit;` u jednom fajlu i `const int limit = 10;` u
  drugom ne linkuje se (`errors/e10`). ✅ Ispravno je
  `extern const int limit = 10;` u jednom `.cpp`, jer `extern` menja
  linkage.

---

# 5. ODR i `inline`: šta sme u header

**One Definition Rule** (`[basic.def.odr]`):

1. U jednom TU-u: najviše jedna definicija.
2. U celom programu: **tačno jedna** definicija svake ne-inline funkcije i
   promenljive koja se koristi.
3. **Izuzetak:** `inline` funkcije i promenljive, klase i template-i smeju da
   budu definisani u **svakom** TU-u koji ih koristi, pod uslovom da su
   **sve definicije identične**.

Header se kopira u svaki `.cpp` koji ga uključi. Zato u header ide samo ono
na šta se odnosi izuzetak 3:

| U header-u | Rezultat |
|---|---|
| `int twice(int x) { ... }` | ❌ `multiple definition` (`errors/e06`) |
| `int counter = 0;` | ❌ `multiple definition` (`errors/e09`) |
| `int add(int, int);` + definicija u `.cpp` | ✅ klasičan način |
| `inline int twice(int x) { ... }` | ✅ jedna funkcija u programu |
| `inline int counter = 0;` (C++17) | ✅ jedna promenljiva, ista adresa u svim TU-ovima |
| `extern int counter;` + `int counter = 0;` u `.cpp` | ✅ klasičan način za promenljivu |
| `static int x = 0;` / `static int f() {...}` | ⚠️ kompajlira se, ali **svaki TU ima svoju kopiju** |
| `const int x = 42;` | ⚠️ kompajlira se, kopija po TU (internal linkage) |
| `inline constexpr int x = 42;` | ✅ jedna konstanta |
| funkcija definisana **u telu klase** | ✅ implicitno `inline` |
| template funkcije i klase | ✅ implicitno izuzete |
| `constexpr` funkcija | ✅ implicitno `inline` |
| `static constexpr int n = 100;` u klasi | ✅ C++17: implicitno `inline` |
| `inline static int count = 0;` u klasi | ✅ C++17: definicija bez `.cpp` |
| `static int count;` u klasi, bez definicije | ❌ `undefined reference` (`errors/e11`) |

`main.cpp` (sekcija 5) to proverava: poredi adresu svake promenljive u
`main.cpp` i u `util.cpp` i pokazuje da `inline` funkcija ima **jedan**
`static` brojač, a `static` funkcija u header-u po jedan u svakom TU-u.

**`inline` danas znači "sme više definicija", ne "ubaci telo na mesto
poziva".** Da li će poziv biti zamenjen telom kompajler odlučuje sam, i
bez obzira na `inline` (`[dcl.inline]`: implementacija nije obavezna da to
uradi). EC++ Item 30 kaže isto: `inline` je molba kompajleru, ne naredba.

⚠️ **`#pragma once` / include guard ne rešava `multiple definition`.**
Guard sprečava da se header uključi dvaput u **jedan** TU (Core Guidelines
**SF.8**: svaki header ima guard). Linker vidi definicije iz **različitih**
TU-ova.

## Tiho kršenje ODR-a

Ako dve `inline` definicije istog imena **nisu identične**, to je UB bez
dijagnostike. Kompajler vidi jedan po jedan TU, a linker samo zadrži jednu
definiciju i ne poredi ih. Test sa `inline int version()` koji vraća `1` u
jednom fajlu i `2` u drugom (g++ 13 i clang 18, isti rezultat):

| Build | `fromA()` | `fromB()` |
|---|---|---|
| `-O0`, redosled `a.cpp b.cpp` | 1 | **1** |
| `-O0`, redosled `b.cpp a.cpp` | **2** | 2 |
| `-O2` (poziv se ubaci na mesto) | 1 | 2 |

Rezultat zavisi od redosleda fajlova i od nivoa optimizacije. ASan i UBSan
ovo ne hvataju. g++ sa `-flto` (`-Wodr`) prijavi samo kad se razlikuju
**tipovi** (`struct S` sa različitim članovima), a ne tela funkcija. Zato
nema primera u `ub/`. Odbrana je da svaka inline definicija postoji na
**jednom mestu** (u header-u) i da se nikad ne kopira ručno.

---

# 6. `inline namespace`: verzionisanje

```cpp
namespace lib {
inline namespace v2 { int api(); }   // lib::api() je lib::v2::api()
namespace v1 { int api(); }          // stara verzija i dalje dostupna kao lib::v1::api()
}
```

Tako su napravljeni `std::literals`, a libstdc++ tako drži novi
`std::string` u `std::__cxx11`. U običnom kodu retko treba, ali objašnjava
imena koja se vide u porukama kompajlera.

---

# 7. Redosled inicijalizacije globalnih promenljivih (EC++ Item 4)

Globalne promenljive sa **dinamičkom** inicijalizacijom (vrednost se računa
pri pokretanju, npr. `int base = compute();` ili `std::string`) se
inicijalizuju pre `main`:

- ✅ **u jednom TU-u** redom kojim su napisane;
- ❌ **između TU-ova** redosled nije određen (`[basic.start.dynamic]`).

```cpp
// a.cpp                          // b.cpp
extern int base;                  int base = compute();   // 41
int derived = base + 1;           // može da se izvrši PRE nego što je base izračunat
```

Test (g++ i clang++, bez sanitizera): `derived = 1` kad je `a.cpp` prvi pri
linkovanju, a `derived = 42` kad je drugi. Isti kod, drugačiji rezultat
zbog redosleda u build skripti (`ub/u01`). ASan ovo hvata samo sa
`check_initialization_order=1`, pa `ub/u01` to sam uključi.

**Rešenja:**

```cpp
int& base() { static int b = compute(); return b; }   // ✅ Meyers singleton (EC++ Item 4)
int derived = base() + 1;                             // base se računa pri PRVOM pozivu
```

- ✅ Globalnu promenljivu zameni funkcijom sa `static` lokalnom
  promenljivom. Lokalni `static` se inicijalizuje pri prvom pozivu, i od
  C++11 je to thread-safe. `util::defaultName()` u `main.cpp` sekciji 7
  radi tako.
- ✅ Ako je vrednost poznata pri kompajliranju, `constexpr` (C++17 i
  ranije) ili `constinit` (C++20) čine inicijalizaciju **statičkom**. Ona se
  obavi pre svake dinamičke, pa redosled ne postoji. Proveren je i
  `extern constexpr int base = 41;` i `constinit int base = compute();`
  (sa `constexpr compute`): oba daju 42 bez obzira na redosled.

---

# Pravilo za praksu

✅ Header: deklaracije, klase, template-i, `inline` funkcije i promenljive,
`inline constexpr` konstante. Sve ostale definicije idu u `.cpp`.

✅ Pomoćne funkcije i promenljive jednog `.cpp` fajla idu u anonimni
namespace.

✅ Svaki header ima `#pragma once` ili include guard (SF.8).

✅ Za swap sopstvenog tipa: `swap` u istom namespace-u, a na mestu poziva
`using std::swap; swap(a, b);` (EC++ Item 25).

⚠️ `using namespace` nikad u header-u (SF.7). U `.cpp` fajlu radije
using-deklaracije.

⚠️ `static` promenljiva ili funkcija u header-u pravi po kopiju u svakom
TU-u. Skoro nikad to nije ono što si hteo.

⚠️ Globalna promenljiva koja zavisi od globalne promenljive iz drugog
fajla: zameni je funkcijom sa `static` lokalnom promenljivom.

**Rezime:** kompajler vidi jedan `.cpp` fajl, a linker ceo program. Linkage
određuje da li je isto ime u dva fajla ista stvar (external) ili dve
različite (internal). ODR kaže da svaka stvar sa external linkage-om ima
tačno jednu definiciju, osim `inline`, a `inline` danas znači upravo
"definicija sme da bude u header-u". Većina grešaka iz ove lekcije ne
postoji dok se ne doda drugi `.cpp` fajl.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_namespace_adl`](exercises/ex1_namespace_adl.cpp) | usage | namespace, ADL, anonimni i inline namespace (sekcije 1, 3, 4, 6) | — |
| [`ex2_swap_adl`](exercises/ex2_swap_adl.cpp) | why | zašto "using std::swap; swap(a, b);" (sekcija 3, EC++ Item 25) | `-DNAIVNO` |
| [`ex3_redosled_globalnih`](exercises/ex3_redosled_globalnih.cpp) | why | zašto globalna ne sme da zavisi od globalne (sekcija 7, EC++ Item 4) | `-DNAIVNO` |

## Zapažanja posle vežbe

