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

## API korišćen u vežbi

- RVO / NRVO (Return Value Optimization / Named RVO) — kompajlerska
  optimizacija koja KONSTRUIŠE povratnu vrednost DIREKTNO na mestu
  poziva, bez ijednog copy/move poziva (objekat se nikad ne "premešta",
  jednostavno se od početka gradi tamo gde treba da završi)
- NRVO nije garantovan standardom (zavisi od kompajlera i nivoa
  optimizacije — zato test sa `-O0` i `-O2` daje različite rezultate),
  ali C++17 GARANTUJE eliziju za `return T(...);` napisano DIREKTNO (bez
  imenovane lokalne promenljive) — to više nije "optimizacija", nego
  propisano ponašanje jezika

## Zapažanja posle vežbe
