# 02 — Debugging: gdb, sanitizeri, assert (kurs 18)

Kurs koristi Visual Studio debugger; ovde je alat **gdb** (Linux, MSYS2).
Koncepti su isti u svakom debuggeru: breakpoint, korak po korak, ispis
promenljive, watchpoint, stek poziva. Uz debugger, najveći deo bagova u
ovom repozitorijumu hvataju **sanitizeri** (ASan, UBSan), pa je druga
polovina lekcije: kako pročitati njihov izveštaj.

**Izvori:** GDB dokumentacija (*Debugging with GDB*: Breakpoints,
Continuing and Stepping, Examining Data), dokumentacija AddressSanitizer i
UndefinedBehaviorSanitizer (clang.llvm.org i gcc.gnu.org, isti runtime),
standard `[assertions]` (`<cassert>`, `NDEBUG`) i `[dcl.pre]`
(`static_assert`). Core Guidelines **P.5** (provera pri kompajliranju ispred
provere pri izvršavanju) i **P.6/P.7** (šta ne može pri kompajliranju,
proveri pri izvršavanju, i to rano).

**Kako vežbati:**

```
./build.sh week0-fundamentals/02-debugging/main.cpp              # program za gdb, bez bagova
./check_cases.sh week0-fundamentals/02-debugging                 # bagovi koje alati hvataju
./check_exercises.sh week0-fundamentals/02-debugging             # vežbe
```

- `ub/` (u01–u03): tri baga sa izveštajima iz sekcija 4 i 5.
- `errors/` (e01–e02): `static_assert` i `-Werror`.
- Brojevi redova u transkriptima ispod se odnose na `main.cpp` ove
  lekcije. Transkripti su stvarni izlaz gdb 15.1 i g++ 13 (adrese
  skraćene u `0x...`).

---

# 1. Debug build

```
g++ -std=c++17 -g -O0 -fno-omit-frame-pointer main.cpp -o dbg
```

| Fleg | Zašto |
|---|---|
| `-g` | debug informacije: imena promenljivih, tipovi, veza mašinski kod → red u izvoru |
| `-O0` | bez optimizacija: svaki red izvora postoji u kodu, redom, promenljive su u memoriji |
| `-fno-omit-frame-pointer` | pouzdan stek poziva (`bt`, i stekovi u ASan izveštaju) |

⚠️ Sa `-O2` kod je preuređen: funkcije se ubacuju (inline) u pozivaoce,
redovi se spajaju i menjaju redosled. Test: `break zbir` na `-O2` build-u
javi "(2 locations)", jer je `zbir` i inline-ovan u `prosek`. Debuguj na
`-O0`, a bag koji se javlja samo na `-O2` je gotovo uvek UB (sekcija 6).

`build.sh` briše izvršni fajl posle pokretanja, pa za gdb kompajliraj
ručno, kao gore.

---

# 2. gdb: osnovni tok

| Komanda | Skraćeno | Šta radi |
|---|---|---|
| `break zbir` / `break main.cpp:21` | `b` | breakpoint na funkciji / redu |
| `run` | `r` | pokreni program (do prvog breakpoint-a) |
| `next` | `n` | sledeći red, PREKO poziva funkcija |
| `step` | `s` | sledeći red, ULAZI u pozvanu funkciju |
| `finish` | `fin` | završi tekuću funkciju, ispiši povratnu vrednost |
| `continue` | `c` | nastavi do sledećeg breakpoint-a |
| `print izraz` | `p` | ispiši vrednost (i `p v.size()`, `p *ptr`, `p niz[2]`) |
| `info locals` | `i lo` | sve lokalne promenljive |
| `bt` | | stek poziva (backtrace) |
| `frame N`, `up`, `down` | `f` | pređi na okvir N u steku (pa `p` vidi njegove promenljive) |
| `quit` | `q` | izlaz |

Transkript (`main.cpp` ove lekcije):

```
(gdb) break zbir
Breakpoint 1 at 0x12b9: file main.cpp, line 19.
(gdb) run
Breakpoint 1, zbir (v=std::vector of length 4, capacity 4 = {...}) at main.cpp:19
19	    int s = 0;
(gdb) bt
#0  zbir (v=std::vector of length 4, capacity 4 = {...}) at main.cpp:19
#1  0x... in prosek (v=std::vector of length 4, capacity 4 = {...}) at main.cpp:27
#2  0x... in main () at main.cpp:45
(gdb) next
20	    for (std::size_t i = 0; i < v.size(); ++i) {
(gdb) next
21	        s += v[i];
(gdb) print s
$1 = 0
(gdb) print v
$2 = std::vector of length 4, capacity 4 = {10, 20, 30, 40}
(gdb) print v.size()
$3 = 4
(gdb) finish
0x... in prosek (v=std::vector of length 4, capacity 4 = {...}) at main.cpp:27
27	    int s = zbir(v);
Value returned is $4 = 100
```

- `bt` se čita odozgo: #0 je gde si SADA, ispod su pozivaoci.
- gdb ispisuje `std::vector`, `std::string` i ostale STL tipove čitljivo
  ("pretty printers"), i sme da pozove njihove metode (`v.size()`).

---

# 3. gdb: uslovni breakpoint i watchpoint

Kad bag nastaje tek u N-toj iteraciji, ne kucaš `next` N puta:

```
(gdb) break 21 if i == 2
Breakpoint 1 at 0x12ca: file main.cpp, line 21.
(gdb) run
Breakpoint 1, zbir (v=std::vector of length 4, capacity 4 = {...}) at main.cpp:21
21	        s += v[i];
(gdb) info locals
i = 2
s = 30
(gdb) watch s
Hardware watchpoint 2: s
(gdb) continue
Hardware watchpoint 2: s

Old value = 30
New value = 60
zbir (v=std::vector of length 4, capacity 4 = {...}) at main.cpp:20
20	    for (std::size_t i = 0; i < v.size(); ++i) {
```

- ✅ **Watchpoint** (`watch s`) zaustavi program kad god se `s` PROMENI i
  pokaže staru i novu vrednost. Najbrži način da nađeš "ko mi je
  pokvario ovu promenljivu".
- Watchpoint na lokalnoj promenljivoj važi dok je njen okvir živ; kad
  funkcija završi, gdb ga sam obriše i javi "Watchpoint 2 deleted because
  the program has left the block in which its expression is valid." (test).

---

# 4. Posle pada: stek poziva

Program bez sanitizera padne sa "Segmentation fault". Pokreni ga u gdb-u
(`ub/u03`, build bez sanitizera: `g++ -std=c++17 -g -O0 ub/u03_null_pokazivac.cpp -o u03`):

```
(gdb) run
Program received signal SIGSEGV, Segmentation fault.
0x... in procitaj (s=0x0) at u03_null_pokazivac.cpp:18
18	int procitaj(const Senzor* s) { return s->id; }
(gdb) bt
#0  0x... in procitaj (s=0x0) at u03_null_pokazivac.cpp:18
#1  0x... in main (argc=1) at u03_null_pokazivac.cpp:21
```

`s=0x0` u #0 kaže uzrok (null pokazivač), a #1 ko ga je prosledio. Sa
sanitizerima isti program prijavi UBSan "member access within null
pointer of type 'const struct Senzor'" (`ub/u03`), pa ASan SEGV.

---

# 5. Čitanje ASan izveštaja

Izveštaj za `ub/u01` (off-by-one upis u niz na steku), skraćeno:

```
==PID==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x... at pc 0x... bp 0x... sp 0x...
WRITE of size 4 at 0x... thread T0
    #0 0x... in popuni(int*, int) u01_off_by_one_stek.cpp:14
    #1 0x... in main u01_off_by_one_stek.cpp:19
Address 0x... is located in stack of thread T0 at offset 52 in frame
    #0 0x... in main u01_off_by_one_stek.cpp:17
  This frame has 1 object(s):
    [32, 52) 'niz' (line 18) <== Memory access at offset 52 overflows this variable
SUMMARY: AddressSanitizer: stack-buffer-overflow u01_off_by_one_stek.cpp:14 in popuni(int*, int)
```

Čitaj odozgo:

1. **Vrsta** u prvom redu: `stack-buffer-overflow`, `heap-buffer-overflow`,
   `heap-use-after-free`, `stack-use-after-scope`, `double-free`...
2. **`WRITE`/`READ of size N`**: upis ili čitanje, i koliko bajtova (4 =
   jedan `int`).
3. **Prvi red steka (`#0`)**: tačna linija gde se desilo.
4. **Koja promenljiva**: `[32, 52) 'niz'`. Niz zauzima bajtove 32–51 u
   okviru funkcije, a pristup je na 52: prvi bajt POSLE niza.

Za greške na **heap-u** izveštaj ima **tri steka** (`ub/u02`, skraćeno):

```
==PID==ERROR: AddressSanitizer: heap-use-after-free on address 0x... at pc 0x... bp 0x... sp 0x...
READ of size 4 at 0x... thread T0
    #0 0x... in main u02_referenca_posle_rasta.cpp:17
0x... is located 0 bytes inside of 12-byte region [0x...,0x...)
freed by thread T0 here:
    #0 0x... in operator delete(void*, unsigned long) ...
    #3 0x... in std::_Vector_base<int, std::allocator<int> >::_M_deallocate(int*, unsigned long) ...
    #4 0x... in void std::vector<int, std::allocator<int> >::_M_realloc_insert<int>(...) ...
    #6 0x... in std::vector<int, std::allocator<int> >::push_back(int&&) ...
    #7 0x... in main u02_referenca_posle_rasta.cpp:16
previously allocated by thread T0 here:
    #0 0x... in operator new(unsigned long) ...
    #3 0x... in std::_Vector_base<int, std::allocator<int> >::_M_allocate(unsigned long) ...
```

- ✅ **"freed by"** je obično pravi odgovor: KO je oslobodio memoriju.
  Idi niz taj stek dok ne stigneš do svog koda: #7, red 16 je
  `v.push_back(4)`. Red 17, gde je program pao, je samo mesto gde se
  posledica videla.
- Tabela "Shadow bytes" na dnu je ASan-ova interna evidencija; za
  debagovanje sopstvenog koda retko treba.

---

# 6. UBSan: nastavlja posle greške

```
ub.cpp:3:39: runtime error: signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'
-2147483648
posle
```

- ⚠️ UBSan **podrazumevano prijavi i nastavi** (test: exit code 0, i
  program ispiše i "posle"). Prijava se lako izgubi u ostatku izlaza.
  ASan, za razliku od njega, prekida program.
- ✅ `-fno-sanitize-recover=all` pri kompajliranju: UBSan stane na prvoj
  grešci (test: exit code 1). `check_exercises.sh` gradi sa tim flegom.
- ✅ `UBSAN_OPTIONS=print_stacktrace=1 ./program`: uz poruku i stek poziva
  (bez toga UBSan pokaže samo red).
- ⚠️ UB koji se javlja samo na `-O2` (a na `-O0` "radi") je pravilo, ne
  izuzetak: optimizator koristi pretpostavku da UB ne postoji (lekcija 01,
  sekcija 4).

---

# 7. `assert` i `static_assert`

| | `assert(uslov)` | `static_assert(uslov, "poruka")` |
|---|---|---|
| Kada | pri izvršavanju | pri kompajliranju |
| Neuspeh | poruka + `abort()` | greška kompajlera (`errors/e01`) |
| Release build | nestaje sa `-DNDEBUG` | uvek važi, ništa ne košta |
| Za | pretpostavke o vrednostima | pretpostavke o tipovima i platformi |

Neuspeli `assert` (test):

```
as: as.cpp:2: int main(int, char**): Assertion `n > 0 && "n mora biti pozitivan"' failed.
```

(Zatim shell javi "Aborted", a exit kod je 134 = 128 + SIGABRT.)

- ✅ `assert(uslov && "objašnjenje")`: string literal je uvek tačan, pa ne
  menja uslov, a ispiše se u poruci.
- ❌ **Nikad bočni efekat u `assert`-u**: `assert(inicijalizuj())` sa
  `-DNDEBUG` ne pozove `inicijalizuj()` uopšte (zadatak z2).
- ✅ Za proveru koja mora da ostane i u release-u (ulaz spolja, podaci sa
  mreže), `if` + izuzetak ili kod greške, ne `assert`.
- `-D_GLIBCXX_ASSERTIONS` uključuje `assert`-ove unutar libstdc++ (npr.
  `v[i]` van opsega, lekcija 17, sekcija 7).

---

# 8. Upozorenja su prva linija odbrane

Svaki build u ovom repozitorijumu ide sa `-Wall -Wextra -Wshadow
-pedantic-errors`, i kod je bez ijednog upozorenja. Razlog: veliki deo
UB-a iz lekcija kompajler **vidi** i upozori, ali ga ne odbije (povratak
bez `return`-a, mešanje signed/unsigned, viseća referenca, `return
std::move`...). Upozorenje među 50 drugih niko ne čita.

- ✅ Build bez upozorenja, pa svako novo upozorenje odmah bode oči.
- ✅ `-Werror` (ili bar `-Werror=return-type`) u sopstvenom projektu:
  upozorenje postaje greška (`errors/e02`).
- Pre debuggera: pročitaj upozorenja. Pre upozorenja: pokreni sa
  sanitizerima.

---

# Pravilo za praksu

✅ Redosled kad nešto ne radi: upozorenja → sanitizeri → gdb. Svaki
sledeći korak je sporiji od prethodnog.

✅ Debug build: `-g -O0 -fno-omit-frame-pointer`, i sanitizeri.

✅ U gdb-u: `bt` odmah posle pada; uslovni breakpoint umesto 100 puta
`next`; `watch` za "ko je promenio ovu vrednost".

✅ U ASan izveštaju prvo vrsta, pa `#0`, pa (za heap) "freed by".

⚠️ UBSan podrazumevano nastavlja -- traži `runtime error` u izlazu, ili
gradi sa `-fno-sanitize-recover=all`.

⚠️ `assert` nestaje u release build-u. Bez bočnih efekata u njemu.

**Rezime:** debugger pokazuje šta program radi, sanitizer pokazuje gde je
prekršio pravila jezika, a `static_assert` i upozorenja hvataju deo grešaka
pre nego što program postoji. Najjeftinija greška je ona koju je
kompajler odbio.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_nadji_bag_gdb`](exercises/z1_nadji_bag_gdb.cpp) | upotreba | nađi logički bag gdb-om (sekcije 1, 2, 3) | — |
| [`z2_assert_bocni_efekat`](exercises/z2_assert_bocni_efekat.cpp) | zašto | zašto u assert-u nikad nema bočnog efekta (sekcija 7) | `-DNDEBUG` |
| [`z3_allocated_by`](exercises/z3_allocated_by.cpp) | zašto | zašto se u ASan izveštaju čita i "allocated by" (sekcija 5) | `-DNAIVNO` |

## Zapažanja posle vežbe
