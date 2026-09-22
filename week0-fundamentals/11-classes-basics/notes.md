# 11 — Klase: osnove

Kako se pravi klasa: pristup, konstruktori i destruktor, podrazumevane
vrednosti članova, `this`, `static` i `const` članovi, copy konstruktor,
delegirajući konstruktori, `= default` i `= delete`. Kopiranje i pravila
generisanja ovde su samo uvod. Detaljno su u week1 s01–s02 (životni vek,
rule of 3) i week2 s06 (kad kompajler šta generiše).

**Izvori:** standard, delovi `[class]`, `[class.access]`,
`[class.default.ctor]`, `[class.base.init]` (init lista i delegiranje),
`[class.mem]` (podrazumevane vrednosti članova), `[class.this]`,
`[class.static]`, `[class.dtor]`, `[class.copy.ctor]`,
`[dcl.fct.def.default]` i `[dcl.fct.def.delete]`. Uz to *Effective C++*
**Item 4** (init lista i redosled), **Item 5** (šta kompajler generiše),
**Item 6** (zabrana kopiranja), **Item 22** (podaci su private),
*Effective Modern C++* **Item 11** (`= delete`), i C++ Core Guidelines
**C.2**, **C.45–C.48** i **C.51**.

**Kako vežbati:**

```
./build.sh week0-fundamentals/11-classes-basics/main.cpp      # svi ISPRAVNI slučajevi
./check_cases.sh week0-fundamentals/11-classes-basics         # svi POGREŠNI slučajevi
```

- `errors/` (e01–e10): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): kod koji se kompajlira, a ASan ga hvata pri pokretanju.
- Pogrešni slučajevi iz ranijih lekcija koji se odnose na klase:
  lekcija 03 `errors/e06` (`explicit`), `e09` (`int x(5);` kao član),
  `e19` (most vexing parse), `e20` i `e21` (`const` i referenca kao član
  bez init liste); lekcija 07 `errors/e04` i `e05` (`const` funkcije);
  lekcija 06 `errors/e11` (`static` član bez definicije).

---

# 1. `class` vs `struct` i enkapsulacija

`class` i `struct` su **isti** mehanizam. Jedina razlika je podrazumevani
pristup: `struct` je `public`, `class` je `private` (isto važi i za
nasleđivanje, lekcija 13).

Konvencija (Core Guidelines **C.2**):

- `struct` kad **nema invarijante**, tj. svaka kombinacija vrednosti je
  ispravna (`Point{x, y}`);
- `class` kad **ima invarijantu**, npr. `balance_ >= 0`. Podaci su tada
  `private` (EC++ Item 22), a jedini put do njih su funkcije koje čuvaju
  invarijantu.

```cpp
class Account {
public:
    bool withdraw(int amount);   // proverava da stanje ne ode ispod 0
private:
    int balance_ = 0;
};
a.balance_ = -100;   // ❌ private (errors/e01)
```

- **Pristup je po klasi, ne po objektu:** funkcija člana vidi `private`
  članove **drugog** objekta iste klase (`other.balance_`). Tako rade copy
  konstruktor i `operator==`.
- `private` postoji samo za kompajler; u memoriji nema zaštite.
- ⚠️ `getX()`/`setX()` za svaki član, bez provere, nije enkapsulacija.
  To je `struct` sa dužom sintaksom. Setter ima smisla kad čuva invarijantu.

---

# 2. Konstruktori i init lista

```cpp
class Sensor {
public:
    Sensor(int id, const std::string& location, int& readings)
        : id_(id), location_(location), readings_(readings) {}   // init lista
private:
    const int id_;
    std::string location_;
    int& readings_;
};
```

**Init lista inicijalizuje, telo konstruktora dodeljuje.** Kad se uđe u
telo, svi članovi su već napravljeni.

| Član | U init listi | Dodelom u telu |
|---|---|---|
| `int`, `std::string` | ✅ jedna inicijalizacija | radi, ali prvo default-init pa dodela |
| `const int` | ✅ | ❌ ne može (lekcija 03, `errors/e20`) |
| referenca | ✅ | ❌ ne može (lekcija 03, `errors/e21`) |
| član bez podrazumevanog konstruktora | ✅ | ❌ ne može |

**Podrazumevani konstruktor:**

- Kompajler ga pravi **samo ako nema nijednog** korisničkog konstruktora.
  Čim napišeš `Widget(int)`, `Widget w;` više ne radi (`errors/e02`).
  Vraća se sa `Widget() = default;` (sekcija 10).
- ⚠️ `Widget w();` nije objekat nego deklaracija funkcije (most vexing
  parse, lekcija 03).

**`explicit`** na konstruktoru sa jednim argumentom (Core Guidelines
**C.46**): bez njega `Temperature t = 21.5;` i `f(21.5)` za
`f(Temperature)` tiho prave objekat. Sa njim mora `Temperature t(21.5)`
(lekcija 03, `errors/e06`).

## Redosled inicijalizacije (EC++ Item 4, C.47)

Članovi se inicijalizuju **redom kojim su deklarisani u klasi**, a ne
redom u init listi. Destruktor ih uništava obrnutim redom.

```cpp
class Label {
public:
    Label(const char* t) : text_(t), decorated_("[" + text_ + "]") {}
private:
    std::string decorated_;   // ❌ deklarisan PRVI -> pravi se pre text_
    std::string text_;
};
```

Ovde `decorated_` čita `text_` pre nego što je napravljen. Test:
ispisuje `[]` umesto `[Ana]`. To je UB, ali ga **ni ASan ni UBSan ne
hvataju**. Jedina odbrana je upozorenje: g++ `-Wall` daje `-Wreorder`, a
clang `-Wall` daje `-Wreorder-ctor`. ✅ Init listu piši istim redom kao
deklaracije, i neka član ne zavisi od člana deklarisanog posle njega.
`main.cpp` ima ispravnu verziju (sekcija 2).

---

# 3. Destruktor

```cpp
~Room() { ... }   // bez parametara, bez povratne vrednosti, tačno jedan
```

Poziva se automatski: na kraju scope-a za lokalne objekte, na `delete` za
objekte sa heap-a, i posle `main` za globalne.

| Šta | Redosled |
|---|---|
| lokalne promenljive | obrnut od pravljenja: `a() b() ~b() ~a()` |
| konstrukcija klase | članovi redom deklaracije, **pa** telo konstruktora |
| destrukcija klase | telo destruktora, **pa** članovi obrnutim redom |

Test iz `main.cpp`: `lamp() desk() Room() | ~Room() ~desk() ~lamp()`.

Na tome počiva RAII: resurs se otvara u konstruktoru, a zatvara u
destruktoru, koji se pozove i kad izuzetak prekine funkciju (week1 s03).

---

# 4. Podrazumevane vrednosti članova (NSDMI, C++11)

```cpp
class Window {
public:
    Window() = default;                            // koristi vrednosti ispod
    explicit Window(int width) : width_(width) {}  // init lista pobeđuje
private:
    int width_ = 800;
    int height_{600};
    std::string title_ = "bez naslova";
};
```

- Oblik je `= vrednost` ili `{vrednost}`. ❌ `int height_(600);` ne radi,
  jer se čita kao deklaracija funkcije (lekcija 03, `errors/e09`).
- Ako init lista navede član, njena vrednost pobeđuje, a podrazumevana se
  ni ne računa.
- ✅ Core Guidelines **C.45/C.48**: konstantne podrazumevane vrednosti
  piši u deklaraciji, a ne u konstruktoru koji samo postavlja vrednosti.
  Tako svaki konstruktor dobija iste vrednosti i nijedan član se ne
  zaboravi.

---

# 5. `this`

- `this` je pokazivač na objekat za koji je funkcija pozvana. U `const`
  funkciji tip mu je `const Klasa*` (`main.cpp` to proverava sa
  `static_assert`).
- **Method chaining:** funkcija vraća `*this` **po referenci**
  (`Builder&`). ⚠️ Ako vraća po vrednosti (`Builder`), svaki sledeći poziv
  u lancu menja **kopiju**. Test: `add(1).add(2).add(3)` daje `6` sa
  referencom, a `1` sa vrednošću.
- Parametar istog imena kao član: `this->value = value` radi, ali uz
  `-Wshadow` (koji ovaj repo koristi) upozore i g++ i clang. Zato članovi
  ovde imaju sufiks `_` (`value_`).

---

# 6. `static` članovi

```cpp
class Connection {
public:
    static int alive() { return alive_; }  // nema this
    static constexpr int kMaxConnections = 8;
private:
    inline static int alive_ = 0;          // C++17: definicija u klasi
};
Connection::alive();                       // poziv bez objekta
```

- `static` podatak postoji **jednom** za celu klasu, i pre nego što
  nastane ijedan objekat.
- `static` funkcija nema `this`: ne vidi ne-static članove (`errors/e03`)
  i ne može biti `const` (`errors/e10`).
- ❌ `static int count = 0;` u klasi ne radi (`errors/e04`). ✅ C++17:
  `inline static int count = 0;`. Pre C++17 definicija ide u jedan `.cpp`
  (`int Counter::count = 0;`), a bez nje javlja linker (lekcija 06,
  `errors/e11`).
- `static constexpr` članovi su od C++17 implicitno `inline`.

---

# 7. `const` member funkcije

Detaljno u lekciji 07, sekcije 4 i 5. Ukratko:

- `int count() const;` obećava da ne menja objekat i jedina se sme pozvati
  na `const` objektu ili kroz `const&` (lekcija 07, `errors/e04`).
- ✅ Svaka funkcija koja ne menja objekat treba da bude `const`. Inače je
  klasu nemoguće koristiti kroz `const T&`, a tako se objekti najčešće
  prosleđuju.
- `mutable` i razlika bitwise/logical constness: lekcija 07.

---

# 8. Copy konstruktor

```cpp
Name(const Name& other);   // parametar MORA biti referenca (errors/e05)
```

Poziva se kad **novi** objekat nastaje kao kopija postojećeg:

| Kod | Šta se poziva |
|---|---|
| `Name b = a;`, `Name c(a);`, `Name d{a};` | copy konstruktor |
| `void f(Name n); f(a);` | copy konstruktor (parametar po vrednosti) |
| `void f(const Name& n); f(a);` | ništa, nema kopije |
| `return local;` | često se izostavi (copy elision, week2 s07) |
| `b = a;` (b već postoji) | **copy dodela** (`operator=`), ne konstruktor |

**Kompajlerov copy konstruktor kopira član po član.** Za `std::string` i
`int` je to tačno ono što treba. Za **sirov pokazivač koji poseduje
memoriju** to je plitka kopija: dva objekta dele istu memoriju i oba je
brišu (`ub/u02`, `attempting double-free`).

Rešenja, od najboljeg:

1. ✅ Ne drži sirov vlasnički pokazivač: `std::string`, `std::vector`,
   `std::unique_ptr`. Tada kompajlerova kopija radi ili je zabranjena
   (rule of 0).
2. ✅ Zabrani kopiranje: `Name(const Name&) = delete;` (sekcija 10).
3. Napiši duboku kopiju (`main.cpp`, sekcija 8). Tada ti trebaju i
   `operator=` i destruktor: **rule of 3**, week1 s02.

⚠️ Isto važi za **referencu kao član**: kompajler je kopira, ali ne čuva
objekat na koji pokazuje. Ako je vezana za privremeni objekat, visi čim
se konstruktor završi (`ub/u01`), i to bez ikakvog upozorenja.

---

# 9. Delegirajući konstruktori (C++11)

```cpp
Rect(int width, int height) : width_(width), height_(height) { /* provera */ }
Rect() : Rect(1, 1) {}                          // delegira
explicit Rect(int side) : Rect(side, side) {}   // delegira
```

✅ Core Guidelines **C.51**: zajednička logika (provera, podrazumevane
vrednosti) je u jednom "glavnom" konstruktoru, a ostali delegiraju njemu.

Pravila:

- ❌ Delegacija mora biti **jedina** stavka init liste (`errors/e06`).
  Ciljni konstruktor već inicijalizuje sve članove.
- ❌ Ciklus delegacija je greška. Direktan (`Rect(int) : Rect(int)`)
  odbijaju oba kompajlera (`errors/e07`). Ciklus preko dva konstruktora
  odbija **samo clang**, a g++ ga kompajlira bez upozorenja i program pukne
  od prepunjenog steka (`ub/u03`).
- Objekat se smatra **napravljenim čim se ciljni konstruktor završi**.
  Ako telo delegirajućeg konstruktora posle toga baci izuzetak, destruktor
  **se poziva**. Kod običnog konstruktora koji baci, destruktor se ne
  poziva. Test: `ciljni gotov telo delegirajućeg baca ~Tracked(0)`.

---

# 10. `= default` i `= delete` (C++11)

```cpp
class Token {
public:
    explicit Token(int v);
    Token() = default;                         // vrati podrazumevani
    Token(const Token&) = delete;              // zabrani kopiranje
    Token& operator=(const Token&) = delete;
};
```

**`= default`** traži od kompajlera da napravi funkciju kao da je sam
odlučio da je napravi.

- Razlika od praznog tela `Widget() {}`: sa `= default` konstruktor nije
  "korisnički", pa `Widget w{};` **postavi članove na nulu**, a tip ostaje
  trivijalan. Sa `{}` članovi bez podrazumevane vrednosti ostaju
  neodređeni. Test: `Defaulted{}.a = 0`;
  `is_trivially_default_constructible` je `true` za `= default`, a `false`
  za `{}`.
- ⚠️ `= default` ne garantuje da funkcija postoji. Ako kompajler ne može da
  je napravi (npr. `const` član ili referenca bez vrednosti), ona je
  **obrisana**, a greška se javi tek kad se objekat pravi (`errors/e09`).

**`= delete`** zabranjuje funkciju. Poziv je greška pri kompajliranju
(`errors/e08`), a ne tek pri pokretanju.

- Zabrana kopiranja (EC++ Item 6): pre C++11 se to radilo privatnim
  copy konstruktorom bez definicije. Kopija spolja je bila greška
  pristupa, ali kopija iz same klase se kompajlirala i padala tek kod
  linkera (`undefined reference to H::H(H const&)`). `= delete` daje
  grešku pri kompajliranju u oba slučaja.
- Radi i za obične funkcije i overload-e (EMC Item 11, lekcija 09).
- Polimorfne bazne klase zabranjuju kopiranje zbog slicing-a (C.67,
  lekcija 13).

Kad kompajler sam pravi koji konstruktor i dodelu, i kad ih briše, je tema
week2 s06 (EMC Item 17).

---

# Mapa na kurs

| Nastavak kursa | Sekcija |
|---|---|
| 41 OOP osnove, 42 Class, 44 Structures | 1 |
| 43 Constructor & Destructor | 2, 3 |
| 45 Non-static Data Member Initializers | 4 |
| 46 this Pointer | 5 |
| 47 Static Class Members | 6 |
| 48 Constant Member Functions | 7 (i lekcija 07) |
| 49–50 Copy Constructor | 8 (i week1 s02) |
| 51 Delegating Constructors | 9 |
| 52 Default & Deleted Functions | 10 (i week2 s06) |

---

# Pravilo za praksu

✅ `struct` za podatke bez invarijante, `class` sa `private` podacima kad
invarijanta postoji.

✅ Članove inicijalizuj u init listi ili podrazumevanom vrednošću u
deklaraciji, a ne dodelom u telu konstruktora.

✅ Init lista istim redom kao deklaracije; `-Wreorder` tretiraj kao grešku.

✅ Konstruktor sa jednim argumentom je `explicit`, osim kad je konverzija
namerna.

✅ Funkcija koja ne menja objekat je `const`.

✅ Jedan glavni konstruktor, ostali delegiraju njemu.

✅ `= delete` za kopiranje koje nema smisla; `= default` umesto praznog
tela.

⚠️ Sirov vlasnički pokazivač kao član znači da kompajlerova kopija ne
valja. Radije `std::string`/`std::vector`/`std::unique_ptr` (rule of 0).

⚠️ Referenca kao član ne produžava život objekta za koji je vezana.

**Rezime:** klasa je tip koji čuva svoju invarijantu. Konstruktor je
uspostavlja (init listom, redom deklaracije), funkcije je čuvaju, a
destruktor oslobađa ono što je konstruktor zauzeo. Kompajler za tebe piše
podrazumevani konstruktor, kopiju i destruktor. Ti odlučuješ da li su
dobri (`= default`), da li ih treba zabraniti (`= delete`), ili moraš da
ih napišeš sam (rule of 3, week1).

## Zapažanja posle vežbe

