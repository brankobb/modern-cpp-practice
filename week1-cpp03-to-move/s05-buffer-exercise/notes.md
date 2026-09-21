# Sesija 5 — Vežba: Buffer

Cilj: klasa `Buffer` koja poseduje dinamički niz `int`.

## Korak 1 — Rule of 3 + copy-and-swap
- ctor(size), destructor, copy ctor, copy assignment (copy-and-swap kao u s03)
- proveri pod ASan-om: nema leak-a, nema double-free, self-assignment radi

## Korak 2 — Rule of 5 (dodaj move)
- move ctor, move assignment (kao u s04), oba `noexcept`
- proveri da se move zaista poziva (std::cout u svakom specijalnom članu)
  umesto copy kad je to očekivano (npr. `std::vector<Buffer>` realokacija,
  `return` lokalnog Buffer-a)

## Zapažanja posle vežbe
