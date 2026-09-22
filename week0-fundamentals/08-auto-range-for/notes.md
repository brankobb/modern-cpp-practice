# 08 — Auto (28) → Range-Based For (29–30)

- `auto x = expr;` — DEDUKUJE tip, ali SKIDA top-level `const` i reference
  (`const int& y = ...; auto x = y;` -> `x` je obično `int`, ne `const int&`)
- `auto&` — zadržava referencu (ali ne const, osim ako ne pišeš `const auto&`)
- `const auto&` — najčešći izbor u range-for: bez kopiranja, bez slučajne
  izmene
- `decltype(expr)` — za razliku od `auto`, ZADRŽAVA const/referencu tačno
  kakva je izražena

## Range-based for gotcha (najčešći bag)
```cpp
for (auto x : container) { x.modify(); }        // menja KOPIJU, original nepromenjen!
for (auto& x : container) { x.modify(); }        // menja original
for (const auto& x : container) { /* read */ }   // bez kopije, bez izmene -- default izbor
```

## Zapažanja posle vežbe
