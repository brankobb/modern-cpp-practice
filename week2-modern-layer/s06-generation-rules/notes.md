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

## API korišćen u vežbi

- `std::vector::reserve(n)` — unapred alocira kapacitet za `n` elemenata
  BEZ konstruisanja objekata, da izbegneš realokaciju tokom sledećih
  `push_back`/`emplace_back` poziva (u vežbi namerno rezervišemo samo 1
  da IZAZOVEMO realokaciju kasnije)
- `emplace_back()` — konstruiše element DIREKTNO u memoriji vektora
  (prosleđuje argumente ctor-u), za razliku od `push_back(T(...))` koji bi
  napravio privremeni objekat pa ga move/copy-ovao unutra
- `noexcept` specifikator na move ctor-u — obećanje kompajleru/biblioteci
  da funkcija NEĆE baciti izuzetak; `std::vector` ovo proverava pri
  realokaciji da odluči da li da MOVE-uje (brzo, ali ako baci usred
  posla ostavlja vector u nekonzistentnom stanju) ili COPY-uje (sporije,
  ali sigurno ako baci — original ostaje netaknut, strong guarantee)

## Zapažanja posle vežbe
