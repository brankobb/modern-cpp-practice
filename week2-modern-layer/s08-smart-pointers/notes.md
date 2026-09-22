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

## API korišćen u vežbi

- `std::unique_ptr<T, Deleter>` — vlasnički (move-only) pametni pokazivač;
  drugi template parametar (`Deleter`) MENJA TIP pokazivača — zato je
  custom deleter "skup" u smislu tipa (svaki različit deleter = drugi
  `unique_ptr` tip), ne u smislu runtime performansi
- lambda kao deleter (`[](FILE* f){ ... }`) — anonimna funkcija;
  `decltype(file_closer)` uzima NJEN TIP da bi se prosledio kao drugi
  template argument `unique_ptr`-u (lambda tipovi su kompajlerski
  generisani, jedinstveni po lambda izrazu, pa se moraju dedukovati)
- `std::shared_ptr<T>` — deljeno vlasništvo preko REFERENCE COUNTING-a
  (atomski brojač u kontrolnom bloku); poslednji `shared_ptr` koji izađe
  iz scope-a (brojač padne na 0) briše objekat
- `std::make_shared<T>(...)` — pravi kontrolni blok I objekat U JEDNOJ
  alokaciji (za razliku od `shared_ptr<T>(new T(...))` koji radi DVE
  odvojene alokacije) — brže i exception-safe (vidi Item 17 iz
  Effective C++ koji smo ranije pokrili u s08)

## Zapažanja posle vežbe
