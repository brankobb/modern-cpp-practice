# 10 — Klase, enkapsulacija, this, static (OOP bridge deo 1/3)

- `struct` i `class` su IDENTIČNI osim: `struct` default je `public`
  (članovi i nasleđivanje), `class` default je `private`. Konvencija: `struct`
  za "gomilu podataka bez invarijanti" (POD), `class` kad postoji invarijanta
  koju čuvaš privatnim podacima
- enkapsulacija = privatni podaci + javni interfejs koji ČUVA invarijantu
  (npr. `setAge(int a)` koji odbija negativan broj, umesto javnog `int age`)
- `this` je implicitni prvi parametar svake non-static member funkcije,
  tipa `ClassType*` (ili `const ClassType*` u `const` metodi) — koristi se
  eksplicitno za method chaining (`return *this;`) ili da razdvoji parametar
  od člana istog imena
- `static` član — DELI se između SVIH instanci, postoji čak i bez ijedne
  instance; `static` member funkcija NEMA `this`, ne može pristupiti
  non-static članovima
- gotcha: `static` član MORA imati definiciju van klase (pre C++17) da ne
  bude linker error — `inline static` (C++17+) to rešava

## API korišćen u vežbi

- `explicit` na jednoargumentnom konstruktoru — sprečava kompajler da ga
  koristi za IMPLICITNU konverziju; bez `explicit`, `Age a = 5;` bi tiho
  pozvalo `Age(5)` (implicitna konverzija int -> Age), što često nije
  namera. Sa `explicit` moraš `Age a(5);` ili `Age a{5};`
- `static` član + definicija van klase (`int InstanceCounter::count_ = 0;`)
  — pre C++17, `static` član klase deklarisan UNUTAR klase je samo
  deklaracija; mora imati TAČNO JEDNU definiciju negde (obično u `.cpp`
  fajlu) ili linker javlja grešku "undefined reference" (C++17 uvodi
  `inline static` koje to rešava bez posebne definicije)

## Zapažanja posle vežbe
