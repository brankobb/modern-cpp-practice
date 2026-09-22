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

## Zapažanja posle vežbe
