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

## Kako čitati ASan izveštaj

Kad `build.sh`/`build.ps1` ispiše crveni blok, čitaj odozgo:
1. `SUMMARY: AddressSanitizer: <vrsta greške>` — vrsta problema
   (`stack-buffer-overflow`, `heap-use-after-free`, `SEGV` itd.)
2. `WRITE`/`READ of size N at <adresa>` — da li je upis ili čitanje, i
   koliko bajtova
3. prvi red pod `#0` u stack trace-u — TAČNA linija koda gde se desilo
4. `is located in stack of thread T0 at offset N in frame ... <== Memory
   access at offset N overflows this variable` — koja promenljiva je
   pogođena (ako je stack varijabla)

Ne moraš da razumeš shadow bytes tabelu na dnu — to je ASan-ova interna
implementacija, retko ti treba za debagovanje sopstvenog koda.

## Zapažanja posle vežbe
