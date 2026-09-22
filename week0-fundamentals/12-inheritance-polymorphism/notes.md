# 12 — Nasleđivanje i polimorfizam (OOP bridge deo 3/3)

- `class Derived : public Base` — public nasleđivanje je skoro uvek ono što
  želiš ("is-a" odnos); `protected`/`private` nasleđivanje menja kako se
  Base-ovi javni/protected članovi vide iz Derived-a (retko potrebno, preskoči
  za sad ako nisi siguran zašto bi ti trebalo)
- `virtual` funkcija — omogućava dynamic dispatch (poziva se STVARNI tip
  objekta, ne deklarisani tip pokazivača/reference) — ovo je mehanizam koji
  ti je već pokazao slicing u week0 s06 i virtual destructor u week1 s01
- `= 0` (pure virtual) — čini klasu ABSTRAKTNOM, ne može se instancirati;
  koristi se za "interfejs" koji izvedene klase MORAJU implementirati
- `override` keyword (C++11) — eksplicitno kažeš "ovo treba da override-uje
  bazu"; ako se potpis ne poklapa (tipfeler, pogrešan `const`), kompajler
  javlja GREŠKU umesto da tiho napravi novu, nepovezanu funkciju
- podsetnik iz week1 s01: NIKAD ne zovi virtual funkciju iz ctor/dtor —
  tokom konstrukcije Base dela, vtable za Derived još nije "aktivna"

## API korišćen u vežbi

- `= 0` na virtual funkciji (pure virtual, `virtual double area() const = 0;`)
  — čini klasu ABSTRAKTNOM; klasa se NE MOŽE instancirati dok se ova
  funkcija ne implementira u nekoj izvedenoj klasi
- `std::vector<Shape*>` + ručni `new`/`delete` u petlji — namerno "sirov"
  pristup da pokaže mehaniku bez skrivanja iza biblioteke; u week2 s08
  ćeš ovo zameniti sa `std::vector<std::unique_ptr<Shape>>` koji čisti
  automatski i eliminiše mogućnost da zaboraviš `delete`

## Zapažanja posle vežbe
