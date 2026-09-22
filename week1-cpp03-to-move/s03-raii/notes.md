# Sesija 3 — RAII i exception safety

Izvori: Effective C++ (3. izd.) stavke 5–14, 29;
Arthur O'Dwyer, *Back to Basics: RAII and the Rule of Zero* (CppCon 2019)

- stack unwinding pri throw — koji destruktori se pozivaju i kojim redom
- basic / strong / nothrow garancija — razlika i primeri
- copy-and-swap idiom — zašto daje strong garanciju "besplatno"
- zašto destruktor ne sme da baca izuzetak (šta se dešava ako baci tokom
  unwinding-a već aktivnog izuzetka -> std::terminate)

## API korišćen u vežbi

- `std::copy(first, last, dest)` (header `<algorithm>`) — kopira opseg
  `[first, last)` počev od `dest`; ovde kopira stari niz u novo alocirani
- `friend void swap(A&, B&)` deklarisana UNUTAR klase — `friend` funkcija
  ima pristup privatnim članovima, ali NIJE member (poziva se kao
  `swap(a, b)`, ne `a.swap(b)`); ovo je "hidden friend" idiom koji ADL
  (argument-dependent lookup) pronalazi automatski bez `using std::swap;`
- generički `std::swap` (koji bi se pozvao da nismo definisali sopstveni)
  radi 3 kopiranja (temp = a; a = b; b = temp); naš `swap` radi samo
  zamenu pokazivača — O(1) umesto O(n)

## Zapažanja posle vežbe
