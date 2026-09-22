# 13 — Nasleđivanje i polimorfizam (OOP bridge)

- `class Derived : public Base` — public nasleđivanje je skoro uvek ono što
  želiš ("is-a" odnos); `protected`/`private` nasleđivanje menja kako se
  Base-ovi javni/protected članovi vide iz Derived-a (retko potrebno, preskoči
  za sad ako nisi siguran zašto bi ti trebalo)
- `virtual` funkcija — omogućava dynamic dispatch (poziva se STVARNI tip
  objekta, ne deklarisani tip pokazivača/reference) — radi SAMO kroz
  pokazivač ili referencu; kopija u bazni objekat gubi pravi tip (slicing, ispod)
- `= 0` (pure virtual) — čini klasu ABSTRAKTNOM, ne može se instancirati;
  koristi se za "interfejs" koji izvedene klase MORAJU implementirati
- `override` keyword (C++11) — eksplicitno kažeš "ovo treba da override-uje
  bazu"; ako se potpis ne poklapa (tipfeler, pogrešan `const`), kompajler
  javlja GREŠKU umesto da tiho napravi novu, nepovezanu funkciju
- podsetnik iz week1 s01: NIKAD ne zovi virtual funkciju iz ctor/dtor —
  tokom konstrukcije Base dela, vtable za Derived još nije "aktivna"

## Slicing

Kad se objekat izvedene klase **kopira** u objekat bazne klase, kopira se
samo bazni deo. Izvedeni deo (njegovi podaci i override-i) se "odseca".
Kompajler ne javlja ni grešku ni upozorenje.

```cpp
std::string byValue(Animal a);         // parametar po vrednosti -> kopija samo Animal dela
std::string byRef(const Animal& a);    // bez kopije -> pravi tip ostaje

Dog rex("Rex");
byValue(rex);   // "..."        <- odsečeno, poziva se Animal::speak
byRef(rex);     // "Rex: Av!"
```

Slicing se dešava na tri mesta:

| Gde | Primer | Ispravno |
|---|---|---|
| parametar po vrednosti | `void f(Animal a)` | `void f(const Animal& a)` ili `const Animal*` |
| kopija u bazni objekat | `Animal copy = rex;` | `const Animal& ref = rex;` |
| kontejner baznih objekata | `std::vector<Animal>` | `std::vector<std::unique_ptr<Animal>>` |

**Pravilo:** polimorfne objekte (klase sa `virtual` funkcijama) prosleđuj i
čuvaj preko reference ili (pametnog) pokazivača, nikad po vrednosti.
Referenca kad objekat mora da postoji, pokazivač kad je opciono (lekcija
04, sekcija 9).

**Rešenje koje hvata grešku pri kompajliranju** (C++ Core Guidelines
C.67): polimorfna bazna klasa zabrani javno kopiranje.

```cpp
class SafeAnimal {
public:
    SafeAnimal() = default;
    SafeAnimal(const SafeAnimal&) = delete;
    SafeAnimal& operator=(const SafeAnimal&) = delete;
    virtual ~SafeAnimal() = default;
    ...
};
std::string byValue(SafeAnimal a);   // ❌ poziv se ne kompajlira (errors/e01)
```

Ako klasi ipak treba kopiranje, dodaje se virtual `clone()` funkcija koja
vraća `std::unique_ptr<Base>`.

Provera: `./check_cases.sh week0-fundamentals/13-inheritance-polymorphism`.

## API korišćen u vežbi

- `= 0` na virtual funkciji (pure virtual, `virtual double area() const = 0;`)
  — čini klasu ABSTRAKTNOM; klasa se NE MOŽE instancirati dok se ova
  funkcija ne implementira u nekoj izvedenoj klasi
- `std::vector<Shape*>` + ručni `new`/`delete` u petlji — namerno "sirov"
  pristup da pokaže mehaniku bez skrivanja iza biblioteke; u week2 s08
  ćeš ovo zameniti sa `std::vector<std::unique_ptr<Shape>>` koji čisti
  automatski i eliminiše mogućnost da zaboraviš `delete`

## Zapažanja posle vežbe
