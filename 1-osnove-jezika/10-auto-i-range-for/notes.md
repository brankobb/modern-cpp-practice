# Lekcija 10 — `auto` i dedukcija tipova

C++11 je uveo `auto`: tip promenljive dedukuje kompajler iz inicijalizatora.
Pravila nisu nova: `auto` koristi (skoro) ista pravila kao **dedukcija za
template**, koja postoji od C++98. Zato je redosled u ovoj lekciji isti kao
u *Effective Modern C++*: prvo template, pa `auto`, pa `decltype`.

**Izvori:** standard, delovi `[temp.deduct.call]` (dedukcija iz argumenata
poziva), `[dcl.spec.auto]` (`auto` i `decltype(auto)`), `[dcl.type.decltype]`
i `[stmt.ranged]` (range-for). Uz to *Effective Modern C++* **Item 1**
(template type deduction), **Item 2** (auto type deduction), **Item 3**
(decltype), **Item 4** (kako videti dedukovani tip), **Item 5** (prefer
auto) i **Item 6** (kada auto daje pogrešan tip).

**Kako vežbati:**

```
./build.sh 1-osnove-jezika/10-auto-i-range-for/main.cpp        # svi ISPRAVNI slučajevi
./check_cases.sh 1-osnove-jezika/10-auto-i-range-for           # svi POGREŠNI slučajevi
```

- `errors/` (e01–e12): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): kod koji se kompajlira, ali **visi**. Svaki od njih
  dolazi od toga što je dedukovani tip referenca ili proxy, a ne vrednost.

`main.cpp` ispisuje dedukovane tipove pomoćnom funkcijom `typeName<T>()`
(sekcija 5). Uporedi ispis sa tabelama ispod.

---

# 1. Zašto `auto` (EMC Item 5)

```cpp
auto x = 5;          // ✅
auto y;              // ❌ bez inicijalizatora nema odakle da se dedukuje (errors/e01)
auto a = 1, b = 2.0; // ❌ jedna deklaracija = jedan tip (errors/e02)
```

**1. `auto` ne može da ostane neinicijalizovan.** Klasa bagova iz lekcije
03 (`int x;`) nestaje.

**2. `auto` ne može da napiše pogrešan tip.** Klasičan primer iz EMC Item 5:

```cpp
std::map<std::string, int> ages;
for (const std::pair<std::string, int>& p : ages)   // ⚠️ pogrešan tip
for (const auto& p : ages)                          // ✅
```

Element mape je `std::pair<const std::string, int>`, **ključ je `const`**.
Uz pogrešno napisan tip kompajler za svaki element pravi privremenu kopiju,
pa se referenca veže za kopiju. To znači i gubitak performansi i adresu
koja ne pokazuje u mapu. `main.cpp` to pokazuje poređenjem adresa, a g++ i
clang upozore (`-Wrange-loop-construct`).

**3. Neki tipovi se ne mogu ni napisati.** Tip lambde zna samo kompajler.
`std::function` može da je čuva, ali je veći i poziva se indirektno:

```cpp
auto square = [](int x) { return x * x; };   // sizeof == 1
std::function<int(int)> boxed = square;       // sizeof == 32 (libstdc++; zavisi od biblioteke)
```

Kada **ne** koristiti `auto`: kad tip nosi važnu informaciju koju čitalac
treba da vidi, i kad izraz vraća proxy (sekcija 6).

---

# 2. Dedukcija za template (EMC Item 1)

```cpp
template <typename T>
void f(ParamType param);
f(expr);   // kompajler iz expr dedukuje T i ParamType
```

Postoje **tri slučaja**, zavisno od oblika `ParamType`:

```cpp
int x = 27;
const int cx = x;
const int& rx = x;
```

**Slučaj 1: `T&` (ili `T*`).** Referenca iz argumenta se odbacuje, a
`const` ostaje:

| Poziv | T | param |
|---|---|---|
| `f(x)` | `int` | `int&` |
| `f(cx)` | `const int` | `const int&` |
| `f(rx)` | `const int` | `const int&` |

**Slučaj 2: `T&&` (univerzalna, tj. forwarding referenca).** Za **lvalue**
`T` postaje **referenca**:

| Poziv | T | param |
|---|---|---|
| `f(x)` | `int&` | `int&` |
| `f(cx)` | `const int&` | `const int&` |
| `f(rx)` | `const int&` | `const int&` |
| `f(27)` | `int` | `int&&` |

`T&&` je univerzalna referenca **samo** u tačno tom obliku, gde se `T`
dedukuje. `std::vector<T>&&` je obična rvalue referenca i ne prima lvalue
(`errors/e11`). Detaljnije u lekciji 27.

**Slučaj 3: `T` (po vrednosti).** `param` je nova kopija, pa se odbacuju i
referenca i `const`:

| Poziv | T |
|---|---|
| `f(x)`, `f(cx)`, `f(rx)` | `int` |
| `f(ptr)` gde je `const char* const ptr` | `const char*` |

Kod pokazivača se odbacuje samo `const` na **samom pokazivaču** (top-level).
`const` na podacima ostaje (lekcija 09, sekcija 10).

**Nizovi i funkcije:**

| Argument | `f(T param)` | `f(T& param)` |
|---|---|---|
| `const char name[13]` | `T = const char*` (raspad) | `T = const char[13]`, param `const char(&)[13]` |
| `void someFunc(int)` | `T = void(*)(int)` | `T = void(int)`, param `void(&)(int)` |

Referenca čuva veličinu niza, pa se veličina može dobiti pri
kompajliranju:

```cpp
template <typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept { return N; }
static_assert(arraySize(name) == 13);
```

---

# 3. Dedukcija za `auto` (EMC Item 2)

`auto` igra ulogu `T`, a ono što stoji uz njega igra ulogu `ParamType`.
Pravila su **ista kao u sekciji 2**:

| Deklaracija | Tip |
|---|---|
| `auto a = x;` | `int` |
| `const auto b = x;` | `const int` |
| `const auto& c = x;` | `const int&` |
| `auto&& d = x;` | `int&` (lvalue) |
| `auto&& e = cx;` | `const int&` |
| `auto&& f = 27;` | `int&&` (rvalue) |
| `auto g = name;` | `const char*` |
| `auto& h = name;` | `const char(&)[13]` |
| `auto i = someFunc;` | `void(*)(int)` |
| `const auto& k = 42;` | `const int&` (veže se i za privremeni) |
| `auto& r = 42;` | ❌ `errors/e05` — `auto&` je obična lvalue referenca |

**Jedina razlika od template-a je `{}`:**

```cpp
auto l = {27};      // std::initializer_list<int>
auto m{27};         // int (od C++17, lekcija 03)

template <typename T> void f(T);
f({1, 2, 3});       // ❌ template ne dedukuje iz {} (errors/e04)
```

⚠️ **`auto` kao povratni tip i kao parametar lambde koristi pravila za
template, ne za `auto` promenljive.** Zato `{}` tu ne radi:

```cpp
auto makeList() { return {1, 2, 3}; }   // ❌ errors/e03
```

❌ Sve `return` naredbe moraju dati isti tip:
`auto pick(bool c) { if (c) return 1; return 2.0; }` (`errors/e10`).

---

# 4. `decltype` i `decltype(auto)` (EMC Item 3)

`decltype(ime)` daje **deklarisani** tip, bez ikakvih pravila odbacivanja:

```cpp
int x = 0;
const int& cw = x;
decltype(x)    // int
decltype(cw)   // const int&
decltype((x))  // int&  <- (x) je IZRAZ (lvalue), ne ime
```

⚠️ Zagrade menjaju rezultat. `decltype` izraza koji je lvalue daje
referencu. `errors/e09` to otkriva: `decltype((x)) y;` traži
inicijalizator, jer je `y` referenca.

**`decltype(auto)`**: dedukuj tip, ali po pravilima `decltype`, pa se
referenca i `const` čuvaju:

```cpp
auto copy = cw;            // int
decltype(auto) same = cw;  // const int&
```

Glavna upotreba je povratni tip koji mora da prenese **tačno** ono što
vraća neka druga funkcija:

```cpp
template <typename Container, typename Index>
decltype(auto) authAndAccess(Container&& c, Index i) {
    return std::forward<Container>(c)[i];   // vraća int&, pa authAndAccess(v, 0) = 10; radi
}
```

Sa `auto` umesto `decltype(auto)` funkcija vraća kopiju, i dodela ne
prolazi (`errors/e12`).

⚠️ **Opasnost:** sa `decltype(auto)` jedan par zagrada menja povratni tip:

```cpp
decltype(auto) f1() { int x = 0; return x; }     // int
decltype(auto) f2() { int x = 0; return (x); }   // int& na lokalnu -> UB (ub/u02)
```

---

# 5. Kako videti dedukovani tip (EMC Item 4)

1. **Poruka o grešci (najpouzdanije).** Šablon bez definicije:

   ```cpp
   template <typename T> class TD;
   TD<decltype(y)> show;   // greška ispiše: TD<const int&>   (errors/e08)
   ```

2. **U programu:** ime funkcije koje daje kompajler sadrži `T`.
   `typeName<T>()` u `main.cpp` čita `__PRETTY_FUNCTION__` (g++ i clang;
   MSVC ima `__FUNCSIG__`). Za ozbiljnu upotrebu postoji Boost.TypeIndex.

3. ⚠️ **`typeid` nije pouzdan.** Po pravilima jezika odbacuje referencu i
   `const`, pa je `typeid(const int&) == typeid(int)`. `name()` uz to vraća
   ime zavisno od implementacije (npr. `i` za `int` na g++).

4. IDE tooltip: brzo, ali za složene tipove često netačno.

---

# 6. Kada `auto` daje pogrešan tip (EMC Item 6)

Neke klase vraćaju **proxy objekat** koji "glumi" drugi tip. Najpoznatija je
`std::vector<bool>`: ne čuva `bool`-ove nego bitove, pa `operator[]`
vraća `std::vector<bool>::reference`, a ne `bool&`.

```cpp
std::vector<bool> flags(10, true);
auto proxy = flags[5];   // tip: std::vector<bool>::reference, NE bool
proxy = false;           // menja VEKTOR -- flags[5] je sada false!
```

⚠️ Kad je vektor **privremen**, proxy visi:

```cpp
auto highPriority = features()[5];   // UB (ub/u01, ASan: heap-use-after-free)
```

**Rešenje: "explicitly typed initializer" idiom.** Kad izraz vraća proxy, a
želiš vrednost, reci to eksplicitno:

```cpp
bool highPriority = features()[5];                     // ✅
auto highPriority = static_cast<bool>(features()[5]);  // ✅ EMC Item 6 -- namera je vidljiva
```

Isto važi za biblioteke sa "expression templates" (npr. Eigen): tamo
`auto sum = a + b;` čuva recept za računanje, a ne rezultat.

---

# 7. Range-for

```cpp
for (auto item : items)        // KOPIJA svakog elementa -- izmene se gube
for (auto& item : items)       // referenca -- menja original
for (const auto& item : items) // čitanje bez kopije -- podrazumevani izbor
for (auto&& item : items)      // radi za sve, i za proxy (vector<bool>)
```

❌ `for (auto& b : flags)` sa `std::vector<bool>` ne radi, jer se `auto&` ne
veže za privremeni proxy (`errors/e06`). `auto&&` radi.

✅ **Structured bindings (C++17):**

```cpp
for (const auto& [name, age] : ages) { ... }   // umesto p.first / p.second
auto [it, inserted] = set.insert(5);           // std::pair iz insert
auto [a, b] = std::tuple{1, 2, 3};             // ❌ broj imena mora da se poklapa (errors/e07)
```

⚠️ **Životni vek u range-for.** Produžava se život **samo poslednjeg**
privremenog objekta u izrazu:

```cpp
for (int v : makeVector())           // ✅ vraćeni vektor živi do kraja petlje
for (int v : Holder{}.items())       // UB do C++23: Holder nestaje, items() visi (ub/u03)
```

C++23 (P2718) produžava život svih privremenih objekata u tom izrazu. U
C++17 i C++20 sačuvaj objekat u promenljivu: `Holder h; for (int v : h.items())`.

---

# 8. `auto` na drugim mestima

```cpp
auto twice(int x) { return 2 * x; }             // C++14: povratni tip iz return
auto add(int a, int b) -> int { return a + b; } // trailing return type (C++11)
auto plus = [](auto a, auto b) { return a + b; };  // generička lambda (C++14)
void print(auto x);                             // C++20: skraćeni zapis template-a
```

Povratni `auto` i `auto` parametri koriste pravila za **template**
(sekcija 2), ne za `auto` promenljive.

---

# Moderni C++ stil

```cpp
auto it = container.find(key);                  // dugačak tip iteratora
auto widget = std::make_unique<Widget>();       // tip se već vidi desno
for (const auto& [key, value] : map)            // čitanje mape
auto callback = [&](int x) { ... };             // lambda
int count = 0;                                  // kad je tip bitna informacija, napiši ga
auto ratio = static_cast<double>(a) / b;        // eksplicitna konverzija + auto
```

---

# Pravilo za praksu

✅ `auto` kad je tip očigledan (desno je `make_unique`, cast, konstruktor),
kad je dugačak (iteratori), ili kad ga ne možeš napisati (lambde).

✅ U range-for: `const auto&` za čitanje, `auto&` za izmenu, `auto&&` u
generičkom kodu. Obični `auto` samo kad ti treba kopija.

✅ Kad ti treba referenca ili `const`, napiši ih: `auto&`, `const auto&`.
`auto` sam ih uvek odbacuje.

⚠️ `auto` + proxy (`std::vector<bool>`, expression templates) = iznenađenje.
Upotrebi `static_cast<T>(...)`.

⚠️ `decltype(auto)` samo za povratne tipove koji prosleđuju referencu, i
nikad sa `return (lokalna);`.

⚠️ Range-for preko člana privremenog objekta visi do C++23.

**Rezime:** `auto` ne pogađa, nego primenjuje pravila template dedukcije.
Po vrednosti se odbacuju referenca i `const`, kroz `&` se `const` čuva, a
`&&` sa lvalue daje referencu. Jedini izuzetak je `{}`. Kad se pravila
jednom nauče, `auto` sprečava celu klasu bagova (neinicijalizovane
promenljive, pogrešni tipovi, skrivene kopije). Izuzetak su proxy tipovi,
kod kojih se tip mora napisati.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_structured_bindings`](exercises/z1_structured_bindings.cpp) | upotreba | auto, range-for i structured bindings (sekcije 1, 3, 7, 8) | — |
| [`z2_range_for_kopija`](exercises/z2_range_for_kopija.cpp) | zašto | zašto "auto&" / "const auto&" u range-for (sekcija 7) | `-DNAIVNO` |
| [`z3_auto_unsigned`](exercises/z3_auto_unsigned.cpp) | zašto | auto uzme TAČAN tip inicijalizatora, i kad je unsigned (sekcije 3, 6) | `-DNAIVNO` |

## Zapažanja posle vežbe
