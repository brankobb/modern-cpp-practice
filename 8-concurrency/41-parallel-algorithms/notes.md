# Lekcija 41 — Paralelni algoritmi (kurs 237 i dalje)

C++17 je većini algoritama iz `<algorithm>` i `<numeric>` (lekcija 36) dodao
verziju sa **politikom izvršavanja** kao prvim argumentom:

```cpp
std::sort(std::execution::par, v.begin(), v.end());   // sme u više niti
```

Nema niti, `mutex`-a ni `future`-a u tvom kodu (lekcije 39, 40) -- biblioteka
deli posao. Ali pravila su stroža nego za običan algoritam: bez
deljenog stanja u lambdi, bez izuzetaka, a `reduce` sme da menja
redosled. I najvažnije praktično pitanje: da li se uopšte izvršava
paralelno zavisi od toga kako je biblioteka napravljena (sekcija 5).

Već obrađeno, ovde samo upućujemo:

- algoritmi, `accumulate`, složenost: lekcija 36;
- data race, zašto je UB, ThreadSanitizer: lekcija 39, sekcija 5;
- `accumulate` sa pogrešnom početnom vrednošću: lekcija 36, zadatak ex2.

**Izvori:** standard, `[execpol]` (politike), `[algorithms.parallel]`
(šta se sme u funkciji koju algoritam poziva; `[algorithms.parallel.exceptions]`:
izuzetak → `terminate`), `[reduce]`, `[transform.reduce]`,
`[inclusive.scan]`, `[exclusive.scan]`. Core Guidelines **CP.2**
(izbegavaj data race), **Per.6** (ne tvrdi ništa o brzini bez merenja). *C++17
-- The Complete Guide* (Josuttis), poglavlja o paralelnim algoritmima.

**Kako vežbati:**

```
./build.sh 8-concurrency/41-parallel-algorithms/main.cpp          # svi ISPRAVNI slučajevi
./check_cases.sh 8-concurrency/41-parallel-algorithms             # svi POGREŠNI slučajevi
./check_exercises.sh 8-concurrency/41-parallel-algorithms         # vežbe
```

- `errors/` (e01–e02): kod koji se **ne kompajlira**.
- `runtime/` (r01): izuzetak iz paralelnog algoritma -- `terminate`, i uz `try`.
- `ub/` nema: ThreadSanitizer uz TBB prijavljuje data race i za **ispravan**
  program (provereno: `sort` + `transform` + `reduce` sa `par`, bez ikakvog
  deljenja) -- `libtbb` nije instrumentisan, pa TSan ne vidi njegovu
  sinhronizaciju. Data race iz sekcije 3 je zato opisan brojevima, ne
  sanitizerom.
- **TBB**: `build.sh` i skripte za proveru same dodaju `-ltbb` kad fajl
  uključuje `<execution>`, a libstdc++ nađe TBB (sekcija 5). Izlaz
  `main.cpp`-a i vežbi je isti sa TBB-om i bez njega (provereno oba
  puta, i skriptama).

---

# 1. Politike izvršavanja (kurs 237)

| Politika | Znači | Od |
|---|---|---|
| `std::execution::seq` | sekvencijalno, u pozivajućoj niti | C++17 |
| `std::execution::par` | sme u više niti | C++17 |
| `std::execution::par_unseq` | više niti **i** vektorske instrukcije (SIMD) u svakoj | C++17 |
| `std::execution::unseq` | samo vektorski, u jednoj niti | C++20 (libstdc++ ga nudi i u C++17 -- provereno) |

- Politika je **dozvola**, ne naredba: `par` kaže "smeš paralelno";
  biblioteka odlučuje (i bez TBB-a ne ume, sekcija 5).
- Isti rezultat za svaku politiku (test: `sort` sa `seq`, `par` i
  `par_unseq` daje isti niz; `transform`, `count_if`, `find` sa `par`).
- Politiku imaju skoro svi algoritmi (`sort`, `transform`, `for_each`,
  `count_if`, `find`, `copy`, `min_element`...). ❌ Nemaju je oni koji su
  po definiciji redom: `accumulate`, `partial_sum`, `iota`
  (`errors/e02`) -- zato postoje `reduce` i `inclusive_scan`.
- ⚠️ Standard traži bar forward iteratore za paralelne verzije;
  libstdc++ prihvata i `istream_iterator` (provereno) -- tada sigurno
  sekvencijalno.

---

# 2. Novi numerički algoritmi (kurs 237)

| Stari (redom) | Novi (sme paralelno) |
|---|---|
| `accumulate(b, e, init)` | `reduce(pol, b, e, init)` |
| `inner_product` | `transform_reduce(pol, b1, e1, b2, init)` |
| — | `transform_reduce(pol, b, e, init, sabiranje, transformacija)` |
| `partial_sum` | `inclusive_scan(pol, b, e, izlaz)` |
| — | `exclusive_scan(pol, b, e, izlaz, init)` -- i-ti bez i-tog |

- Test: `reduce(par)` 1..1000 = 500500; skalarni proizvod `{1,2,3}·{4,5,6}`
  = 32; zbir kvadrata 1..10 = 385; `inclusive_scan` od `3 1 4 1 5` =
  `3 4 8 9 14`, `exclusive_scan` = `0 3 4 8 9`.
- Novi algoritmi postoje i **bez** politike, ali i tada imaju slobodu
  `reduce`-a (sekcija 4).

---

# 3. Deljeno stanje: bez data race-a

❌ Lambda koja menja zajedničku promenljivu je data race -- UB (lekcija 39,
sekcija 5):

```cpp
long sum = 0;
std::for_each(std::execution::par, v.begin(), v.end(), [&](int x) { sum += x; });   // ❌
```

Izmereno (100000 jedinica, `-O1`, TBB, 4 jezgra, 3 pokretanja): 71064,
66887, 77276 -- umesto 100000. Bez TBB-a bi ispalo tačno, jer radi
sekvencijalno (provereno) -- **greška se ne vidi na mašini bez TBB-a**.

✅ Tri ispravna načina (test: sva tri daju tačan zbir):

1. algoritam koji sam skuplja rezultat: `reduce`, `count_if`,
   `transform_reduce`, `min_element`...;
2. `std::atomic<long>` -- ispravno, ali sve niti se bore za istu
   promenljivu (izmereno, 20 000 000 elemenata, `-O2`: `atomic` +
   `for_each(par)` ~400 ms, `reduce(par)` ~4 ms, sekvencijalni
   `accumulate` < 1 ms);
3. svaki element piše samo u **svoje** mesto: `transform` u unapred
   napravljen izlaz.

⚠️ `par_unseq` je još stroži: funkcija ne sme da se sinhronizuje sa
drugima -- nema zaključavanja mutex-a ni `atomic` čekanja (alokacija je
izričito dozvoljena, `[algorithms.parallel.defns]`). Iteracije mogu da
se prepliću unutar jedne niti (SIMD), pa bi ista nit zaključala isti
mutex dvaput. Takav kod je UB bez ikakvog upozorenja.

---

# 4. Pravila: redosled, asocijativnost, izuzeci

- ⚠️ **`reduce` sme da grupiše i premešta** operande: operacija mora biti
  asocijativna i komutativna (`+`, `*`, `min`, `max`). Sa `-`: `accumulate`
  daje 84, `reduce(seq)` 94 (test, libstdc++: grupiše po 4 i sekvencijalno;
  sa TBB-om i `par` ovde 84, bez TBB-a 94 -- zadatak ex2). ✅ Za `-`:
  `budget - reduce(..., plus)`.
- ⚠️ Za `double` ni `+` nije tačno asocijativan: `reduce(par)` može da se
  razlikuje od `accumulate` u poslednjim ciframa, i od pokretanja do
  pokretanja. (Zadatak ex1 bira vrednosti čiji su zbirovi tačni, pa je
  izlaz uvek isti.)
- ❌ Paralelni `for_each` vraća `void`, a ne funktor (`errors/e01`; zadatak
  ex3): svaka nit ima svoju kopiju funktora.
- ❌ **Izuzetak** iz funkcije koju paralelni algoritam poziva → `std::terminate`,
  i kad je poziv u `try` bloku (`runtime/r01`; provereno sa TBB-om i
  bez njega). ✅ Proveri podatke pre algoritma (test: `none_of`), ili
  hvataj unutar lambde i vrati grešku kao podatak.
- Redosled poziva nije određen: nikakav ispis ni zavisnost od "prethodnog"
  elementa u lambdi.

---

# 5. Kad se isplati, i ko zaista radi paralelno

**Biblioteka.** libstdc++ (g++, i clang na Linux-u) koristi **Intel TBB**
(`libtbb-dev`) ako nađe njegova zaglavlja pri kompajliranju:

- TBB instaliran → paralelno, ali **mora `-ltbb`** (inače linker greška,
  provereno); `build.sh` ga sam doda;
- TBB nije instaliran → sve politike rade **sekvencijalno**, bez upozorenja
  (provereno: `for_each(par)` vidi 1 nit; sa TBB-om 4);
- `-D_GLIBCXX_USE_TBB_PAR_BACKEND=0` bira sekvencijalno i kad TBB postoji.

MSVC ima svoju implementaciju bez TBB-a. MSYS2 (Windows): za g++ važi isto
što i gore ako je instaliran TBB paket (ime biblioteke za linker može da
bude drugačije); libc++ (clang64) ima paralelne algoritme tek delimično --
**nije provereno na Windows-u**.

**Merenje** (ova mašina: 4 jezgra, `-O2`, TBB; vreme u ms):

| Posao | `seq` | `par` | bez TBB-a, `par` |
|---|---|---|---|
| `sort` 10 000 000 `double` | ~950 | ~350 (2,7×) | ~950 |
| `transform` (`sqrt · sin`) 10 000 000 | ~225 | ~57 (4×) | ~230 |
| `reduce` 1000 `int`, 10 000 puta | ~1 | ~55 (**50× sporije**) | ~1 |

- ✅ Isplati se za **mnogo** podataka i **skup** posao po elementu.
- ⚠️ Za male nizove paralelizacija je čist trošak: pokretanje zadataka i
  sinhronizacija koštaju više od samog posla (treći red).
- ✅ Per.6: meri pre i posle, na ciljnoj mašini, sa `-O2`.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 237 | Parallel Algorithms - I | sekcije 1, 2; `errors/e02`; zadatak ex1 |
| 237+ | Parallel Algorithms (nastavak) | sekcije 3, 4, 5; `errors/e01`; `runtime/r01`; zadaci ex2, ex3 |

---

# Pravilo za praksu

✅ Politika `par` samo za velike podatke i skup posao po elementu -- i
izmeri.

✅ Rezultat skupljaj algoritmom (`reduce`, `count_if`, `transform_reduce`)
ili pisanjem u svoje mesto; nikad `[&]` + menjanje zajedničke promenljive.

⚠️ `reduce` samo sa asocijativnom i komutativnom operacijom; za `double`
očekuj razliku u poslednjim ciframa.

⚠️ Izuzetak u paralelnom algoritmu je `terminate`: proveri ulaz pre.

⚠️ Proveri da li se zaista izvršava paralelno: bez TBB-a libstdc++ tiho
radi sekvencijalno (a data race se tada ne vidi).

**Rezime:** politika izvršavanja pretvara običan algoritam u paralelan
jednim argumentom, ali prebacuje odgovornost na tebe: funkcija mora da
bude bez deljenog stanja i bez izuzetaka, operacija za `reduce`
asocijativna, a posao dovoljno velik. Da li se išta izvršava paralelno
zavisi od biblioteke -- sa libstdc++ od TBB-a.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVE`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_processing_readings`](exercises/ex1_processing_readings.cpp) | usage | obrada velikog niza merenja paralelnim algoritmima (sekcije 1, 2, 3) | — |
| [`ex2_reduce_is_not_accumulate`](exercises/ex2_reduce_is_not_accumulate.cpp) | why | zašto reduce nije "brži accumulate" (sekcije 2, 4) | `-DNAIVE` |
| [`ex3_for_each_without_state`](exercises/ex3_for_each_without_state.cpp) | why | zašto paralelni for_each ne vraća funktor (sekcija 4; errors/e01) | `-DNAIVE` |

## Zapažanja posle vežbe
