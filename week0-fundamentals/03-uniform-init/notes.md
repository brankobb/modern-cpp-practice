# 03 — Inicijalizacija u C++

Inicijalizacija je jedna od najvažnijih tema u C++-u, jer je kroz standarde
dodavano više načina da se objekat inicijalizuje. **Uniform initialization
(`{}`)** uvedena je u C++11 sa idejom da bude jedinstven način za sve — i
skoro je uspela.

**Izvori:** standard, delovi `[dcl.init]` (inicijalizacija), `[dcl.init.list]`
(list-initialization), `[dcl.init.aggr]` (agregati), `[over.match.list]`
(izbor konstruktora za `{}`), `[class.base.init]` (init lista) i
`[dcl.spec.auto]` (`auto`). Uz to *Effective Modern C++*: **Item 7**
("Distinguish between () and {} when creating objects"), **Item 2** (`auto`
i `{}`) i **Item 21** (`make_unique`/`make_shared`).

**Kako vežbati:**

```
./build.sh week0-fundamentals/03-uniform-init/main.cpp               # svi ISPRAVNI slučajevi
./build.sh week0-fundamentals/03-uniform-init/main.cpp -std=c++20    # + C++20 delovi
./week0-fundamentals/03-uniform-init/check_errors.sh                 # svi POGREŠNI slučajevi
```

`errors/` sadrži po jedan fajl za svaki **pogrešan** slučaj (e01–e23). Svaki
se NE kompajlira, a `check_errors.sh` proverava da pada na g++ i clang++, i
to baš iz razloga opisanog u fajlu. Na Windows-u pogledaj grešku za jedan
fajl ovako: `.\build.ps1 week0-fundamentals\03-uniform-init\errors\e01_narrowing_promenljiva.cpp -Compiler clang++`.

> **Napomena o kompajlerima.** Za neki kod standard kaže da je *neispravan*,
> ali od kompajlera traži samo "dijagnostiku", a warning se računa kao
> dijagnostika. Zato g++ `int x{d};` (narrowing iz promenljive) podrazumevano
> samo upozori i napravi program, a clang isto tako samo upozori na
> designated initializers van redosleda. Build zato koristi
> `-pedantic-errors` (i za clang `-Werror=reorder-init-list`): sve što
> standard zabranjuje postaje greška na oba kompajlera.

---

# 1. Default initialization

Bez ikakvog inicijalizatora.

```cpp
int x;          // lokalna promenljiva: NEODREĐENA vrednost (garbage)
std::string s;  // poziva default konstruktor -> prazan string
```

Klase:

```cpp
struct A { int x; };

A a;            // lokalno: a.x je neodređen
```

✅ Čitanje posle prvog upisa. ❌ Čitanje pre upisa je **undefined behavior**,
a ne "verovatno nula". Kompajler neće prijaviti grešku.

**Izuzetak koji se često zaboravlja:** promenljive sa *statičkim trajanjem*
(globalne, `static` lokalne, `static` članovi) prvo se **zero-inicijalizuju**,
pa su `0`:

```cpp
int g;                      // 0
void f() { static int s; }  // 0
```

Pravilo "`int x;` je garbage" važi samo za lokalne i dinamički alocirane
(`new int`) objekte.

---

# 2. Value initialization

Prazne zagrade.

```cpp
int x{};
```

ili stariji oblik:

```cpp
int x = int();
```

Rezultat:

```cpp
int x{};      // 0
double d{};   // 0.0
bool b{};     // false
int* p{};     // nullptr
```

Klase:

```cpp
struct A { int x; };

A a{};        // a.x == 0
```

Formalno, `A a{}` za **agregat** nije value-init nego *aggregate
initialization*, ali je rezultat isti: svi članovi su nula. Za klasu sa
korisničkim default konstruktorom, value-init poziva taj konstruktor.

❌ `int x();` **nije** value-initialization. To je deklaracija funkcije
(most vexing parse, sekcija 16).

---

# 3. Direct initialization

Koriste se obične zagrade.

```cpp
int x(42);
std::string s("hello");
std::vector<int> v(10);   // 10 elemenata, svi 0
```

Konstruktor se poziva direktno sa datim argumentima. Direct-init je i
`static_cast<T>(e)`, `new T(args)` i stavka init liste `m_x(x)`.

- **`explicit` konstruktori SE razmatraju:** `ExplicitOnly e(5);` ✅
- Na argumente se primenjuju obične implicitne konverzije:
  `Name n("Marko");` ✅ (`"Marko"` → `std::string` je jedna korisnička
  konverzija, u argumentu konstruktora).
- Narrowing se **tiho** dozvoljava: `int t(3.99);` daje 3, bez warning-a čak
  i uz `-Wall -Wextra`. Upozorenje daje tek `-Wconversion`.

---

# 4. Copy initialization

Koristi `=`.

```cpp
int x = 42;
std::string s = "hello";
MyClass obj = value;
```

Copy-init nije samo `=`. Isti mehanizam važi i za prosleđivanje argumenta
**po vrednosti**, `return` po vrednosti i `throw`/`catch` po vrednosti.

Pravila koja je razlikuju od direct-init:

- ❌ **`explicit` konstruktori se uopšte NE razmatraju:**
  `ExplicitOnly e = 5;` ne radi (`errors/e06`).
  Posledica koja iznenadi: kad klasa ima `explicit S(int)` i `S(long)`,
  onda `S a = 1;` bira **`S(long)`**, jer je `S(int)` izbačen iz izbora.
- ❌ **Najviše JEDNA korisnička konverzija:** `Name n = "Marko";` ne radi
  (`errors/e08`). Put je `"Marko"` → `std::string` → `Name`, a to su dve
  korisničke konverzije. `Name n("Marko");` radi (sekcija 3).
- ✅ **Od C++17 garantovani copy elision:** `std::atomic<int> a = 0;` radi,
  iako `std::atomic` nije kopirljiv. Objekat se pravi direktno, bez kopije.
  U C++14 (za koji je pisan EMC) ovo je bila greška (`errors/e23`).

---

# 5. Uniform initialization (brace / list initialization)

C++11.

```cpp
int x{42};              // direct-list-initialization
std::string s{"hello"};
int y = {42};           // copy-list-initialization
```

Ideja:

```cpp
T obj{args};
```

radi (skoro) svuda: za proste tipove, klase, agregate, nizove i članove
klase.

Dva oblika se razlikuju samo po `explicit` konstruktorima, i to drugačije
nego kod `()` i `=`:

| | `explicit` ctor |
|---|---|
| `S s{1};` (direct-list) | razmatra se i može da pobedi ✅ |
| `S s = {1};` (copy-list) | **razmatra se**, ali ako pobedi, program je neispravan ❌ (`errors/e07`) |
| `S s = 1;` (copy-init) | ne razmatra se uopšte, bira se drugi ctor |

Zato uz `explicit S(int)` i `S(long)`: `S s = 1;` → `S(long)`, a
`S s = {1};` → **greška**.

---

# 6. Najveća prednost `{}`: sprečava narrowing conversions

Zagrade i `=` tiho gube podatke:

```cpp
double d = 3.99;
int a(d);    // 3, bez upozorenja
int b = d;   // 3, bez upozorenja
```

Sa `{}` je **narrowing zabranjen** i to je greška pri kompajliranju:

```cpp
int x{d};    // ❌ error: narrowing conversion
```

Tačno pravilo (`[dcl.init.list]`): narrowing je konverzija koja *može*
izgubiti vrednost, i procenjuje se po **tipu**, ne po vrednosti. Izuzetak su
**konstante** koje stvarno staju u ciljni tip.

| Primer | | Zašto |
|---|---|---|
| `int x{3};` | ✅ | int konstanta |
| `char c{65};` | ✅ | konstanta staje u `char` |
| `float f{0.1};` | ✅ | double konstanta u opsegu float-a (tačnost ne mora da bude potpuna) |
| `double w{f};` | ✅ | float → double je proširenje |
| `long long big{i};` | ✅ | int → long long je proširenje |
| `int x{d};` | ❌ e01 | double → int iz promenljive |
| `int x{3.14};` | ❌ e02 | konstanta ne staje tačno |
| `unsigned u{-1};` | ❌ e03 | konstanta van opsega |
| `float f{d};` | ❌ e04 | double → float iz promenljive |
| `int i{ll};` | ❌ e05 | `long long` → `int`, čak i kad je `ll == 1` |

⚠️ Zavisnost od platforme: `long l = 1; int i{l};` je narrowing na Linux-u
(`long` ima 64 bita), a **nije** na Windows-u (`long` ima 32 bita, kao
`int`). Isti kod se na jednoj platformi kompajlira, a na drugoj ne.

Kad je gubitak nameran, napiši ga eksplicitno: `int e{static_cast<int>(d)};`.
Zbog ovoga mnogi timovi preferiraju `{}` svuda.

---

# 7. Uniform initialization za objekte

```cpp
class Person {
public:
    Person(std::string name, int age)
        : m_name{std::move(name)}
        , m_age{age}
    {}
private:
    std::string m_name;
    int m_age;
};

Person p{"Marko", 30};
```

`std::move(name)`: parametar je već kopija, pa ga premeštamo u član umesto
da ga kopiramo još jednom (week2 s07).

---

# 8. Default member initializers

```cpp
class Config {
public:
    Config() = default;
    explicit Config(int timeout) : timeout_{timeout} {}
private:
    int timeout_{5};
    int retries_ = 3;
    std::string name_{"default"};
};
```

- `Config a;` → `timeout_=5, retries_=3, name_="default"`
- `Config b{60};` → `timeout_=60`, jer stavka init liste **pregazi** default
  member initializer za taj član.
- Dozvoljeni oblici su `{}` i `=`. ❌ `int c(3);` ne radi (`errors/e09`):
  gramatika tu ne dozvoljava `()` (EMC Item 7).

---

# 9. Constructor initializer list

Ovo je prava inicijalizacija:

```cpp
A(int x) : m_x{x} {}      // inicijalizacija
```

A ovo je **dodela**:

```cpp
A(int x) { m_x = x; }     // m_x je već inicijalizovan pre tela; ovo je assignment
```

**Koliko je bitno zavisi od tipa člana:**

- Za `int` razlike u praksi nema, ali init lista je i dalje dobar stil.
- Za članove **tipa klase** (npr. `std::string`) dodela u telu znači
  **default konstruktor + `operator=`**, a init lista znači **jedan
  konstruktor**. `main.cpp` to pokazuje sa `Tracer` klasom.
- Init lista je **obavezna** (dodela u telu se ne kompajlira) za:
  - `const` članove (`errors/e20`)
  - reference (`errors/e21`)
  - članove bez default konstruktora
  - bazne klase bez default konstruktora

⚠️ **Redosled:** članovi se inicijalizuju redosledom **deklaracije u
klasi**, ne redosledom u init listi. `-Wall` upozorava (`-Wreorder`). Ako
jedan član zavisi od drugog, ovo je pravi bag (week1 s01).

---

# 10. Uniform initialization i STL zamka

Jedna od najpoznatijih zamki:

```cpp
std::vector<int> v1(10);      // 10 elemenata: 0 0 0 0 0 0 0 0 0 0
std::vector<int> v2{10};      // 1 element:   10
std::vector<int> v3(10, 20);  // 10 elemenata, svi 20
std::vector<int> v4{10, 20};  // 2 elementa:  10 20
```

A evo i obrnute zamke, koju malo ko zna:

```cpp
std::vector<std::string> v5{10};  // 10 PRAZNIH stringova, ne jedan string!
```

Razlog: `10` ne može da postane `std::string`, pa se `initializer_list`
konstruktor ne može pozvati i kompajler se vraća na `vector(size_type)`
(sekcija 11). Ista sintaksa `{10}` daje 1 element za `vector<int>` i 10 za
`vector<string>`.

Zato `{}` nije uvek isto što i `()`.

---

# 11. `initializer_list` prioritet (EMC Item 7)

```cpp
class A {
public:
    A(int, int)                  { std::cout << "ctor\n"; }
    A(std::initializer_list<int>) { std::cout << "init list\n"; }
};

A a(1, 2);   // ctor
A b{1, 2};   // init list
```

Tačno pravilo (`[over.match.list]`) ima dve faze:

1. Za `{}`, kompajler **prvo** razmatra SAMO `initializer_list`
   konstruktore. Ako se argumenti mogu konvertovati u element-tip, taj
   konstruktor pobeđuje, **čak i kad postoji tačan match** među ostalim
   konstruktorima:

   ```cpp
   Widget(int, bool);  Widget(int, double);  Widget(std::initializer_list<long double>);
   Widget w{10, true};  // initializer_list<long double>, ne (int, bool)
   Widget w{10, 5.0};   // initializer_list<long double>, ne (int, double)
   ```

2. Tek ako `initializer_list` konstruktor **uopšte nije moguć** (nema
   konverzije), razmatraju se ostali:

   ```cpp
   WidgetFallback(int, bool);  WidgetFallback(std::initializer_list<std::string>);
   WidgetFallback w{10, true}; // (int, bool): int ne može u string
   ```

❌ **Ako bi konverzija zahtevala narrowing, program je neispravan.**
Kompajler se NE vraća na drugi konstruktor (`errors/e10`):

```cpp
W(int, double);  W(std::initializer_list<bool>);
W w{10, 5.0};    // greška: 10 -> bool je narrowing, iako je W(int, double) savršen
```

**Prazne zagrade:**

```cpp
WidgetEmpty e1{};    // default konstruktor: {} znači "bez argumenata"
WidgetEmpty e2({});  // initializer_list konstruktor, PRAZNA lista
WidgetEmpty e3{{}};  // initializer_list, ali sa JEDNIM elementom (0)
```

Česta zabluda je da je `e3{{}}` prazna lista. Nije: unutrašnje `{}` je
jedan element liste, value-inicijalizovan u `0` (provereno na g++ i clang).

⚠️ **Slučaj koji zavisi od kompajlera:** `Widget w5{w4};`, kad `Widget`
ima i `initializer_list<long double>` konstruktor i `operator float()`.
g++ 13 bira `initializer_list` konstruktor (w4 → float → long double), što
odgovara važećem tekstu standarda (CWG 2137). clang 18 bira copy
konstruktor. Pouka: ne kombinuj `initializer_list` konstruktor sa
konverzijom u njegov element-tip, a kopiju piši sa `()`.

**Savet iz EMC Item 7:** kad klasi *dodaš* `initializer_list` konstruktor,
postojeći kod koji koristi `{}` može tiho da počne da poziva taj novi
konstruktor. Nema compile error-a, samo drugačije ponašanje.

---

# 12. `auto` i brace initialization

```cpp
auto a{5};          // int
auto b = {5};       // std::initializer_list<int>
auto c = 5;         // int
auto d = {1, 2, 3}; // std::initializer_list<int>
```

❌ `auto z{1, 2};` ne radi, jer direct-list sa `auto` traži tačno jedan
element (`errors/e11`).
❌ `auto w = {1, 2.0};` ne radi: elementi moraju biti istog tipa da bi se
dedukovao `initializer_list<T>` (`errors/e12`).

Istorija: EMC Item 2 (pisan za C++14) kaže da je i `auto a{5};` bio
`initializer_list<int>`. To je promenio dokument **N3922**, deo C++17.
Kompajleri su ga primenili i unazad kao ispravku (g++ od verzije 5, clang od
3.8), pa danas `auto a{5}` daje `int` i sa `-std=c++11`.

Sigurnije i čitljivije:

```cpp
auto x = 5;
int  y{5};
```

---

# 13. Dinamička alokacija

```cpp
auto p1 = new int{42};    // 42
auto p2 = new int(42);    // 42 -- oba rade
auto p3 = new int;        // default-init: NEODREĐENO
auto p4 = new int();      // value-init: 0   (ovde () NIJE most vexing parse)
auto p5 = new int{};      // value-init: 0
auto a1 = new int[5]{};       // 0 0 0 0 0
auto a2 = new int[5]{1, 2};   // 1 2 0 0 0
auto person = new Person{"Marko", 30};
```

Bitna razlika je između `new int` (garbage) i `new int()` / `new int{}`
(nula). Za `new T{args}` važe ista pravila kao za `T{args}` (narrowing,
`initializer_list` prioritet).

U modernom kodu se `new` direktno skoro ne piše: koristi se
`std::make_unique<T>(args)` (week2 s08). Ta funkcija interno koristi `()`,
vidi sekciju 17.

---

# 14. Aggregate initialization

Agregat je niz, ili klasa:

- bez korisničkih konstruktora (vidi razliku C++17 / C++20 ispod),
- bez private/protected nestatičkih članova,
- bez virtual funkcija,
- sa najviše javnim, ne-virtual baznim klasama (od C++17).

Takva klasa se puni direktno, redosledom deklaracije, bez poziva
konstruktora:

```cpp
struct Point { int x; int y; };

Point p{1, 2};          // x=1, y=2
Point q{1};             // x=1, y=0 (-Wextra ipak upozori na izostavljen član)
Point z{};              // x=0, y=0
Line  l{{0, 0}, {3, 4}}; // ugnežđeni agregati

int arr[]{1, 2, 3, 4};  // veličina 4 se dedukuje
int part[5]{1, 2};      // 1 2 0 0 0
```

❌ Previše inicijalizatora (`errors/e13`, `e15`).
❌ Narrowing važi i ovde: `Point p{1.5, 2};` (`errors/e14`).

**C++17 → C++20 razlike:**

| | C++17 | C++20 |
|---|---|---|
| `struct S { S() = default; int x, y; }; S s{1, 2};` | ✅ agregat (ctor nije *user-provided*) | ❌ nije agregat (ctor je *user-declared*), `errors/e22` |
| `Point p(1, 2);` | ❌ `errors/e16` | ✅ agregat i sa `()` (P0960) |

Druga promena je razlog što `std::make_unique<Point>(1, 2)` radi tek od
C++20.

---

# 15. Designated initializers (C++20)

Kao u C-u, ali strože.

```cpp
struct NetConfig { int timeout; int retries; bool verbose; };

NetConfig a{.timeout = 10, .retries = 3};  // verbose = false
NetConfig b{.retries = 5};                 // preskakanje je dozvoljeno: timeout = 0
```

Vrlo korisno za konfiguracione strukture. Pravila koja C nema:

- ❌ Redosled mora biti **redosled deklaracije**:
  `{.retries = 3, .timeout = 10}` ne radi (`errors/e17`).
- ❌ Ne sme se mešati sa pozicionim inicijalizatorima:
  `{.timeout = 1, 2}` ne radi (`errors/e18`).

---

# 16. Most vexing parse

Stara zamka sa `()`. Pravilo jezika: sve što *može* da se pročita kao
deklaracija **jeste** deklaracija.

```cpp
MyClass obj();            // NIJE objekat: deklaracija funkcije obj koja vraća MyClass
TimerWidget w(Timer());   // "pravi" most vexing parse: deklaracija funkcije w čiji
                          // je parametar funkcija koja vraća Timer
```

`main.cpp` ovo dokazuje sa `static_assert(std::is_function_v<decltype(obj)>)`.
❌ Pokušaj korišćenja kao objekta: `obj.value` ne radi (`errors/e19`).

Rešenja:

```cpp
MyClass obj{};              // uvek objekat
MyClass obj;                // takođe objekat
TimerWidget w{Timer{}};     // objekat
TimerWidget w((Timer()));   // i dodatne zagrade rešavaju
```

Zbog ovoga mnogi koriste brace initialization svuda.

---

# 17. Generički kod (EMC Item 7 i Item 21)

```cpp
template <typename T, typename... Ts>
T make(Ts&&... params) {
    return T(std::forward<Ts>(params)...);   // ili T{...}?
}

make<std::vector<int>>(10, 20);  // sa (): 10 elemenata; sa {}: 2 elementa
```

Autor template-a ne može da zna koje ponašanje pozivalac očekuje.
`std::make_unique` i `std::make_shared` zato **interno koriste `()`**, i to je
deo njihove dokumentacije. Posledica: `std::make_unique<std::vector<int>>(10, 20)`
pravi vektor sa 10 elemenata.

---

# Moderni C++ stil (što ćeš videti u ozbiljnom kodu)

```cpp
int count{0};
std::string name{"Marko"};
std::vector<int> values{1, 2, 3};
MyClass obj{arg1, arg2};

MyClass::MyClass(int x)
    : value_{x}
{}
```

---

# Pravilo za praksu

✅ Primitivni tipovi, sa vrednošću:

```cpp
int x{0};
double d{0.0};
```

✅ Članovi klase:

```cpp
int counter_{0};
```

✅ Konstruktorske liste (obavezno za `const`, reference, članove i baze bez
default ctor-a):

```cpp
MyClass(int x) : value_{x} {}
```

✅ Aggregate/konfiguracione strukture:

```cpp
Config cfg{.timeout = 10, .retries = 3};   // C++20
```

⚠️ Kontejneri i klase sa `initializer_list` konstruktorom. Ovde **svesno**
biraš `()` ili `{}`:

```cpp
std::vector<int> v(10);          // 10 elemenata
std::vector<int> v{10};          // jedan element: 10
std::vector<std::string> s{10};  // 10 praznih stringova
```

⚠️ Build sa `-pedantic-errors`, da bi "{} sprečava narrowing" važilo i na
g++.

EMC Item 7 zaključuje da nema konsenzusa da li je bolji `{}` ili `()` kao
podrazumevani stil. Izaberi jedan, drži ga se, a drugi koristi tamo gde
moraš.

**Rezime:** `{}` koristi kad god možeš, jer sprečava narrowing, izbegava
most vexing parse i daje konzistentan stil. Obavezno pazi na
`std::initializer_list` konstruktore, jer oni menjaju ponašanje u odnosu na
`()`. Kod klasa koje ih imaju (`std::vector`), `{}` i `()` nisu zamenljivi.

## Zapažanja posle vežbe
