# Sesija 8 — Smart pointeri

Izvori: Effective Modern C++ st. 18–21

- `unique_ptr` — move-only, custom deleter (menja tip!), bez cene u odnosu
  na sirov pokazivač (osim ako deleter nije stateless)
- `shared_ptr` — kontrolni blok (broj referenci + broj weak referenci),
  atomski increment/decrement (cena za thread-safety), dvostruka veličina
  pokazivača
- `weak_ptr` — ne utiče na životni vek, rešava kružne reference, `lock()`
  vraća `shared_ptr` ili prazan ako je objekat već uništen
- `make_unique`/`make_shared` — zašto ih preferirati (jedna alokacija za
  shared_ptr umesto dve, exception safety kod prosleđivanja argumenata)

## Zapažanja posle vežbe
