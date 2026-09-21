# Sesija 7 — Copy elision i parametri

Izvori: Effective Modern C++ st. 41

- RVO/NRVO — kompajler direktno konstruiše povratnu vrednost na mestu poziva,
  bez kopiranja/pomeranja
- C++17 garantovana copy elision (za prvostepeni RVO, ne NRVO) — nema poziva
  ctor-a uopšte, ne samo optimizacija
- zašto `return std::move(x);` na lokalnoj promenljivoj ŠTETI — onemogućava
  NRVO (kompajler sad mora da zove move ctor umesto elizije)
- prosleđivanje parametra po vrednosti + move unutra — kad ima smisla
  (jeftin move, poziv se ionako mora dogoditi) naspram overload-a
  (const&, &&)

## Zapažanja posle vežbe
