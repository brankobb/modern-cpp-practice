# Sesija 2 — Kopiranje (C++03)

Izvori: Effective C++ (3. izd.) stavke 5–14, 29; learncpp.com (kopiranje)

- šta kompajler generiše sam od sebe (default ctor, copy ctor, copy assignment,
  dtor) i pod kojim uslovima to NE generiše
- shallow copy (kopira pokazivač) vs deep copy (kopira ono na šta pokazivač
  pokazuje) — zašto je shallow copy opasna kad klasa poseduje resurs
- self-assignment (`a = a;`) — zašto naivna implementacija puca
- rule of 3: ako pišeš jedno od {destructor, copy ctor, copy assignment},
  verovatno ti trebaju sva tri

## Zapažanja posle vežbe
