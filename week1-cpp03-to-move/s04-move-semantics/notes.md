# Sesija 4 — Move semantika

Izvori: Effective Modern C++ st. 23;
Klaus Iglberger, *Back to Basics: Move Semantics* (CppCon 2019)

- lvalue vs rvalue — kako ih prepoznati (da li izraz ima ime/adresu)
- `T&&` (rvalue reference) — vezuje se samo za rvalue (privremene vrednosti)
- `std::move` je SAMO `static_cast<T&&>` — ne pomera ništa samo po sebi,
  samo dozvoljava da se odabere move preklapanje
- move ctor / move assignment — "ukradi" resurs umesto da ga kopiraš
- moved-from stanje — objekat mora ostati validan (destruktibilan), ali
  vrednost je nedefinisana (obično "prazan")

## Zapažanja posle vežbe
