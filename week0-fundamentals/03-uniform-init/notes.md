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

## Formalna podela: koja inicijalizacija je koja

Sve što si video gore (narrowing, most vexing parse, initializer_list
preferencija) su POSLEDICE toga koju "kategoriju" inicijalizacije koristiš.
Standard ih imenuje ovako:

| Kategorija | Sintaksa | Primer |
|---|---|---|
| **default-initialization** | `T t;` (bez initializer-a) | `int x;` (nedefinisana vrednost!), `Widget w;` (zove default ctor) |
| **value-initialization** | `T t{};` | `Widget w2{};` — za klase zove default ctor, za primitivne tipove zero-inicijalizuje (`int x{};` -> 0) |
| **direct-initialization** | `T t(args);` ili `T t{args};` | `Widget w3(5);`, `Widget w4{5};` — konstruktor se poziva DIREKTNO, `explicit` ctor-i SE razmatraju |
| **copy-initialization** | `T t = args;` (uključuje i prosleđivanje po vrednosti, `return`, `catch` po vrednosti) | `int narrow_ok(3.14)` je zapravo DIRECT (zagrade!) — pravi copy-init primer bio bi `int x = 3.14;` — `explicit` ctor-i se NE razmatraju |
| **list-initialization** | `{}` sintaksa, deli se na *direct-list-init* (`T t{args};`) i *copy-list-init* (`T t = {args};`) | oba prolaze kroz narrowing proveru; initializer_list ctor ima prioritet u oba |
| **aggregate initialization** | `T t{a, b};` za agregate (nema user-deklarisan ctor, nema private/protected non-static članove, nema virtual funkcije) | `PodPoint p{1, 2};` iz sesije 10 — direktno puni članove redosledom deklaracije, nema ctor poziva uopšte |

**Zašto je bitna razlika direct vs copy kod `explicit`:** `explicit` na
konstruktoru ga isključuje iz razmatranja BAŠ kod copy-initialization
(i copy-list-initialization) — ne kod direct-initialization. Zato
`Explicit e(5);` radi a `Explicit e = 5;` ne radi, iako oba "izgledaju"
kao da prave isti objekat. Vidi primer u main.cpp.

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
