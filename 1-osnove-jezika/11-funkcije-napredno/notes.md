# Lekcija 11 — Funkcije: overloading, podrazumevani argumenti, pokazivači na funkcije

Osnove funkcija (parametri, povratna vrednost, prosleđivanje po vrednosti)
su u lekciji 01. Ova lekcija pokriva ono što dolazi posle: više funkcija
istog imena i kako kompajler bira jednu od njih, podrazumevane argumente i
funkcije kao vrednosti (pokazivači, lambde, `std::function`).

`inline` i `namespace` su u lekciji 08. Tamo se vide tek sa više `.cpp`
fajlova.

**Izvori:** standard, delovi `[over.load]` (šta sme da se overload-uje),
`[over.match]` i `[over.ics.rank]` (overload resolution), `[dcl.fct.default]`
(podrazumevani argumenti), `[conv.func]` (funkcija u pokazivač) i
`[func.wrap.func]` (`std::function`). Uz to *Effective Modern C++* **Item
11** (`= delete`) i **Item 26** (ne overload-uj univerzalne reference), i
*Effective C++* **Item 37** (podrazumevani argumenti i virtual funkcije).

**Kako vežbati:**

```
./build.sh 1-osnove-jezika/11-funkcije-napredno/main.cpp        # svi ISPRAVNI slučajevi
./check_cases.sh 1-osnove-jezika/11-funkcije-napredno           # svi POGREŠNI slučajevi
```

- `errors/` (e01–e10): kod koji se **ne kompajlira**.
- `ub/` (u01): poziv kroz null pokazivač na funkciju.

---

# 1. Overloading: isto ime, različiti parametri

```cpp
int area(int side);
int area(int width, int height);   // drugi broj parametara
double area(double radius);        // drugi tip parametra
```

Overload se razlikuje **samo po listi parametara**. Ne računa se:

| Pokušaj | Zašto ne radi |
|---|---|
| `int f(); double f();` | ❌ samo povratni tip. Na mestu poziva se ne vidi koji je traženi (`errors/e01`) |
| `using Id = int; void f(int); void f(Id);` | ❌ alias nije novi tip (`errors/e02`) |
| `void f(int); void f(const int);` | ❌ top-level `const` nije deo potpisa (lekcija 09, `errors/e13`) |
| `void f(int*); void f(const int*);` | ✅ low-level `const` jeste deo potpisa |

---

# 2. Overload resolution: koji overload pobeđuje

Za svaki argument kompajler rangira koliko je "skupa" konverzija do tipa
parametra (`[over.ics.rank]`), od najbolje ka najgoroj:

1. **Tačan match**: isti tip. Tu spadaju i niz → pokazivač, funkcija →
   pokazivač i dodavanje `const`.
2. **Promocija**: `char`/`short`/`bool` → `int`, i `float` → `double`.
3. **Konverzija**: sve ostale ugrađene, npr. `int` → `double`,
   `double` → `int`, `int` → `long`, izvedena klasa\* → bazna\* i
   pokazivač → `bool`.
4. **Korisnička konverzija**: konstruktor ili operator konverzije, npr.
   `const char*` → `std::string`.
5. **`...`** (C varargs).

Pobeđuje overload koji je bar jednako dobar za sve argumente i bolji za
bar jedan. Ako takvog nema, poziv je **dvosmislen** i ne kompajlira se.

```cpp
void f(int);  void f(double);
f('a');    // f(int)     -- promocija
f(1.5f);   // f(double)  -- promocija
f(5L);     // ❌ long -> int i long -> double su obe KONVERZIJE (errors/e03)

void h(long); void h(double);
h(5);      // ❌ int -> long i int -> double: obe konverzije (errors/e04)
```

⚠️ **Zamka:** standardna konverzija uvek pobeđuje korisničku:

```cpp
void g(bool);
void g(const std::string&);
g("hello");   // g(bool)!  const char* -> bool je standardna konverzija
```

---

# 3. Overload po vrsti reference

```cpp
void take(int&);         // ne-const lvalue
void take(const int&);   // const lvalue -- i sve ostalo kad nema bolje opcije
void take(int&&);        // rvalue (privremeni, std::move)

take(x);             // take(int&)
take(cx);            // take(const int&)
take(3);             // take(int&&) -- && je bolji od const& za rvalue
take(std::move(x));  // take(int&&)
```

Na ovome počiva move semantika: copy konstruktor uzima `const T&`, a move
konstruktor `T&&` (lekcija 22).

---

# 4. Overload sa univerzalnom referencom (EMC Item 26)

```cpp
template <typename T> void logName(T&&);
void logName(int);

logName(1);       // logName(int) -- tačan match, a ne-template pobeđuje
short idx = 1;
logName(idx);     // TEMPLATE! T = short& je tačan match; int traži promociju
```

Funkcija sa univerzalnom referencom (`T&&`) je tačan match za **skoro
sve**, pa "otima" pozive koje si namenio drugim overload-ima. **Pravilo iz
EMC:** ne pravi overload-e pored funkcije sa `T&&`. Alternative (EMC Item
27): druga imena funkcija, prosleđivanje po `const T&` ili tag dispatch.

---

# 5. `= delete` na overload-u (EMC Item 11)

```cpp
bool isLucky(int number);
bool isLucky(char) = delete;
bool isLucky(double) = delete;

isLucky(7);     // ✅
isLucky('a');   // ❌ ne kompajlira se
isLucky(3.5);   // ❌ bez delete bi tiho postao isLucky(3) (errors/e10)
```

Obrisana funkcija **učestvuje** u overload resolution-u. Kad pobedi, poziv
je greška. Tako se zabranjuju implicitne konverzije koje ne želiš.

---

# 6. Podrazumevani argumenti

```cpp
void createUser(const std::string& name, int id = nextId());
createUser("Ana");      // id = nextId() -- izračunato SADA
createUser("Marko");    // id = nextId() -- izračunato ponovo
createUser("Vera", 99);
```

Pravila:

- Vrednost se **računa pri svakom pozivu**, na mestu poziva.
- ❌ Samo **poslednji** parametri smeju da je imaju:
  `void f(int a = 1, int b);` ne radi (`errors/e05`).
- ❌ Navodi se **jednom**, obično u deklaraciji u header-u, ne i u
  definiciji (`errors/e06`).
- ❌ Može da napravi dvosmislenost sa drugim overload-om:
  `void f(); void f(int = 0); f();` (`errors/e07`). Svaka deklaracija je
  ispravna, greška je tek na mestu poziva.

⚠️ **EC++ Item 37: ne menjaj podrazumevani argument u override-u.**

```cpp
struct Shape  { virtual void draw(Color c = Red) const; };
struct Circle : Shape { void draw(Color c = Green) const override; };

const Shape& s = circle;
s.draw();   // poziva Circle::draw -- ali sa Red!
```

Funkcija se bira **dinamički**, po stvarnom tipu objekta (virtual).
Podrazumevani argument se bira **statički**, po tipu izraza (`Shape`).
Rezultat je mešavina koju niko nije nameravao.

---

# 7. Pokazivači na funkcije i callback-ovi

```cpp
int add(int a, int b);
using BinaryOp = int (*)(int, int);   // čitljivije od int (*op)(int, int)
BinaryOp op = add;                    // ime funkcije se raspada u pokazivač
op(2, 3);
int apply(BinaryOp op, int a, int b) { return op(a, b); }   // callback
```

- ❌ Kod overload-ovane funkcije, overload se bira po **ciljnom tipu**, pa
  `auto p = &process;` ne radi (`errors/e08`).
  ✅ `void (*pd)(double) = process;` ili `static_cast<void (*)(int)>(process)`.
- ✅ Lambda **bez capture-a** se pretvara u pokazivač na funkciju.
- ❌ Lambda **sa capture-om** ne može (`errors/e09`), jer ima stanje. Za nju
  postoji `std::function<int(int)>`, ili `auto` / template parametar.
- ⚠️ Poziv kroz null pokazivač na funkciju je UB (`ub/u01`). Prazan
  `std::function` baca `std::bad_function_call`.

| | Pokazivač na funkciju | `std::function` | Template/`auto` parametar |
|---|---|---|---|
| Lambda sa capture-om | ❌ | ✅ | ✅ |
| Cena poziva | indirektan poziv | indirektan poziv, moguća alokacija | direktan, može inline |
| Prazan poziv | UB | izuzetak | — |

Pravilo: za callback koji se **čuva**, `std::function`. Za callback koji
se samo **poziva odmah** (kao u algoritmima), template parametar.

---

# Pravilo za praksu

✅ Overload-uj samo kad sve verzije rade **istu stvar** za različite tipove.
Inače daj različita imena.

✅ `= delete` na overload-ima koje ne želiš, umesto da se osloniš na to da
niko neće proslediti `double`.

✅ Podrazumevane argumente navodi u deklaraciji, jednom.

⚠️ Ne overload-uj pored funkcije sa `T&&` (EMC Item 26).

⚠️ Ne menjaj podrazumevane argumente u override-u virtual funkcije
(EC++ Item 37).

⚠️ `g("tekst")` sa overload-ima `bool` i `std::string` bira `bool`.

**Rezime:** overload resolution je rangiranje konverzija: tačan match, pa
promocija, pa konverzija, pa korisnička konverzija. Većina iznenađenja
dolazi odatle što je "najbolji" overload po pravilima jezika drugačiji od
onog koji je čovek imao na umu (`bool` umesto `string`, template umesto
`int`). Zato overload-e drži malobrojnim i jednoznačnim, a neželjene
kombinacije zabrani sa `= delete`.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_overload_callback`](exercises/z1_overload_callback.cpp) | upotreba | overloading, podrazumevani argumenti, callback (sekcije 1, 2, 6, 7) | — |
| [`z2_delete_konverzije`](exercises/z2_delete_konverzije.cpp) | zašto | zašto "= delete" na overload-u (sekcija 5, EMC Item 11) | `-DNAIVNO` |
| [`z3_univerzalna_referenca`](exercises/z3_univerzalna_referenca.cpp) | zašto | zašto ne overload-ovati sa univerzalnom referencom (sekcija 4, EMC Item 26) | `-DNAIVNO` |

## Zapažanja posle vežbe
