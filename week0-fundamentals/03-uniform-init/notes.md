# 03 — Uniform initialization (19)

- `T x{...}` sintaksa (C++11), koristi se dosledno kasnije u kursu/repo-u
- **narrowing conversion se ODBIJA** sa `{}` a PROLAZI (uz warning) sa `()`
  ili `=` — npr. `int x{3.14};` je greška pri kompajliranju, `int x(3.14);` nije
- **most vexing parse**: `Widget w();` se parsira kao DEKLARACIJA FUNKCIJE
  koja vraća Widget, ne kao default-konstruisan objekat — `Widget w{};` to rešava
- **initializer_list preferencija**: ako klasa ima i običan ctor i
  `ctor(std::initializer_list<T>)`, `{}` sintaksa UVEK preferira
  initializer_list verziju ako postoji, čak i kad to nije ono što želiš
  (klasičan `std::vector<int> v(3, 5)` vs `std::vector<int> v{3, 5}` primer)

## Zapažanja posle vežbe
