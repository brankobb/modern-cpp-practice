# 03 — Uniform initialization

Izvor: **Effective Modern C++, Item 7 — "Distinguish between () and {} when
creating objects."** Ceo main.cpp prati taj item tačku po tačku.

## Tri sintakse

```cpp
int x(0);    // zagrade
int y = 0;   // '='
int z{0};    // vitičaste zagrade ("braced init")
int w = {0}; // '=' + vitičaste (skoro isto kao z)
```

Za proste tipove sve četiri rade isto. Razlike se pojavljuju kod klasa —
otud i ostatak ovog dokumenta.

## Šta standard dozvoljava / ne dozvoljava po sintaksi

| Slučaj | `()` | `=` | `{}` |
|---|---|---|---|
| Default vrednost non-static člana klase | ❌ (parsira se kao deklaracija funkcije) | ✅ | ✅ |
| Inicijalizacija non-copyable objekta (npr. `std::atomic<int>`) | ✅ | ❌ | ✅ |
| Narrowing conversion (npr. `double` → `int`) | ✅ prolazi (uz warning) | ✅ prolazi (uz warning) | ❌ compile error |
| Imun na most vexing parse | ❌ | N/A | ✅ |
| Poziva `initializer_list` ctor ako postoji | ❌ | zavisi | ✅ UVEK ako je konverzija moguća |

## `initializer_list` "otmica" overload resolution-a — glavna poenta Item 7

Ako klasa ima BILO KOJI konstruktor koji prima `std::initializer_list<T>`,
`{}` sintaksa će UVEK pokušati NJEGA prvo — čak i kad postoji "bolji" match
među ostalim konstruktorima, čak i za copy/move konstrukciju. Kompajler
odustaje od `initializer_list` ctor-a SAMO ako je konverzija argumenata u
njegov element-tip potpuno nemoguća (uključujući: ako bi zahtevala
narrowing, program se NE kompajlira — ne prelazi tiho na drugi ctor).

- Prazne vitičaste zagrade `{}` znače "BEZ argumenata" (poziva default
  ctor), NE "prazan initializer_list". Da pozoveš `initializer_list` ctor
  sa STVARNO praznom listom, jedini pouzdan način je `Widget({})`.
  **Pažljivo:** `Widget{{}}` NIJE isto — proverio sam ovo uživo (g++,
  `-std=c++17`) i unutrašnje `{}` se tretira kao JEDAN element liste koji
  se value-inicijalizuje (npr. u `int` postaje 0), pa dobijaš listu sa
  JEDNIM elementom, ne praznu — lako je pogrešno pretpostaviti suprotno.

## Savet za autore klasa

- Ako DODAŠ `initializer_list` ctor postojećoj klasi, klijentski kod koji
  koristi `{}` može odjednom da počne da zove DRUGI konstruktor nego pre —
  tiha promena ponašanja, ne compile error. Testiraj postojeći kod posle
  ovakve izmene.
- Nema konsenzusa "uvek koristi `{}`" ili "uvek koristi `()`" — odaberi
  JEDNU konvenciju kao default i drži je se, koristi drugu samo kad moraš
  (npr. `{}` kad ti treba da izbegneš narrowing, `()` kad radiš sa tipom
  koji ima `initializer_list` ctor a ti hoćeš NEKI DRUGI konstruktor).

## Savet za generički (template) kod

Autor generičke funkcije ne može unapred znati da li pozivalac očekuje
"`()` ponašanje" ili "`{}` ponašanje" za dati tip — zato `std::make_unique`
i `std::make_shared` INTERNO koriste `()`, i to je DOKUMENTOVANA odluka kao
deo njihovog interfejsa, ne slučajnost. Vidi `genericCodeProblem()` u
main.cpp za konkretan primer zašto je ovo bitno.

## Zapažanja posle vežbe
