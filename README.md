# modern-cpp-practice

Vežbe iz modernog C++-a u 41 lekciji, podeljene u osam delova po temi:
osnove jezika, klase, životni vek i resursi, šabloni, funkcije kao
vrednosti, pametni pokazivači, standardna biblioteka i konkurentnost.
Lekcije su numerisane redom (01–41): svaka se oslanja samo na prethodne,
a "lekcija NN" u tekstu uvek znači folder `NN-...`. Veza sa Udemy kursom je
u tabeli "Mapa na kurs" na kraju svake lekcije.

## Postavljanje (Linux ili WSL)

Sve je provereno na **Ubuntu 24.04**: g++ 13 i clang 18 (libstdc++). Na
Windows-u je najbliže tome WSL sa Ubuntu 24.04 (PowerShell:
`wsl --install -d Ubuntu-24.04`; ako je instalirano više verzija,
`wsl --list --verbose` pa `wsl --set-default Ubuntu-24.04`).

```
sudo apt update
sudo apt install -y g++ clang libclang-rt-18-dev libtbb-dev python3 git
```

- `libclang-rt-18-dev`: ASan/UBSan/TSan za clang (bez njega skripte
  preskaču clang za `ub/` i grade vežbe bez sanitizera);
- `libtbb-dev`: da paralelni algoritmi (lekcija 41) zaista rade paralelno;
- `python3`: za `check_refs.py`.

Repozitorijum kloniraj unutar Linux fajl sistema (`~/...`), ne u
`/mnt/c/...`: Windows git može da pretvori krajeve linija u CRLF, i onda
bash skripte ne rade (a `/mnt/c` je i sporiji). Ubuntu 22.04 ima g++ 11 i
clang 14 -- stariji kompajleri, pa poruke i ponašanje mogu da se razlikuju
od onog što piše u lekcijama.

## Kako je lekcija organizovana

| Fajl / folder | Šta je |
|---|---|
| `notes.md` | objašnjenje po standardu: ✅ ispravno, ❌ greška, ⚠️ zamka; EMC / EC++ / Core Guidelines; na kraju "Pravilo za praksu", tabela vežbi i prazno "Zapažanja posle vežbe" za tvoje beleške |
| `main.cpp` | samo ispravni primeri, sa označenim izlazom; isti izlaz na g++ i clang, C++17 i C++20 |
| `errors/` | kod koji NE SME da se kompajlira (u komentaru: zašto i kako ispravno) |
| `ub/` | kod koji se kompajlira, a ASan/UBSan (ili ThreadSanitizer, za niti) ga hvata pri pokretanju |
| `runtime/` | kod koji se kompajlira, a program se prekine iako nije UB (npr. `std::terminate`) |
| `exercises/` | tri zadatka: bar jedan "upotreba" (vežbaš jezik) i bar jedan "zašto" (prvo vidiš problem, pa ga ispraviš); rešenja u `exercises/solutions/` |
| `<deo>/zavrsna-vezba/` | na kraju svakog dela jedna veća vežba koja spaja sve lekcije tog dela: `notes.md` (zadatak i spisak lekcija), `zadatak.cpp` (kostur koji se kompajlira, sa blokom OČEKIVANI IZLAZ), `resenje.cpp`; delovi 3 i 6 imaju tu vežbu kao lekciju (25 i 33) |

Build i pokretanje jednog fajla preko `./build.sh <fajl.cpp>`: kompajlira sa
AddressSanitizer + UndefinedBehaviorSanitizer i `-pedantic-errors` (kod koji
standard zabranjuje je uvek greška, ne samo warning). Dodatni argumenti idu
kompajleru (`-std=c++20`, `-DNAIVNO`, drugi `.cpp` fajl). Za programe sa
nitima (lekcije 39, 40) `--tsan` gradi sa ThreadSanitizer-om umesto ASan/UBSan
(data race i redosled zaključavanja; TSan i ASan ne mogu u isti build).

```
./build.sh 3-zivotni-vek-i-resursi/19-zivotni-vek-objekta/main.cpp
./build.sh 1-osnove-jezika/03-inicijalizacija/exercises/z2_narrowing_senzor.cpp -DNAIVNO
./build.sh 8-konkurentnost/39-niti/main.cpp --tsan
```

Provere (bash; na Windows-u iz MSYS2 shell-a):

```
./check_cases.sh 1-osnove-jezika/04-pokazivaci-i-reference   # errors/, ub/ i runtime/ jedne lekcije
./check_exercises.sh 1-osnove-jezika/04-pokazivaci-i-reference  # zadaci i rešenja (bez argumenta: sve)
./check_refs.py                                                   # reference između lekcija
```

`check_exercises.sh` gradi svaki zadatak i rešenje sa g++ i clang++, u C++17
i C++20, sa `-Werror`: zadatak mora da se kompajlira i nerešen, izlaz rešenja
mora da bude tačno blok OČEKIVANI IZLAZ iz zadatka, a demonstracije problema
(`-DNAIVNO` i sl.) moraju da pokažu ono što tekst tvrdi.

## Kako raditi zadatak

1. Pročitaj korake u komentaru na vrhu fajla.
2. Zadatak "zašto": prvo pokreni sa makroom iz koraka 1 (npr. `-DNAIVNO`) i
   pogledaj problem (pogrešan izlaz, ASan izveštaj ili grešku kompajlera).
   Objasni sebi ZAŠTO se to desilo, pre nego što pišeš ispravku.
3. Piši kod u `#else` grani / na mestu `TODO`, otkomentariši test u
   `main()` i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
4. Tek onda otvori `exercises/solutions/` i uporedi pristup, ne samo izlaz.

## Realan plan

Procena za lekciju sa svim delovima:

| Deo lekcije | Otprilike |
|---|---|
| `notes.md` + pokretanje `main.cpp` | 1–1.5 h |
| `errors/` i `ub/` (pročitaj, pokreni, objasni) | 0.5–1 h |
| tri zadatka iz `exercises/` | 1.5–2.5 h |

To je 3–5 sati po lekciji, dakle dva sedenja po ~2h. Ukupno 41 lekcija
(lekcije 25 i 33 su same po sebi vežbe) ≈ 80 sedenja; uz 5 sedenja
nedeljno, to je oko 16 nedelja. Predlog:

- tempo, ne datum: lekcija je gotova kad svi zadaci rade i kad umeš da
  objasniš svaki "zašto" zadatak bez gledanja u rešenje;
- ono što već znaš preleti: pročitaj "Pravilo za praksu" i uradi samo "zašto"
  zadatke; ako prođu bez muke, idi dalje;
- posle svakih 4–5 lekcija jedno sedenje za ponavljanje (tvoje beleške iz
  "Zapažanja posle vežbe").

## Windows (MSYS2 clang64) i razlike

Koristi `build.ps1` (isti flegovi). Treba ti MinGW-w64 g++ na
PATH-u — najlakše preko [MSYS2](https://www.msys2.org/)
(`pacman -S mingw-w64-ucrt-x86_64-gcc`, pa dodaj `ucrt64/bin` u PATH) ili
[w64devkit](https://github.com/skeeto/w64devkit).

Ako ti PowerShell odbije da pokrene skriptu ("running scripts is disabled"),
otključaj za tekuću sesiju: `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass`.

Mnogi MSYS2 GCC build-ovi nemaju `libasan`/`libubsan` za mingw target —
ako linker javi `cannot find -lasan`, `build.ps1` to automatski prepozna i
pređe na build BEZ sanitizera (uz upozorenje), pa ništa ne blokira rad.
Za pravi ASan/UBSan na Windows-u instaliraj Clang toolchain umesto GCC-a:
`pacman -S mingw-w64-clang-x86_64-toolchain`, dodaj `clang64/bin` u PATH,
i pokreni sa `.\build.ps1 <fajl> -Compiler clang++`.

```powershell
.\build.ps1 3-zivotni-vek-i-resursi\19-zivotni-vek-objekta\main.cpp
```

Sve u ovom repozitorijumu je provereno na Linux-u: g++ 13 i clang 18, oba
sa **libstdc++** (GNU standardna biblioteka). MSYS2 clang64 koristi
**libc++** (LLVM standardna biblioteka) i linker lld, pa ponešto može da
izgleda drugačije. Šta verovatno izgleda drugačije (ovde NIJE provereno
na Windows-u):

- `-D_GLIBCXX_ASSERTIONS`, `-D_GLIBCXX_SANITIZE_VECTOR` i `-D_GLIBCXX_DEBUG`
  postoje samo u libstdc++. U libc++ ne rade ništa, pa demonstracije koje ih
  koriste (lekcija 07: sekcija 7 i zadaci z2, z3) tamo izgledaju drugačije;
- LeakSanitizer (curenje memorije: lekcija 13 z3, lekcija 19 z2, lekcija 32 z2) na
  Windows-u ASan ne podržava, pa tamo curenje vidiš samo po brojaču živih
  objekata koji zadaci ispisuju;
- ThreadSanitizer (`--tsan`, lekcije 39 i 40) ne postoji za Windows; data race
  primere pokreni u WSL-u;
- paralelni algoritmi (lekcija 41) sa g++ rade paralelno samo uz instaliran TBB,
  i tada traže `-ltbb` (`build.sh` ga dodaje sam, `build.ps1` ne -- dodaj
  ga kao dodatni argument; ime biblioteke u MSYS2 može da se razlikuje);
  bez TBB-a rade sekvencijalno, sa istim izlazom;
- tekstovi poruka (`what()` izuzetaka, imena iz `typeid`, poruke
  kompajlera) i neke nespecifikovane stvari (stanje moved-from stringa,
  koliko puta `vector` raste) mogu da se razlikuju;
- skripte za proveru su bash -- pokreći ih iz MSYS2 shell-a.

**Molba:** kad pokreneš lekciju na Windows-u i vidiš razliku u odnosu na
ono što piše u `notes.md` ili u bloku OČEKIVANI IZLAZ, zapiši je u
"Zapažanja posle vežbe" te lekcije (šta, sa kojim kompajlerom i flegovima)
i javi -- to ide u lekciju kao proverena razlika.

## Deo 1: osnove jezika (`1-osnove-jezika/`)

Tipovi, inicijalizacija, pokazivači i reference, nizovi, stringovi i `vector` (koriste se od početka), `const`, `auto`, funkcije, linkovanje, `constexpr`, dinamička memorija. Fokus na zamkama i pravilima standarda, ne tutorial od nule.

| # | Tema | Šta je unutra | Izvori |
|---|---|---|---|
| [01](1-osnove-jezika/01-tipovi-ulaz-izlaz-funkcije) | Primitivni tipovi, I/O, funkcije | veličine i `<cstdint>`, `char` i bajtovi, promocije i signed/unsigned, prekoračenje (signed UB, unsigned modulo), deljenje, pokretni zarez, stanje stream-a i `>>` pa `getline`, manipulatori, `[[nodiscard]]`, izlazak bez `return`-a, redosled argumenata; pogrešni slučajevi u `errors/` i `ub/` | kurs 14–17, ES.100–106, F.20 |
| [02](1-osnove-jezika/02-debagovanje) | Debugging | debug build, gdb (breakpoint, `next`/`step`/`finish`, uslovni breakpoint, `watch`, `bt` posle pada) sa stvarnim transkriptima, čitanje ASan izveštaja (tri steka, "freed by"/"allocated by"), UBSan nastavlja posle greške, `assert` vs `static_assert` i `NDEBUG`, `-Werror`; bagovi za vežbu u `ub/`, `errors/` | kurs 18 |
| [03](1-osnove-jezika/03-inicijalizacija) | Inicijalizacija | default/value/direct/copy/list init, narrowing, initializer_list prioritet, agregati, C++20 designated; pogrešni slučajevi u `errors/` | kompletno, EMC It. 7 |
| [04](1-osnove-jezika/04-pokazivaci-i-reference) | Pokazivači i reference | `nullptr`, aritmetika, `void*`, `const`, vezivanje referenci i životni vek, prosleđivanje/vraćanje, vlasništvo; pogrešni slučajevi u `errors/` i `ub/` | kompletno, EMC It. 8, EC++ It. 16/20/21 |
| [05](1-osnove-jezika/05-slozeni-tipovi) | Složeni tipovi | podela tipova, C nizovi i `std::array`, `enum` vs `enum class`, `union` i `std::variant`, `using` vs `typedef`, funkcijski tipovi; pogrešni slučajevi u `errors/` i `ub/` | EMC It. 9/10 |
| [06](1-osnove-jezika/06-stringovi) | Stringovi | literali i raw stringovi, `std::string` (npos, bajtovi vs slova, `c_str()` životni vek), brojevi ↔ tekst (`stoi`, `from_chars`), string streams, korisnički literali (`""s`, `ms`, `_km`); pogrešni slučajevi u `errors/` i `ub/` | kurs 85, 86, 88, 90 |
| [07](1-osnove-jezika/07-vector-i-initializer-list) | `std::vector` i `std::initializer_list` | pravljenje, size vs capacity, reserve vs resize, pristup i izmene, `initializer_list` (const elementi, životni vek niza), šta ASan ne vidi i `-D_GLIBCXX_ASSERTIONS`; pogrešni slučajevi u `errors/` i `ub/` | kurs 92, 93 |
| [08](1-osnove-jezika/08-namespace-i-linkovanje) | `namespace`, linkage i `inline` | using-deklaracija vs direktiva, ADL i `swap` idiom, external/internal linkage, ODR i šta sme u header, C++17 `inline` promenljive, redosled inicijalizacije između fajlova; dva `.cpp` fajla (build: `./build.sh .../main.cpp .../util.cpp`); pogrešni slučajevi u `errors/` (i linker greške) i `ub/` | EC++ It. 4/25/30 |
| [09](1-osnove-jezika/09-const) | `const` | pokazivači i `int**` zamka, const member funkcije, `mutable` i thread-safety, const povratna vrednost, `const_cast`, STL, `constexpr`, top-level vs low-level; pogrešni slučajevi u `errors/` i `ub/` | kompletno, EC++ It. 3, EMC It. 13/15/16 |
| [10](1-osnove-jezika/10-auto-i-range-for) | `auto` i dedukcija tipova | tri slučaja template dedukcije, `auto`, `decltype`/`decltype(auto)`, prikaz tipa, `vector<bool>` proxy, range-for i životni vek; pogrešni slučajevi u `errors/` i `ub/` | kompletno, EMC It. 1–6 |
| [11](1-osnove-jezika/11-funkcije-napredno) | Funkcije | overloading i overload resolution, overload po vrsti reference, `= delete`, podrazumevani argumenti, pokazivači na funkcije i `std::function`; pogrešni slučajevi u `errors/` i `ub/` | EMC It. 11/26, EC++ It. 37 |
| [12](1-osnove-jezika/12-constexpr) | `constexpr` | sme vs mora pri kompajliranju, UB u konstantnom izrazu je greška, literal tipovi, tabele u `.rodata`, `if constexpr`, `static_assert`; C++20 `consteval`/`constinit` u `main_cpp20.cpp`; pogrešni slučajevi u `errors/` i `ub/` | kurs 91, EMC It. 15 |
| [13](1-osnove-jezika/13-dinamicka-memorija) | Dinamička memorija | `malloc`/`free` vs `new`/`delete`, neuspela alokacija, `new[]`/`delete[]` i zašto se oblici ne mešaju, 2D nizovi na četiri načina, `make_unique`/`vector`; pogrešni slučajevi u `errors/` i `ub/` | EC++ It. 16, EMC It. 21 |
| [ZV](1-osnove-jezika/zavrsna-vezba) | **Završna vežba:** izveštaj o merenjima | čita konfiguraciju kanala i merenja iz teksta, proverava svaki red (četiri vrste grešaka kao `enum class` sa `constexpr` tabelom naziva), statistika po kanalu, formatiran izveštaj; spaja lekcije 01–13 | — |

## Deo 2: klase (`2-klase/`)

Klase, preopterećenje operatora, nasleđivanje i polimorfizam, konverzije tipova.

| # | Tema | Šta je unutra | Izvori |
|---|---|---|---|
| [14](2-klase/14-klase-osnove) | Klase: osnove | class vs struct i invarijanta, konstruktori i init lista (redosled!), destruktor, NSDMI, `this` i chaining, `static` i `const` članovi, copy konstruktor (plitka vs duboka kopija), delegirajući konstruktori, `= default`/`= delete`; pogrešni slučajevi u `errors/` i `ub/` | EC++ It. 4/5/6/22, EMC It. 11 |
| [15](2-klase/15-preopterecenje-operatora) | Operator overloading | član vs slobodna funkcija, `friend` (hidden friend), `operator=` i dodela samom sebi, poređenje (C++17 i C++20 `<=>`), stream, `[]`, `++`, `()`, `*`/`->`, šta se ne preopterećuje; pogrešni slučajevi u `errors/` i `ub/` | EC++ It. 10/11/23/24 |
| [16](2-klase/16-nasledjivanje-i-polimorfizam) | Nasleđivanje i polimorfizam | pristup i vrste nasleđivanja, redosled konstrukcije, sakrivanje imena, `virtual`/`override`/`final`, virtual destruktor, virtual u konstruktoru, vptr/vtable i devirtualizacija, slicing i `clone()`, dijamant, kompozicija, apstraktne klase i interfejsi; pogrešni slučajevi u `errors/` i `ub/` | EC++ It. 7/9/32/33/38/39 |
| [17](2-klase/17-konverzije-tipova) | Konverzije tipova | implicitne konverzije, `static_cast`/`dynamic_cast`/`const_cast`/`reinterpret_cast` i zašto ne C-cast, strict aliasing i `bit_cast`, konstruktor i operator konverzije (`explicit operator bool`), korisnički → korisnički tip, proverena konverzija brojeva, `typeid` i RTTI; pogrešni slučajevi u `errors/` i `ub/` | EC++ It. 27 |

## Deo 3: životni vek i resursi (`3-zivotni-vek-i-resursi/`)

Izuzeci su prvi, jer sve ostalo u ovom delu računa na njih: životni vek objekta, kopiranje, RAII, move semantika, pravila generisanja specijalnih funkcija, copy elision. Na kraju vežba koja sve spaja (lekcija 25).

| # | Tema | Šta je unutra | Izvori |
|---|---|---|---|
| [18](3-zivotni-vek-i-resursi/18-izuzeci) | Izuzeci | `throw`/`try`/`catch` i hijerarhija `std::exception`; redosled `catch` blokova; sopstveni tip izuzetka; stack unwinding; `throw;` vs `throw e;`; `std::nested_exception`; function-try-block, destruktor ne baca; `noexcept` specifikator i operator; `std::exception_ptr`; `errors/` i `runtime/` (`std::terminate`) | kurs 114–119; EC++ It. 8, 29; EMC It. 14; E.2, E.14–E.16 |
| [19](3-zivotni-vek-i-resursi/19-zivotni-vek-objekta) | Životni vek objekta | trajanje skladišta; redosled u bloku, nizu i klasi; privremeni objekti i produženje života; izuzetak u konstruktoru; `std::exit`; redosled uništavanja static objekata; `ub/` | EC++ It. 4, 13; standard `[basic.life]` |
| [20](3-zivotni-vek-i-resursi/20-kopiranje) | Kopiranje | šta kompajler piše; kada se kopija pravi; kada je obrisana; plitka vs duboka kopija; rule of 3; kopiraj sve delove; rule of 0; `errors/` i `ub/` | EC++ It. 5, 6, 11, 12 |
| [21](3-zivotni-vek-i-resursi/21-raii) | RAII i exception safety | RAII klasa; stack unwinding; resursi u članovima; basic/strong/nothrow sa testom; destruktor ne baca; `unique_ptr` sa deleterom, scope guard; `errors/` i `ub/` | EC++ It. 8, 13, 14, 29 |
| [22](3-zivotni-vek-i-resursi/22-move-semantika) | Move semantika | kategorije vrednosti; vezivanje referenci; `std::move` je cast; move ctor/dodela; imenovana `T&&` je lvalue; move na `const` kopira; moved-from stanje; kad move nije jeftin; `errors/` i `ub/` | kurs 53–55, 58; EMC It. 23, 25, 29 |
| [23](3-zivotni-vek-i-resursi/23-pravila-generisanja) | Pravila generisanja | tabela generisanja proverena kodom; destruktor ukida move; `= default` i `noexcept`; `vector` i `move_if_noexcept`; šablonski konstruktor otima kopiju; move-only tipovi; `errors/` i `ub/` | EMC It. 11, 14, 17, 26 |
| [24](3-zivotni-vek-i-resursi/24-copy-elision) | Copy elision i parametri | RVO/NRVO i pessimizing move sa testom; automatski move na `return` (i razlika g++/clang, P1825); garantovana elizija; izbor oblika parametra; `emplace_back`; `errors/` | kurs 57; EMC It. 25, 41, 42 |
| [25](3-zivotni-vek-i-resursi/25-vezba-bafer) | Vežba `Buffer` | `exercise.cpp` (kostur) i `main.cpp` (rešenje): rule of 3 → 5 → 0; merenje kopija/move-ova; `noexcept` i realokacija `vector`-a; `errors/` i `ub/` | kurs 56; C.20, C.21, C.66 |

## Deo 4: šabloni (`4-sabloni/`)

Funkcijski šabloni, forwarding (odmah posle funkcijskih šablona, jer ga klasni šabloni koriste), klasni šabloni i traits, C++17 novine za šablone.

| # | Tema | Šta je unutra | Izvori |
|---|---|---|---|
| [26](4-sabloni/26-funkcijski-sabloni) | Funkcijski šabloni | instancijacija i zašto šablon ide u header (greška linkera); dedukcija bez konverzija, eksplicitni i podrazumevani argumenti; two-phase lookup; eksplicitna specijalizacija vs overload (zašto overload); ne-tipski parametri i `auto`; `errors/` | kurs 130–137; EMC It. 1; T.144 |
| [27](4-sabloni/27-forwarding-i-zivotni-vek) | Forwarding i lifetime zamke | forwarding vs rvalue referenca; reference collapsing; `std::forward` vs `std::move`; kada forwarding ne radi; `string_view`; invalidacija iteratora; lambda capture; `errors/` i `ub/` | EMC It. 24, 25, 28, 30–32 |
| [28](4-sabloni/28-klasni-sabloni-i-traits) | Klasni šabloni, variadic, traits | savršeno prosleđivanje + variadic; paketi, rekurzija i fold izrazi; klasni šabloni, lenja instancijacija, `typename`, CTAD i deduction guide; potpuna i delimična specijalizacija klase; alias šabloni; type traits i sopstveni trait; `static_assert`; `errors/` | kurs 138–150; EMC It. 9; T.43, T.100, T.150 |
| [29](4-sabloni/29-cpp17-sabloni) | C++17 novine za šablone | CTAD (standardni tipovi, sopstveni deduction guide, agregat u C++17, zamke: kopija umesto kontejnera kontejnera, string literal kao `const char*`); fold izrazi (četiri oblika, prazan paket, zarez, fold nad tipovima); `_v`/`_t` i kako su napravljeni; `if constexpr` (grana po tipu, `else`, rekurzija, `static_assert(false)`); `errors/` | kurs 216–223; *C++ Templates* (2. izd.), *C++17 -- The Complete Guide* |

## Deo 5: funkcije kao vrednosti (`5-funkcije-kao-vrednosti/`)

Lambde, `std::function` i `std::bind`.

| # | Tema | Šta je unutra | Izvori |
|---|---|---|---|
| [30](5-funkcije-kao-vrednosti/30-lambde) | Lambda izrazi | callback: pokazivač na funkciju → funkcijski objekat → lambda; closure tip iznutra (`sizeof`, jedinstven tip, konverzija u pokazivač, `constexpr`); capture po vrednosti/referenci, `[=]`/`[&]`, globalne se ne zarobljavaju; `this`, `[*this]` i zamka `[=]` u metodi; init capture i move-only; generičke lambde, `std::function`, IIFE; `errors/` i `ub/` | kurs 151–159; EMC It. 31, 32, 34; F.50–F.54 |
| [31](5-funkcije-kao-vrednosti/31-function-i-bind) | `std::function` i `std::bind` | `std::function` kao jedan tip za sve callback-ove, prazan i `bad_function_call`, konverzije, metode i `std::invoke`, cena (32 B, heap za veliko stanje); `std::bind` sa placeholder-ima, metode, `std::ref`, `std::mem_fn`; zamke bind-a (kopira, računa odmah, ignoriše višak) i zašto lambda; `errors/`, `ub/`, `runtime/` | kurs 161–165; EMC It. 5, 34; T.49 |

## Deo 6: pametni pokazivači (`6-pametni-pokazivaci/`)

`unique_ptr`, `shared_ptr`, `weak_ptr`, pa vežba: sopstveni `UniquePtr` i RAII na mikrokontroleru.

| # | Tema | Šta je unutra | Izvori |
|---|---|---|---|
| [32](6-pametni-pokazivaci/32-pametni-pokazivaci) | Pametni pokazivači | `unique_ptr` i vlasništvo u parametrima; `shared_ptr` i kontrolni blok; make funkcije (brojanje alokacija); `weak_ptr`; ciklusi; deleteri; nizovi; `enable_shared_from_this`; `errors/` i `ub/` | kurs 72–82; EMC It. 18–22 |
| [33](6-pametni-pokazivaci/33-vezba-uniqueptr-embedded) | Vežba | `exercise.cpp` (kostur) i `main.cpp` (rešenje): sopstveni `UniquePtr`; guard za prekide koji vraća prethodno stanje; `std::lock_guard` sa `SpinLock`; objekat bez heap-a (placement new, `alignas`, `std::launder`); `errors/` i `ub/` | — |

## Deo 7: standardna biblioteka (`7-standardna-biblioteka/`)

Kontejneri, algoritmi, `optional`/`variant`/`any`, `string_view` i `filesystem`.

| # | Tema | Šta je unutra | Izvori |
|---|---|---|---|
| [34](7-standardna-biblioteka/34-sekvencijalni-kontejneri) | Sekvencijalni kontejneri | kontejneri, iteratori i algoritmi; kategorije iteratora; `array`, `vector` (rast, alokacije), `deque` (stabilne reference), `list`/`forward_list` (čvorovi, `splice`); tabele invalidacije i složenosti; izbor kontejnera; `errors/` i `ub/` | kurs 166–171; Effective STL It. 1; SL.con.1, SL.con.2 |
| [35](7-standardna-biblioteka/35-asocijativni-kontejneri) | Asocijativni i neuređeni kontejneri | `set`/`multiset` (insert, bound-ovi, erase svih vs jednog); poredak i ekvivalencija; `map`/`multimap` (`[]` ubacuje, `at`, `find`, `insert_or_assign`, `try_emplace`, `equal_range`); heš tabela, bucket-i, rehash; `std::hash` i sopstveni heš, cena lošeg heša; C++17 `extract`/`merge`; `errors/` i `ub/` | kurs 172–178; Effective STL It. 19, 22, 24 |
| [36](7-standardna-biblioteka/36-algoritmi) | Složenost, algoritmi, izmene kontejnera u C++11 | veliko O izmereno brojanjem poređenja; `find`/`count_if`/`all_of`/`minmax_element`/`accumulate`; `copy_if` + `back_inserter`, `transform`, erase-remove, `unique`; `sort`/`stable_sort`/`partial_sort`/`nth_element`, `lower_bound`/`equal_range`, `merge`, `set_intersection`; C++11/17: `emplace_back`, `cbegin`, `std::size`, `shrink_to_fit`; STL projekat kao vežba; `errors/` i `ub/` | kurs 179–187; Effective STL It. 30, 31, 32, 43 |
| [37](7-standardna-biblioteka/37-optional-variant-any) | `std::optional`, `std::variant`, `std::any` | `optional` (pristup i provera, `emplace`/`in_place`, cena, lenja inicijalizacija, zamka `optional<bool>`); `variant` (`get`/`get_if`/`holds_alternative`, `monostate`, `visit` sa `Preopterecen`, isti povratni tip, mašina stanja, `valueless_by_exception`); `any` (`any_cast`, tačan tip, literal kao `const char*`, alokacija); `errors/`, `ub/`, `runtime/` | kurs 224–230; C.181, F.20; *C++17 -- The Complete Guide* |
| [38](7-standardna-biblioteka/38-string-view-i-filesystem) | `std::string_view` i `std::filesystem` | `string_view` (pogled bez kopije, izmerene alokacije, sečenje, nema `'\0'`, pogled koji visi, `from_chars`); `path` (`/`, delovi, leksičke operacije, navodnici pri ispisu); `directory_entry`, funkcije za direktorijume, izuzetak ili `error_code`, neodređen redosled; dozvole; `errors/`, `ub/`, `runtime/` | kurs 231–236; SL.str.2, SL.str.3; *C++17 -- The Complete Guide* |

## Deo 8: konkurentnost (`8-konkurentnost/`)

Niti i mutex, `async`/`future`/`promise`, paralelni algoritmi.

| # | Tema | Šta je unutra | Izvori |
|---|---|---|---|
| [39](8-konkurentnost/39-niti) | Niti, `std::mutex`, `lock_guard` | `std::thread` (funkcija, lambda, funktor, metoda), `join`/`detach`/`joinable`, move-only; argumenti se kopiraju, `std::ref`; rezultat kroz reference i `join` kao sinhronizacija; data race, `mutex`, `try_lock`; `lock_guard`, `scoped_lock` protiv deadlock-a, `unique_lock`; `this_thread`; `errors/`, `ub/` (ThreadSanitizer), `runtime/` | kurs 189–195; EMC It. 37; CP.2, CP.20–CP.31 |
| [40](8-konkurentnost/40-async-i-future) | `std::async`, `std::future`, `std::promise` | `async` vraća `future`, `get` jednom, `shared_future`; politike `async`/`deferred`/podrazumevana; `wait_for`/`wait_until` i `future_status`; destruktor future-a iz `async`-a čeka (`[[nodiscard]]`); `promise`, `promise<void>` kao signal, broken promise, `packaged_task`; izuzeci kroz `get()` i `set_exception`; `errors/`, `ub/`, `runtime/` | kurs 196–201; EMC It. 35, 36, 38, 39; CP.4, CP.60, CP.61 |
| [41](8-konkurentnost/41-paralelni-algoritmi) | Paralelni algoritmi | politike `seq`/`par`/`par_unseq`/`unseq`; `reduce`, `transform_reduce`, `inclusive_scan`/`exclusive_scan`; deljeno stanje bez data race-a (izmereni izgubljeni zbirovi), `atomic` i njegova cena; `reduce` i asocijativnost, `for_each(par)` vraća `void`, izuzetak → `terminate`; TBB u libstdc++ (`-ltbb`, tihi sekvencijalni rad bez njega), izmereno ubrzanje i usporenje; `errors/`, `runtime/` | kurs 237+; CP.2, Per.6; *C++17 -- The Complete Guide* |

## Provera posle dela 6

Kad završiš lekciju 33 (ne na određeni datum). Odgovori naglas, oko 2 minuta po
pitanju. Zapiši odgovore u [ANSWERS.md](ANSWERS.md) pre nego što ih izgovoriš.

1. Šta kompajler generiše ako deklarišeš samo destruktor?
2. Zašto `std::move` sam po sebi ništa ne pomera?
3. Zašto move konstruktor treba da bude `noexcept`?
4. Kada `return std::move(x)` šteti?
5. `unique_ptr` vs `shared_ptr`: kolika je cena i kada koji?
6. Kako copy-and-swap daje strong garanciju?
7. Kako izgleda RAII bez heap-a u embedded kodu?

Ako sigurno odgovoriš na bar 5 pitanja, prelaziš na STL. U suprotnom, prva dva
sedenja posle toga idu na ponavljanje: "zašto" zadaci iz delova 3 i 6, bez
gledanja u rešenja.
