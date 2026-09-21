# Sesija 6 — Pravila generisanja

Izvori: Effective Modern C++ st. 11, 14, 17, 29

- kada kompajler generiše default ctor / copy ctor / copy assignment /
  move ctor / move assignment / dtor, a kada ih briše (npr. deklarišeš
  destruktor -> move se ne generiše)
- `= default` vs `= delete` — kad ih eksplicitno pišeš i zašto
  (npr. onemogući copy, ali dozvoli move)
- `noexcept` na move ctor-u — zašto `std::vector` bira copy umesto move
  pri realokaciji ako move ctor NIJE noexcept (strong exception safety
  garancija za vector)

## Zapažanja posle vežbe
