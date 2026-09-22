# 01 — Primitivni tipovi, I/O, funkcije (14–17)

- `sizeof` po platformi nije garantovan (samo minimalni opsezi) — nikad ne pretpostavljaj tačnu veličinu
- integer promotion: `char`/`short` u aritmetici se promovišu u `int` pre operacije
- signed/unsigned poređenje — miks izaziva implicitnu konverziju u unsigned, često pogrešno (npr. `-1 > 0u`)
- integer overflow kod signed je **UB** (undefined behavior), kod unsigned je **definisan wraparound**
- `cin >> x` kad parsiranje ne uspe ostavlja stream u fail state — sledeća čitanja se ignorišu dok ne uradiš `cin.clear()` + `cin.ignore()`
- funkcije: parametri po vrednosti su DEFAULT (kopija); povratna vrednost takođe kopija/move (videćeš RVO kasnije u week1)

## Zapažanja posle vežbe
