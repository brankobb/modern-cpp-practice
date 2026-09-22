# 04 — Pointers (20) → Reference (22)

## Pointeri
- pokazivač = adresa + tip; `*p` dereferencira, `&x` uzima adresu
- pointer arithmetic: `p + 1` pomera za `sizeof(T)` bajtova, ne za 1 bajt
- niz "decay-uje" u pokazivač na prvi element kad se prosledi funkciji —
  `sizeof` unutar funkcije više ne daje veličinu niza, daje veličinu pokazivača
- dangling pointer: pokazivač koji pokazuje na već oslobođenu/izašlu iz scope-a
  memoriju — dereferenciranje je UB (ASan će ga uhvatiti)
- `nullptr` (C++11) umesto `NULL`/`0` — tip-sigurno

## Reference
- referenca MORA biti inicijalizovana pri deklaraciji, ne može biti "null",
  ne može se "rebind"-ovati posle (uvek se odnosi na isti objekat)
- ispod haube je često implementirana kao pokazivač, ali jezik je tretira
  kao alias — nema posebne sintakse za dereferenciranje

## Zapažanja posle vežbe
