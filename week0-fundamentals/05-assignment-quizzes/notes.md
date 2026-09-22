# 05 — Assignment kvizovi (21, 23)

U originalnom kursu ovo su bili kratki tekstualni "proveri se" zadaci
ubačeni između video lekcija — pokrivaju ono što je NEPOSREDNO prethodilo
(pointers za #21, reference za #23). Ovde su kao predict-the-output kviz:
pročitaj main.cpp, ZAPIŠI predviđanje za svaki blok PRE pokretanja, pa
uporedi.

## Napomena za Q4

`func(n) + func(n)` — redosled kojim se evaluiraju LEVI i DESNI operand
`+` operatora nije definisan standardom (čak ni u C++17 — ta garancija
pokriva samo neke operatore poput `<<`, `&&`, `||`, `,`, ne obično `+`).
Kompajler SME da evaluira desni operand pre levog. Zato je rezultat
`result` zavistan od redosleda (kompajler-specifično), a `n` na kraju
uvek bude uvećan za 2 bez obzira na redosled — sam bag je u tome što
kod koji zavisi od redosleda evaluacije nije prenosiv/predvidiv.

## Zapažanja posle vežbe (šta si pogrešio i zašto)
