# 09 — Function Overloading → Default Args → Inline → Function Pointers → Namespace (31–35)

- **overload resolution**: kompajler bira najspecifičniji match; ako je
  dvosmisleno (npr. `int`/`double` overload i pozoveš sa `long`), compile
  error na ambiguity
- **default args + overload**: default argument može napraviti poziv
  dvosmislenim sa drugim overload-om -- kompajler ce to prijaviti kao grešku,
  ne runtime bag
- **inline**: NE garantuje da kompajler stvarno ubaci kod (to je samo hint,
  kompajler odlučuje) -- stvarna svrha danas je da dozvoli definiciju
  funkcije u header fajlu bez ODR (One Definition Rule) kršenja kad se
  header uključi u više .cpp fajlova
- **function pointers**: `RetType (*name)(ArgTypes...)` sintaksa je ružna --
  `using FuncPtr = RetType(*)(ArgTypes...);` je čitljivija; ovo je preteča
  `std::function`/lambdi koje ćeš videti kasnije
- **namespace**: `using namespace X;` na global scope-u u header fajlu
  zagađuje SVAKI fajl koji ga uključi -- izbegavaj; `using X::foo;` (ciljana
  deklaracija) je bezbednija; ADL (argument-dependent lookup) znači da se
  funkcija iz namespace-a argumenta nađe i BEZ `using`

## API korišćen u vežbi

- `using FuncPtr = RetType(*)(ArgTypes...);` — moderni alias (C++11) za tip
  pokazivača na funkciju, čitljiviji od starog
  `typedef RetType (*FuncPtr)(ArgTypes...);`
- `inline` na funkciji — NE garantuje da će kompajler stvarno "ubaciti" kod
  (to kompajler odlučuje sam, na osnovu heuristika); stvarna svrha danas
  je da dozvoli DEFINICIJU funkcije u header fajlu bez kršenja One
  Definition Rule kad se taj header uključi u više `.cpp` fajlova
- ADL (argument-dependent lookup) — kompajler automatski traži funkciju i
  u namespace-u TIPA ARGUMENTA, čak i bez `using` (zato `length(v)` radi
  bez `mymath::` prefiksa ako je `v` tipa `mymath::Vec2` — probaj da
  izbaciš `mymath::` iz poziva u main-u)

## Zapažanja posle vežbe
