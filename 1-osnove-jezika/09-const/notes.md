# Lekcija 09 — `const`

`const` je obećanje: **ovaj objekat se kroz ovo ime neće menjati**.
Kompajler to obećanje proverava. Zato je `const` jedan od retkih alata koji
istovremeno dokumentuje nameru i hvata greške pri kompajliranju. Scott
Meyers to sažima u jednu rečenicu: *"Use const whenever possible."*

**Izvori:** standard, delovi `[basic.type.qualifier]` i `[dcl.type.cv]`
(cv-kvalifikatori), `[conv.qual]` (konverzije koje dodaju `const`),
`[class.this]` (`const` member funkcije), `[dcl.stc]` (`mutable`),
`[expr.const.cast]` (`const_cast`), `[dcl.constexpr]` i `[expr.const]`
(`constexpr` i konstantni izrazi). Uz to *Effective C++* **Item 3** ("Use
const whenever possible") i *Effective Modern C++* **Item 13** (prefer
`const_iterator`), **Item 15** (`constexpr`) i **Item 16** (thread-safe
`const` member funkcije).

**Kako vežbati:**

```
./build.sh 1-osnove-jezika/09-const/main.cpp        # svi ISPRAVNI slučajevi
./check_cases.sh 1-osnove-jezika/09-const           # svi POGREŠNI slučajevi
```

- `errors/` (e01–e20): kod koji se **ne kompajlira**. Ovde je većina
  grešaka, jer je to i poenta `const`-a.
- `ub/` (u01–u02): kod koji se kompajlira, a sruši se, jer se kroz
  `const_cast` piše u memoriju koja je samo za čitanje.

---

# 1. Šta je `const`

```cpp
const int limit = 10;
limit = 20;               // ❌ errors/e02
const int x;              // ❌ mora odmah da dobije vrednost (errors/e01)
```

**`const` NE znači "poznato pri kompajliranju".** Vrednost sme da se
izračuna u runtime-u, samo se posle toga ne menja:

```cpp
const int fromSensor = readSensor();   // ✅
```

Za to postoji `constexpr` (sekcija 9).

**Const objekat klase** mora da ima vrednost za svaki član:

```cpp
const std::string s;   // ✅ string ima korisnički default ctor -> prazan
struct Plain { int x; };
const Plain a;         // ❌ x bi zauvek bio neodređen (errors/e14)
const Plain b{};       // ✅ x == 0
```

Napomena o linkovanju: `const` promenljiva na nivou namespace-a ima
*internal linkage*. `const int limit = 10;` u header-u daje svakom `.cpp`
fajlu njegovu kopiju. To je ispravno, a od C++17 postoji i
`inline constexpr` za jednu zajedničku.

---

# 2. `const` i pokazivači

`const` se odnosi na ono **levo** od sebe. Ako levo nema ničega, odnosi se
na ono desno. Zato se čita **s desna na levo**:

```cpp
const int* p;         // pokazivač na const int  (== int const* p)
int* const p = &x;    // const pokazivač na int
const int* const p;   // const pokazivač na const int
```

`const int*` i `int const*` su **isti tip** ("west const" i "east const",
stvar stila). Tabelu šta sme, a šta ne, pogledaj u lekciji 04, sekcija 6.

**Dodavanje `const` je implicitno, skidanje nije** (`[conv.qual]`):

```cpp
int* p = &x;
const int* cp = p;              // ✅
int** pp = &p;
const int* const* cpp = pp;     // ✅
const int** bad = pp;           // ❌ errors/e03 -- iznenađenje!
```

Zašto `int**` → `const int**` nije dozvoljeno, iako `int*` → `const int*`
jeste? Da jeste, ovako bi se bez ijednog cast-a menjao const objekat:

```cpp
const int c = 1;
int* p;
const int** cpp = &p;   // (kad bi bilo dozvoljeno)
*cpp = &c;              // p sada pokazuje na c
*p = 2;                 // menja const objekat!
```

Sa `const int* const*` drugi korak (`*cpp = &c`) nije moguć, pa je ta
konverzija bezbedna i dozvoljena.

---

# 3. `const` reference

```cpp
int x = 1;
const int& r = x;   // čitanje kroz r
r = 5;              // ❌ errors/e17
x = 2;              // ✅ x sam nije const; r sada čita 2
```

`const T&` je **podrazumevani oblik parametra** za objekte koje funkcija
samo čita (EC++ Item 20): nema kopije, a prima i privremene objekte.

```cpp
std::size_t countChars(const std::string& s);
countChars("literal");   // ✅ pravi privremeni std::string
```

❌ Parametar primljen kao `const T&` ne sme da se menja (`errors/e19`).

---

# 4. `const` member funkcije (EC++ Item 3)

```cpp
class TextBlock {
public:
    const char& operator[](std::size_t i) const;   // za const objekte
    char&       operator[](std::size_t i);         // za ne-const objekte
};
```

U `const` member funkciji `this` je tipa `const T*`. Pravila:

- ❌ Na `const` objektu mogu da se zovu **samo `const` funkcije**
  (`errors/e04`).
- ❌ `const` funkcija ne sme da menja članove (`errors/e05`).
- ❌ `const` funkcija ne sme da vrati ne-const referencu na član
  (`errors/e06`), jer bi pozivalac kroz nju menjao const objekat.
- ✅ Može se **overload-ovati po `const`**. Kompajler bira verziju po tome
  da li je objekat `const`.

**Kako izbeći dupliranje koda** (EC++ Item 3): ne-const verzija poziva
const verziju, pa sa rezultata skine `const`.

```cpp
char& operator[](std::size_t i) {
    return const_cast<char&>(static_cast<const TextBlock&>(*this)[i]);
}
```

Ovo je bezbedno jer je `*this` ovde sigurno ne-const. **Obrnuto nikako:**
`const` funkcija koja zove ne-const funkciju prekršila bi obećanje. C++23
isto rešava bez cast-ova ("deducing this").

**Bitwise vs logička konstantnost.** Kompajler proverava samo da se
**bitovi objekta** ne menjaju. Ako je član pokazivač, `const` funkcija
sme da menja ono na šta on pokazuje:

```cpp
class Buffer {
    char* data_;
public:
    void scribble() const { data_[0] = '#'; }   // ✅ kompajlira se -- data_ se ne menja
};
```

Ovo je legalno, ali za korisnika je objekat promenjen. **Logičku**
konstantnost ("objekat izgleda isto spolja") održavaš ti, ne kompajler.

---

# 5. `mutable` i thread-safety (EMC Item 16)

`mutable` član sme da se menja i u `const` funkciji. Služi za stanje koje
nije deo "vidljive" vrednosti objekta: keš, brojač poziva, mutex.

```cpp
class Polynomial {
public:
    double expensiveValue() const {        // spolja: čisto čitanje
        std::lock_guard<std::mutex> lock(mutex_);
        if (!cacheValid_) { cachedValue_ = compute(); cacheValid_ = true; }
        return cachedValue_;
    }
private:
    mutable std::mutex mutex_;
    mutable bool cacheValid_ = false;
    mutable double cachedValue_ = 0.0;
};
```

**EMC Item 16:** korisnici pretpostavljaju da `const` funkcije smeju da se
zovu iz više niti istovremeno, jer "samo čitaju". Čim `const` funkcija
menja `mutable` stanje, to stanje mora da bude zaštićeno mutex-om ili da
bude `std::atomic`.

Cena: `std::mutex` i `std::atomic` ne mogu ni da se kopiraju ni da se
premeštaju, pa **klasa gubi implicitni copy/move** (`errors/e20`).

---

# 6. `const` povratna vrednost

EC++ Item 3 (C++03 era) preporučuje da `operator*` vraća `const` objekat,
da bi besmislena dodela postala greška:

```cpp
const Rational operator*(const Rational&, const Rational&);
(a * b) = c;   // ❌ errors/e15
```

**U modernom C++-u to više nije dobar savet**, jer `const` povratna
vrednost ne može da se premesti:

```cpp
const Tracer makeConst();
Tracer makePlain();
t = makeConst();   // copy assignment -- move nije moguć jer je vrednost const
t = makePlain();   // move assignment
```

Isti cilj bez gubitka move-a daje **ref-qualifier** na `operator=`:
dodela je dozvoljena samo u lvalue.

```cpp
Rational& operator=(const Rational&) & = default;
Rational& operator=(Rational&&) & = default;
a = b * c;       // ✅ i to move
(a * b) = c;     // ❌ nema operator= za rvalue
```

**Pravilo:** ne vraćaj `const T` po vrednosti. `const T&` je nešto drugo i
potpuno je u redu.

---

# 7. `const_cast`

`const_cast` je **jedini** cast koji skida `const`. `static_cast` to ne
sme (`errors/e16`), pa se svako skidanje `const`-a vidi u kodu.

✅ Legitimno samo kad objekat **sam nije const**:

```cpp
void legacyLog(char* msg);                  // stari C API: ne menja msg
legacyLog(const_cast<char*>(s.c_str()));    // ✅ jer legacyLog ne piše

int x = 1;
const int* cp = &x;
*const_cast<int*>(cp) = 2;                  // ✅ x nije const
```

⚠️ **Upis u objekat koji JESTE definisan kao `const` je UB:**

```cpp
const int limit = 10;                        // globalni: memorija samo za čitanje
*const_cast<int*>(&limit) = 20;              // UB -- SEGV (ub/u01)
char* s = const_cast<char*>("hello");
s[0] = 'H';                                  // UB -- SEGV (ub/u02)
```

Za lokalni `const` UB je još podmukliji, jer program ne pukne:

```cpp
const int c = 1;
int* p = const_cast<int*>(&c);
*p = 2;
std::cout << c << " " << *p;   // "1 2" -- ista adresa, dve vrednosti
```

Kompajler je `c` ugradio kao konstantu 1. Nijedan sanitizer ovo ne
prijavljuje (provereno na g++ i clang).

---

# 8. `const` i STL (EMC Item 13)

```cpp
for (auto it = v.cbegin(); it != v.cend(); ++it) sum += *it;  // const_iterator
*v.cbegin() = 5;                                               // ❌ errors/e07

const std::vector<int> cv{1, 2, 3};
cv.push_back(4);                                               // ❌ errors/e09

const std::map<std::string, int> ages{{"Ana", 30}};
ages["Ana"];              // ❌ errors/e08 -- operator[] UBACUJE element kad ga nema
ages.at("Ana");           // ✅ baca std::out_of_range ako nema ključa
ages.find("Marko");       // ✅ vraća end() ako nema ključa

std::as_const(obj)[0];    // C++17: nateraj izbor const overload-a
for (const auto& x : v)   // podrazumevani oblik za čitanje
```

EMC Item 13: koristi `const_iterator` (`cbegin`/`cend`) kad god ne
menjaš elemente.

---

# 9. `const` vs `constexpr` (EMC Item 15)

| | `const` | `constexpr` |
|---|---|---|
| Menja se posle inicijalizacije | ne | ne |
| Vrednost poznata pri kompajliranju | nije obavezno | **obavezno** |
| `constexpr int n = rand();` | — | ❌ `errors/e11` |
| Veličina `std::array`, template argument | ❌ ako je runtime (`errors/e10`) | ✅ |

```cpp
constexpr int square(int n) { return n * n; }
constexpr int n = square(4);        // izračunato pri kompajliranju
std::array<int, n> a{};             // ✅
int r = square(readSensor());       // ✅ ista funkcija radi i u runtime-u
```

⚠️ **Suptilno pravilo:** `const int` sa konstantnim inicijalizatorom
**jeste** upotrebljiv u konstantnim izrazima, ali to važi samo za
celobrojne i enum tipove. `const double` nije:

```cpp
const int m = 10;        int arr[m];            // ✅
const double d = 1.0;    static_assert(d > 0);  // ❌ errors/e12
constexpr double e = 1.0; static_assert(e > 0); // ✅
```

**Pravilo:** za konstante poznate pri kompajliranju piši `constexpr`, ne
`const`. U C++20 postoje i `consteval` (funkcija se sme izvršiti **samo**
pri kompajliranju) i `constinit`.

---

# 10. Top-level vs low-level `const`

- **Top-level:** sam objekat je `const` (`const int x`, `int* const p`).
- **Low-level:** ono na šta se pokazuje ili referiše je `const`
  (`const int* p`, `const int& r`).

```cpp
const int ci = 1;
const int* const cpci = &ci;
auto a = ci;      // int          -- top-level const se odbacuje (kopija)
auto c = cpci;    // const int*   -- low-level ostaje, top-level odbačen
decltype(ci) d = 2;   // const int -- decltype čuva sve
```

Isto važi za dedukciju template-a po vrednosti (lekcija 10).

**Top-level `const` na parametru po vrednosti nije deo potpisa:**

```cpp
void f(int);
void f(const int);   // ❌ ista funkcija -- redefinicija (errors/e13)

void g(int*);
void g(const int*);  // ✅ low-level const JESTE deo potpisa -- dva overload-a
```

`void f(const int x) { ... }` u **definiciji** ima smisla: zabranjuje da
telo funkcije slučajno promeni `x`. Pozivaoca to ne zanima.

---

# 11. Lambde: `operator()` je podrazumevano `const`

```cpp
int x = 0;
auto bad  = [x]() { return ++x; };          // ❌ errors/e18
auto next = [x]() mutable { return ++x; };  // ✅ menja SVOJU kopiju
next(); next();                              // 1, 2 -- spoljni x ostaje 0
auto ref  = [&x]() { return ++x; };         // ✅ menja spoljni x
```

---

# Moderni C++ stil

```cpp
constexpr int kMaxUsers = 100;                    // konstanta poznata pri kompajliranju
const auto config = loadConfig();                 // izračunato jednom, ne menja se
void print(const Widget& w);                      // parametar: čitanje bez kopije
int size() const noexcept;                        // svaka funkcija koja ne menja objekat
for (const auto& item : items) { ... }            // iteracija bez izmene
auto it = std::find(v.cbegin(), v.cend(), x);     // const_iterator
```

---

# Pravilo za praksu

✅ Promenljive koje se ne menjaju označi `const`, a konstante poznate pri
kompajliranju `constexpr`.

✅ Parametre koje samo čitaš primaj kao `const T&` (ili po vrednosti, za
male tipove).

✅ Svaku member funkciju koja ne menja objekat označi `const`. Ako to ne
uradiš, ne može da se pozove na `const` objektu ni kroz `const&`.

✅ `mutable` samo za keš, mutex i brojače. Onda zaštiti `const` funkcije
za rad iz više niti.

⚠️ Ne vraćaj `const T` po vrednosti, jer blokira move. Za zabranu
`(a * b) = c` koristi `&` na `operator=`.

⚠️ `const_cast` samo za stare API-je i EC++ Item 3 obrazac. Upis u
stvarno `const` objekat je UB, a lokalni primer pokazuje da ga alati ne
hvataju uvek.

⚠️ `const` ne znači "compile-time", za to je `constexpr`. `const` ne
znači ni "thread-safe" ako postoji `mutable` stanje.

**Rezime:** `const` je obećanje koje kompajler proverava. Stavljaj ga svuda
gde važi: na promenljive, parametre, member funkcije i iteratore.
Kompajler proverava samo bitove objekta, a logičku konstantnost i
thread-safety `mutable` stanja čuvaš ti.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_const_metode`](exercises/z1_const_metode.cpp) | upotreba | const metode, mutable keš i const/non-const par (sekcije 4, 5, 7) | — |
| [`z2_const_zarazno`](exercises/z2_const_zarazno.cpp) | zašto | zašto const metode od samog početka (sekcije 3, 4) | `-DNAIVNO` |
| [`z3_mutable_lambda`](exercises/z3_mutable_lambda.cpp) | zašto | zašto je operator() lambde podrazumevano const (sekcija 11) | `-DNAIVNO`, `-DMUTABLE` |

## Zapažanja posle vežbe
