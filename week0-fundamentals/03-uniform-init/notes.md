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

## API korišćen u vežbi

- `std::initializer_list<T>` (header `<initializer_list>`) — lagani "proxy"
  objekat koji kompajler automatski pravi za `{a, b, c}` sintaksu; NE
  poseduje podatke (samo pokazivač + veličina na privremeni niz koji
  kompajler kreira) — zato ga nikad ne čuvaj za kasnije, samo koristi
  odmah
- `std::vector<int> v(3, 5)` — poziva ctor `vector(size_type count, const
  T& value)`: 3 elementa, svaki inicijalizovan na 5
- `std::vector<int> v{3, 5}` — poziva ctor `vector(initializer_list<T>)`
  (ako postoji, UVEK ima prioritet nad drugim ctor-ima kod `{}` sintakse)
  — zato ispadne `[3, 5]` (dva elementa), ne `[5, 5, 5]`

## Zapažanja posle vežbe
