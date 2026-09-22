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

## Zapažanja posle vežbe
