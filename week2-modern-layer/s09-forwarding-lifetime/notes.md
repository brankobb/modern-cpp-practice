# Sesija 9 — Forwarding i lifetime zamke

Izvori: Effective Modern C++ st. 24, 25, 28

- forwarding reference (`T&&` u kontekstu deduction-a, npr. template
  parametar ili `auto&&`) vs prava rvalue reference -- kako ih razlikovati
- `std::forward` -- prosleđuje argument dalje zadržavajući njegovu
  lvalue/rvalue kategoriju
- reference collapsing (`T& &&` -> `T&`, `T&& &&` -> `T&&`, itd.)
- dangling reference na privremenu vrednost (temporary) -- kad se
  privremena vrednost uništi
- `string_view` -- ne poseduje podatke, lako dangluje ako izvorni string
  nestane
- invalidacija iteratora -- koje operacije nad kontejnerom je invalidiraju

## Zapažanja posle vežbe
