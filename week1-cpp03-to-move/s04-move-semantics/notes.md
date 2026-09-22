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

## API korišćen u vežbi

- `std::move(x)` (header `<utility>`) — NIJE funkcija koja nešto pomera;
  to je efektivno `static_cast<T&&>(x)`, samo GOVORI kompajleru "tretiraj
  ovo kao rvalue", što otvara vrata da se pri overload resolution-u
  izabere move ctor/assignment umesto copy. Sam poziv `std::move` ne
  menja `x` ni na koji način — tek NAREDNI kod (ono što uzme `x` kao
  `T&&`) stvarno "krade" resurs

## Zapažanja posle vežbe
