# Lekcija 16 — Nasleđivanje i polimorfizam

Kako se klasa gradi na drugoj klasi (nasleđivanje), kako poziv kroz
baznu referencu stigne do funkcije izvedene klase (`virtual`), i šta sve
tu može da pođe naopako: slicing, destruktor koji nije virtual, virtual
poziv iz konstruktora, sakrivanje imena.

**Izvori:** standard, delovi `[class.derived]`, `[class.virtual]`,
`[class.abstract]`, `[class.access.base]`, `[class.protected]`,
`[class.member.lookup]`, `[class.mi]` i `[namespace.udecl]` (nasleđeni
konstruktori). Uz to *Effective C++* **Item 7** (virtual destruktor),
**Item 9** (virtual u konstruktoru), **Item 32** (public = is-a),
**Item 33** (sakrivanje imena), **Item 38/39** (kompozicija i private
nasleđivanje), i C++ Core Guidelines **C.35**, **C.67**, **C.128**,
**C.130** i **C.133**.

**Kako vežbati:**

```
./build.sh 2-classes/16-inheritance-and-polymorphism/main.cpp      # ISPRAVNI slučajevi
./check_cases.sh 2-classes/16-inheritance-and-polymorphism         # POGREŠNI slučajevi
```

- `errors/` (e01–e13): kod koji se **ne kompajlira** (e13 ne linkuje;
  pomoćni fajl je u `errors/support/`).
- `ub/` (u01–u03): kod koji se kompajlira, a ASan/UBSan ga hvata.

---

# 1. Nasleđivanje i pristup

```cpp
class SavingsAccount : public Account { ... };   // SavingsAccount JESTE Account
```

**`public` nasleđivanje znači "is-a"** (EC++ Item 32): svuda gde se
očekuje `Account&` ili `Account*` sme da se prosledi `SavingsAccount`.
Ako to ne važi (kvadrat "jeste" pravougaonik, ali mu se ne može menjati
samo širina), nasleđivanje nije pravi alat.

| Član baze | Vidi ga izvedena klasa | Vidi ga ostatak programa |
|---|---|---|
| `public` | ✅ | ✅ |
| `protected` | ✅ (samo kroz sopstveni tip, `errors/e10`) | ❌ |
| `private` | ❌ (`errors/e06`) | ❌ |

⚠️ `protected` **podaci** su slaba enkapsulacija (C.133): svaka izvedena
klasa može da pokvari invarijantu baze. Bolje su `protected` funkcije.

| Vrsta nasleđivanja | `public` članovi baze postaju | Konverzija `Derived*` → `Base*` spolja |
|---|---|---|
| `public` | `public` | ✅ |
| `protected` | `protected` | ❌ |
| `private` (podrazumevano za `class`) | `private` | ❌ (`errors/e07`) |

---

# 2. Konstrukcija i destrukcija

```
frame() Vehicle(2) bell() Bike() | ~Bike() ~bell() ~Vehicle() ~frame()
```

- **Konstrukcija:** prvo **baza** (sa svojim članovima), pa **članovi
  izvedene klase**, pa **telo** izvedenog konstruktora. Destrukcija
  obrnuto.
- Argumenti za bazu idu u **init listu**: `Bike() : Vehicle(2), bell_("bell")`.
  Ako baza nema podrazumevani konstruktor, a init lista je ne navede,
  greška (`errors/e09`).
- **Nasleđeni konstruktori (C++11):** `using Vehicle::Vehicle;` u `Truck`
  daje `Truck(int)` bez pisanja.

---

# 3. Sakrivanje imena (EC++ Item 33)

```cpp
class Logger     { void log(int); void log(const std::string&); };
class FileLogger : public Logger { void log(double); };

fileLogger.log(5);            // FileLogger::log(double)! int -> double
fileLogger.log("tekst"s);     // ❌ ne kompajlira se (errors/e08)
```

Traženje imena staje u **prvom** scope-u gde nađe ime (`FileLogger`), pa
se `Logger::log` uopšte ne razmatra. Overload ne ide preko granice klase.

✅ `using Logger::log;` u izvedenoj klasi vraća sva imena iz baze, pa su
svi overload-i zajedno (`main.cpp`, sekcija 3).

Isto važi za **virtual** funkcije: ako potpis nije isti (npr. fali
`const`), izvedena funkcija ne nadjačava baznu, nego je **sakrije**. g++ i
clang sa `-Wall` upozore (`-Woverloaded-virtual`), a `override` to
pretvara u grešku (`errors/e03`).

---

# 4. `virtual`, `override`, `final`

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;     // pure virtual -> apstraktna klasa
    virtual std::string name() const;    // ima podrazumevanu implementaciju
};
class Circle : public Shape {
    double area() const override;        // override: kompajler proveri da nadjačava
};
class Square final : public Shape { ... }; // final: kraj hijerarhije
```

- **Dynamic dispatch:** poziv virtual funkcije **kroz referencu ili
  pokazivač** ide na funkciju **stvarnog** tipa objekta. Kroz objekat po
  vrednosti ide na funkciju statičkog tipa (slicing, sekcija 8).
- Kvalifikovan poziv `ref.Shape::name()` isključuje virtual i uvek zove
  baznu verziju.
- **Apstraktna klasa** (bar jedna `= 0` funkcija) ne može da ima objekte
  (`errors/e02`). Izvedena klasa koja ne implementira sve `= 0` funkcije
  je i sama apstraktna.
- ✅ **Uvek `override`** (C.128) na funkciji koja nadjačava. Greška u
  potpisu postaje greška pri kompajliranju (`errors/e03`), a ne tiha nova
  funkcija.
- `final` na klasi zabranjuje nasleđivanje (`errors/e04`), a na funkciji
  dalje nadjačavanje (`errors/e05`).
- ❌ Konstruktor ne može biti virtual (`errors/e11`).
- Podrazumevani argumenti se kod virtual funkcija biraju statički: lekcija
  11, sekcija 6 (EC++ Item 37).

---

# 5. Virtual destruktor (EC++ Item 7)

```cpp
Resource* r = new FileResource;
delete r;   // sa virtual ~Resource():   ~FileResource() ~Resource()
            // bez virtual:              UB -- ~FileResource se ne pozove (ub/u01)
```

`delete` kroz bazni pokazivač poziva destruktor **statičkog** tipa, osim
ako je destruktor virtual. Bez toga izvedeni deo nikad ne oslobodi svoje
resurse, a standard kaže da je to UB. Test: ASan prijavi
`new-delete-type-mismatch`, a kad se ta provera isključi, LeakSanitizer
prijavi string koji `~Label` nije oslobodio.

✅ Pravilo (C.35): destruktor bazne klase je **`public` i `virtual`**
(ako se briše kroz bazni pokazivač), ili **`protected` i ne-virtual**
(ako ne sme).

---

# 6. Virtual poziv u konstruktoru (EC++ Item 9)

```
u Widget(): Widget::kind; posle konstrukcije: Button::kind(OK)
```

Dok se pravi bazni deo, objekat **jeste** bazna klasa. Izvedeni deo još
ne postoji (`label_` nije napravljen), pa virtual poziv ide na funkciju
baze. Isto važi u destruktoru, obrnutim redom.

⚠️ Ako je ta funkcija u bazi **pure virtual**, program pukne sa `pure
virtual method called` (`ub/u02`). Direktan poziv oba kompajlera prijave,
ali kroz pomoćnu funkciju ne.

---

# 7. vptr i vtable (kurs 106–107)

Standard ne propisuje kako radi `virtual`; sve ispod je način na koji
rade g++ i clang (Itanium C++ ABI), proveren testom.

- Za svaku polimorfnu **klasu** postoji jedna **vtable**: niz adresa
  njenih virtual funkcija. Svaki **objekat** ima skriven pokazivač na
  vtable svoje klase (**vptr**), na početku objekta.
- Poziv `p->area()`: pročitaj vptr iz objekta, pročitaj adresu iz
  tabele na poznatom mestu, pozovi. To je jedan indirektan skok više od
  običnog poziva, i najčešće sprečava inline.

| Test (`main.cpp`, sekcija 7) | Rezultat |
|---|---|
| `sizeof(Plain)` / `sizeof(WithVirtual)` | 4 / 16 (8 bajtova vptr + `int` + poravnanje) |
| dva `Machine` objekta: isti vptr? | da (jedna vtable po klasi) |
| `Machine` i `Robot`: isti vptr? | ne |
| vptr objekta `Robot` **dok se pravi `Machine` deo** | isti kao kod `Machine` |
| `sizeof(Record)` za `Record : Printable, Storable` | 16: po jedan vptr za svaku polimorfnu bazu |

**vptr u toku konstrukcije:** konstruktor svake klase na početku postavi
vptr na **svoju** vtable. Dok radi konstruktor baze, objekat zaista
"jeste" baza, pa virtual poziv ide na njene funkcije (sekcija 6). Posle
konstruktora izvedene klase vptr pokazuje na njenu tabelu. Destruktori
rade isto, obrnutim redom.

**Devirtualizacija:** kad kompajler zna tačan tip, virtual poziv postaje
običan (i može inline). Test, `-O2`, funkcija koja prima `const T&` i
poziva `area()`:

| Tip parametra | g++ 13 | clang 18 |
|---|---|---|
| `Square` (nije `final`) | proveri vptr: ako je `Square::area`, inline; inače indirektan skok | indirektan skok |
| `Fixed` (`final`) | direktno, inline | direktno, inline |

`final` kaže kompajleru da ispod te klase nema izvedenih, pa `Fixed&` može
da bude samo `Fixed`. Isto važi za `final` na samoj funkciji.

---

# 8. Slicing

Kad se izvedeni objekat **kopira** u objekat bazne klase, kopira se samo
bazni deo. Izvedeni deo (podaci i override-i) se "odseca". Kompajler ne
javlja ni grešku ni upozorenje.

| Gde | Primer | Ispravno |
|---|---|---|
| parametar po vrednosti | `void f(Animal a)` | `void f(const Animal& a)` |
| kopija u bazni objekat | `Animal copy = rex;` | `const Animal& ref = rex;` |
| kontejner baznih objekata | `std::vector<Animal>` | `std::vector<std::unique_ptr<Animal>>` |

**Rešenje koje hvata grešku pri kompajliranju** (C.67): polimorfna bazna
klasa zabrani javno kopiranje (`= delete` ili `protected`). Tada se
slicing ne kompajlira (`errors/e01`).

---

# 9. Kopiranje polimorfnog objekta: `clone()` (C.130)

Kad kopija ipak treba, a imaš samo `Document&`, kopija mora biti **pravog**
tipa. Rešenje je virtual funkcija koja pravi kopiju:

```cpp
class Document {
public:
    virtual std::unique_ptr<Document> clone() const = 0;
protected:
    Document(const Document&) = default;   // samo izvedene klase kopiraju bazni deo
};
class Report : public Document {
    std::unique_ptr<Document> clone() const override { return std::make_unique<Report>(*this); }
};
```

---

# 10. Višestruko nasleđivanje i dijamant

```cpp
struct Printer : Device {};   struct Scanner : Device {};
struct Copier : Printer, Scanner {};   // DVA Device podobjekta -> c.id je dvosmisleno (errors/e12)
```

- `virtual` nasleđivanje (`struct Printer : virtual Device`) daje **jedan**
  zajednički `Device`.
- Virtual bazu tada pravi **najizvedenija** klasa (`Copier() : Device(3)`),
  a pozivi `Device(1)` i `Device(2)` iz `Printer` i `Scanner` se ignorišu.
  Test: ispisuje se samo `Device(3)`.
- ✅ U praksi: višestruko nasleđivanje od **interfejsa** (klase samo sa
  pure virtual funkcijama, bez podataka) je bezbedno i uobičajeno.
  Dijamant sa podacima je retko potreban.

---

# 11. Kompozicija vs private nasleđivanje (EC++ Item 38/39)

| | Znači | Kad |
|---|---|---|
| `public` nasleđivanje | "is-a" | izvedena klasa se koristi kao bazna |
| kompozicija (član) | "has-a" / "implementirano pomoću" | **podrazumevani izbor** za ponovnu upotrebu koda |
| `private` nasleđivanje | "implementirano pomoću" | retko: kad treba nadjačati virtual funkciju ili pristupiti `protected` delu |

Nasleđivanje samo da bi se "pozajmio kod" je najčešća greška. Kompozicija
je labavija veza: `Car` ima `Engine` i ne izlaže njegov interfejs.

---

# 12. Apstraktne klase i interfejsi (kurs 112)

```cpp
class Sink {                                         // interfejs: samo pure virtual, bez podataka
public:
    virtual ~Sink() = 0;
    virtual void log(const std::string& m) const = 0;
};
Sink::~Sink() = default;                             // MORA da postoji
void Sink::log(const std::string& m) const { ... }   // pure virtual SME da ima telo
```

- **Pure virtual destruktor** je način da klasa bude apstraktna kad nema
  nijednu drugu `= 0` funkciju. Ali destruktor izvedene klase uvek
  poziva destruktor baze, pa **definicija mora da postoji**; bez nje
  greška dolazi od linkera (`errors/e13`, `undefined reference to
  Base::~Base()`).
- **Pure virtual funkcija sa telom:** izvedena klasa mora da je
  nadjača, ali može da pozove podrazumevano ponašanje eksplicitno
  (`Sink::log(m)`). Test: `[console] [podrazumevano] poruka`.
- **Interfejs** (samo pure virtual funkcije i virtual destruktor, bez
  podataka) je najčistiji oblik nasleđivanja: višestruko nasleđivanje
  od interfejsa nema problem dijamanta (sekcija 10), a C++ Core
  Guidelines ga preporučuju za hijerarhije (C.121, C.129).

---

# Pravilo za praksu

✅ `public` nasleđivanje samo za "is-a"; za ponovnu upotrebu koda
kompozicija.

✅ Bazna klasa za polimorfizam: `virtual` destruktor, zabranjeno javno
kopiranje (C.67), po potrebi `clone()`.

✅ `override` na svakoj funkciji koja nadjačava; `final` gde hijerarhija
treba da stane.

✅ Polimorfne objekte čuvaj i prosleđuj kroz referencu ili (pametni)
pokazivač, nikad po vrednosti.

✅ `using Base::f;` kad izvedena klasa dodaje overload.

⚠️ Ne zovi virtual funkcije iz konstruktora i destruktora.

⚠️ `static_cast` naniže ne proverava tip (`ub/u03`). Za proveru
`dynamic_cast` (lekcija 17), a najčešće je bolja virtual funkcija.

**Rezime:** nasleđivanje daje dve stvari: izvedena klasa sadrži baznu (i
pravi se posle nje), i sme da se koristi tamo gde se očekuje bazna.
Polimorfizam radi samo kroz reference i pokazivače, samo za `virtual`
funkcije, i samo kad je objekat potpuno napravljen. Većina grešaka dolazi
od kopiranja u baznu klasu (slicing), brisanja kroz baznu klasu bez
virtual destruktora, i funkcija koje izgledaju kao override a nisu.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_oblici`](exercises/ex1_oblici.cpp) | usage | interfejs, override, final, virtualni destruktor i clone() (sekcije 4, 5, 9, 12) | — |
| [`ex2_override`](exercises/ex2_override.cpp) | why | zašto override (sekcija 4, EMC Item 12) | `-DNAIVNO`, `-DOVERRIDE` |
| [`ex3_slicing`](exercises/ex3_slicing.cpp) | why | zašto se polimorfni objekti ne čuvaju po vrednosti (sekcija 8) | `-DNAIVNO` |

## Zapažanja posle vežbe

