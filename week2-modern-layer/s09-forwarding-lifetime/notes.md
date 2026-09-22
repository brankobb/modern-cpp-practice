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

## API korišćen u vežbi

- `T&& arg` u template kontekstu (`template<typename T> void wrapper(T&&
  arg)`) — ovo NIJE obična rvalue referenca, nego "forwarding reference"
  (universal reference); tip `T` se DEDUKUJE tako da može da veže i
  lvalue i rvalue argument (van template konteksta, `T&&` gde je `T`
  KONKRETAN tip je uvek obična rvalue referenca)
- `std::forward<T>(arg)` (header `<utility>`) — prosleđuje `arg` DALJE
  zadržavajući njegovu ORIGINALNU lvalue/rvalue kategoriju (za razliku
  od `std::move` koji UVEK forsira rvalue, bez obzira šta mu prosledis)
- `std::string_view` — "pogled" na string bez vlasništva nad podacima
  (samo pokazivač + dužina, ne kopira karaktere); brz za prosleđivanje,
  ali OPASAN ako izvorni string nestane pre nego što view prestane da
  se koristi (dangling view)

## Zapažanja posle vežbe
