# 03 — Inicijalizacija u C++ (kompletan pregled)

Izvori: Effective Modern C++ Item 7 ("Distinguish between () and {} when
creating objects") za `{}` deo; ostalo je standardna terminologija
(cppreference "initialization"). main.cpp prati ovu strukturu sekciju po
sekciju, svaka tvrdnja je testirana kompajliranjem/pokretanjem pre nego što
je ušla ovde.

## 1. Default initialization

Bez ikakvog initializer-a.

```cpp
int x;          // NEINICIJALIZOVAN (garbage vrednost, UB ako ga čitaš pre pisanja)
std::string s;  // poziva default ctor -- string JESTE inicijalizovan (prazan)
```

Za klase, default-init zove default ctor; za primitivne tipove i članove
bez default member initializer-a, vrednost je nedefinisana.

## 2. Value initialization

Prazne vitičaste zagrade.

```cpp
int x{};     // 0
double d{};  // 0.0
bool b{};    // false
```

Za klase sa default ctor-om, poziva ga (isto kao default-init). Razlika se
vidi kod primitivnih tipova i agregatnih struktura bez ctor-a — value-init
ih ZERO-inicijalizuje, default-init ih ostavlja nedefinisanim.

## 3. Direct initialization

Obične zagrade, argumenti se direktno prosleđuju konstruktoru.

```cpp
int x(42);
std::string s("hello");
std::vector<int> v(10); // 10 elemenata
```

`explicit` konstruktori SE razmatraju ovde (vidi sekciju o `explicit`
ispod).

## 4. Copy initialization

Koristi `=`.

```cpp
int x = 42;
std::string s = "hello";
```

Uključuje implicitne konverzije. `explicit` konstruktori se NE razmatraju
kod copy-init — zato `Widget w = 5;` ne radi ako je `Widget(int)`
označen kao `explicit`, a `Widget w(5);` (direct-init) radi.

## 5. Uniform initialization (brace init, C++11)

```cpp
int x{42};
std::string s{"hello"};
T obj{args};   // radi (skoro) svuda -- ideja "uniform" u imenu
```

Postoje DVA oblika: *direct-list-init* (`T t{args};`) i *copy-list-init*
(`T t = {args};`) — kod oba važi narrowing provera, kod oba initializer_list
ctor ima prioritet; razlika je ista kao direct vs copy (explicit ctor se
razmatra samo kod direct-list-init).

---

## Najveća prednost `{}`: sprečava narrowing conversions

```cpp
double d = 3.14;
int x(d);   // radi, gubiš podatke (uz warning)
int x = d;  // radi, gubiš podatke (uz warning)
int x{d};   // COMPILE ERROR -- narrowing conversion
```

Zato mnogi timovi preferiraju `{}` kao default stil — greška se vidi na
kompajliranju, ne otkriva se u runtime-u meseci kasnije.

## `initializer_list` ima PRIORITET nad ostalim konstruktorima

Ako klasa ima BILO KOJI ctor koji prima `std::initializer_list<T>`, `{}`
sintaksa ga UVEK preferira ako je konverzija argumenata moguća — čak i kad
postoji "bolji"/tačniji match među ostalim konstruktorima, i čak i za
copy/move konstrukciju (ako klasa ima conversion operator koji otvara put
ka initializer_list tipu — vidi `initializerListHijack()` u main.cpp za
konkretnu demonstraciju sa `operator float()`).

Ako bi ta konverzija zahtevala narrowing, program se NE kompajlira — kompajler
se NE vraća tiho na drugi konstruktor, čak i da bi taj drugi bio savršen
match (`initializerListNarrowingError()`).

Kompajler se vraća na normalan overload resolution SAMO kad initializer_list
ctor uopšte nije viable (nema puta konverzije) — `initializerListFallback()`.

**Prazne `{}` znače "bez argumenata"** (default ctor), NE prazan
initializer_list. Pouzdan način da pozoveš initializer_list ctor sa
STVARNO praznom listom je `Widget({})`. Pažljivo: `Widget{{}}` NIJE isto
— testirano uživo (g++, `-std=c++17`): unutrašnje `{}` se tretira kao
JEDAN element liste koji se value-inicijalizuje (za `int` postaje 0), pa
dobijaš listu sa JEDNIM elementom, ne praznu (`emptyBracesMeaning()`).

## `auto` + `{}` — C++17 je promenio pravilo

```cpp
auto x{5};   // C++17+: x je int
auto y = {5}; // uvek: y je std::initializer_list<int>
```

Pre C++17, `auto x{5};` je deduktovao `std::initializer_list<int>` (isto
kao `y` ispod) — poznat izvor zabune, popravljeno u C++17 (P0433).
Testirano: na ovom kompajleru `typeid(x).name()` daje `i` (int), a
`typeid(y).name()` daje `St16initializer_listIiE`.

## `new` sa `{}`

```cpp
auto p1 = new int(42); // radi
auto p2 = new int{42}; // radi, isti rezultat
```

Oba rade identično za proste slučajeve; za tipove sa `initializer_list`
ctor-om važi ista "otmica" pravila kao gore.

## Aggregate initialization

Za tipove BEZ user-deklarisanog konstruktora, bez private/protected
non-static članova, bez virtual funkcija (i, od C++17, sa najviše javnim
base klasama):

```cpp
struct Point { int x; int y; };
Point p{1, 2}; // p.x=1, p.y=2 -- NEMA poziva konstruktora, direktno puni članove

int arr[]{1, 2, 3, 4}; // isto važi za nizove
```

## Designated initializers (C++20)

```cpp
struct Point { int x; int y; };
Point p{.x = 10, .y = 20};
```

Zahteva `-std=c++20` (`build.sh`/`build.ps1` prihvataju extra argument:
`./build.sh main.cpp -std=c++20`). Pravilo koje se lako zaboravi:
designatori MORAJU biti u istom redosledu kao deklaracija članova — `Point
p{.y = 20, .x = 10};` je GREŠKA, ne samo "čudno".

## Most Vexing Parse

```cpp
MyClass obj(); // NIJE objekat -- deklaracija funkcije koja vraća MyClass
MyClass obj{}; // UVEK objekat -- {} nema tu dvosmislenost
```

## Constructor initializer list vs assignment u telu

```cpp
A(int x) : m_x{x} {}     // INICIJALIZACIJA -- preporučeno
A(int x) { m_x = x; }    // ASSIGNMENT -- m_x se PRVO default-konstruiše, PA dodeli
```

Za proste tipove (`int`) razlika je samo u efikasnosti (jedna konstrukcija
manje). Za `const` članove i reference, assignment u telu se NE KOMPAJLIRA
— init lista je JEDINI način (vidi `constAndRefMembers()` u main.cpp).

---

## Pravilo za praksu (checklist)

- Primitivni tipovi i članovi klase: `{}` kao default (`int x{0};`)
- Konstruktorske liste: uvek init listu, ne assignment u telu
- Aggregate strukture (konfiguracije i sl.): `{}`, po potrebi designated
  initializers (C++20) za čitljivost
- STL kontejneri: **PAZI** — `std::vector<int> v(10);` (10 elemenata) vs
  `std::vector<int> v{10};` (1 element, vrednost 10) — najčešća greška na
  intervjuima i u produkcionom kodu
- Kad klasa ima `initializer_list` ctor: budi svestan da `{}` može da
  "otme" poziv koji si očekivao da ide na drugi konstruktor

## Zapažanja posle vežbe
