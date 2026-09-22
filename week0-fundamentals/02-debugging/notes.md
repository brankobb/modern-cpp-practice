# 02 — Debugging (18)

Kurs koristi Visual Studio debugger; ti radiš na Linuxu pa je alat **gdb**
(ili lldb). Isti koncepti, druga komanda.

Osnovne gdb komande koje treba da znaš napamet:
- `break <fajl>:<linija>` ili `break <funkcija>` — breakpoint
- `run` — pokreni pod debuggerom
- `next` (over) / `step` (into) — korak po korak
- `print <var>` / `p <var>` — ispiši vrednost
- `watch <var>` — stani kad se promeni
- `bt` (backtrace) — call stack, KLJUČNO posle segfault-a
- `continue` — nastavi do sledećeg breakpoint-a

## Radni tok za ovu vežbu

1. Kompajliraj SA debug simbolima (build.sh već ima `-g`)
2. `gdb ./run_binary` (build.sh briše binarni posle izvršavanja — za gdb sesiju
   kompajliraj ručno: `g++ -std=c++17 -g -fsanitize=address main.cpp -o dbg`)
3. Nađi bag ispod PRE nego što pogledaš rešenje — koristi `break main`, `next`,
   `print`, i kad puca `bt`

## Zapažanja posle vežbe
