# Sesija 23 — `std::string_view` i `std::filesystem` (kurs 231–236)

Dve C++17 biblioteke za svakodnevni rad sa tekstom i fajlovima:

- **`std::string_view`**: pogled na znakove koje poseduje neko drugi --
  pokazivač + dužina, bez kopiranja i bez alokacije. Brz, ali ne čuva
  ništa: važi dok važi original.
- **`std::filesystem`**: putanje (`path`), informacije o fajlovima
  (`directory_entry`, `status`), pravljenje, kopiranje, brisanje i
  obilazak direktorijuma, dozvole -- prenosivo, umesto POSIX/Win32 poziva.

Već obrađeno, ovde samo upućujemo:

- `std::string`, SSO (kratak string bez heap-a): lekcija 15;
- `string_view` ne poseduje podatke, privremeni string (prvi susret):
  s09, sekcija 5 -- ovde isto, šire;
- iterator/pokazivač koji visi posle realokacije: lekcija 04, sekcija 12;
  lekcija 17;
- `std::optional` (za `uBroj` u zadatku z1): s22.

**Izvori:** standard, `[string.view]`, `[charconv]` (`from_chars`),
`[filesystem]` (`[fs.class.path]`, `[fs.class.directory.entry]`,
`[fs.op.funcs]`, `[fs.enum.perms]`, `[fs.err.report]` -- izuzetak ili
`error_code`). Core Guidelines **SL.str.2** (`string_view` za "pogled"
na znakove), **SL.str.3** (C string -- `const char*` sa `'\0'` -- je
posebna stvar). *C++17 -- The Complete Guide* (Josuttis).

**Kako vežbati:**

```
./build.sh week3-advanced/s23-string-view-filesystem/main.cpp      # svi ISPRAVNI slučajevi
./check_cases.sh week3-advanced/s23-string-view-filesystem         # svi POGREŠNI slučajevi
./check_exercises.sh week3-advanced/s23-string-view-filesystem     # vežbe
```

- `errors/` (e01–e05): kod koji se **ne kompajlira**.
- `ub/` (u01–u02): `string_view` koji nadživi string ili njegov bafer.
- `runtime/` (r01–r02): `filesystem_error`, `substr` van opsega, bez `try`.
- `main.cpp` i zadatak z1 rade u privremenom direktorijumu
  (`temp_directory_path()`) i brišu ga na kraju. Na Windows-u su
  putanje sa `\`, `path::string_type` je `std::wstring`, a od dozvola
  postoji samo "samo za čitanje" -- sekcija 7 tamo izgleda drugačije
  (nije provereno na Windows-u).

---

# 1. `string_view`: pogled na tuđe znakove (kurs 231)

```cpp
std::string_view a = "pritisak";          // literal
std::string_view b = s;                   // std::string -- implicitno, jeftino
std::string_view c(niz, 5);               // pokazivač + dužina (niz bez '\0')
auto d = "napon"sv;                       // using namespace std::string_view_literals
```

- `sizeof(string_view)` je 16: pokazivač + dužina (test). Kopira se po
  vrednosti, kao `int`.
- Ima skoro sve metode `std::string`-a koje samo čitaju: `size`, `[]`,
  `find`, `substr`, poređenja. `substr` je O(1) i ne kopira: rezultat
  gleda u istu memoriju (test: `t.data() == s.data()`).
- `remove_prefix(n)` / `remove_suffix(n)` smanjuju **pogled**, ne menjaju
  original (test: skidanje razmaka sa obe strane).
- ❌ Samo za čitanje (`errors/e02`). ❌ U `std::string` samo eksplicitno --
  `std::string s(sv)` -- jer je to alokacija (`errors/e01`). ❌ C++17 nema
  `std::string + string_view` (`errors/e04`); `r += sv` radi.
- ⚠️ `substr(pos)` baca `out_of_range` za `pos > size()` (`runtime/r02`),
  ali `[]` i `remove_prefix` ne proveravaju ništa.

---

# 2. `string_view` kao parametar (kurs 231)

```cpp
std::size_t duzinaStr(const std::string& s);   // literal -> privremeni std::string
std::size_t duzinaSv(std::string_view s);      // literal -> samo pokazivač + dužina
```

- Test (libstdc++, literal od 32 znaka, duži od SSO bafera od 15):
  `const std::string&` → 1 alokacija, `string_view` → 0. `substr` od 20
  znakova: `std::string` → 1 alokacija, `string_view` → 0.
- ✅ Parametar koji samo čita tekst: `std::string_view`, po vrednosti
  (SL.str.2). Prima `std::string`, literal i `const char*` bez kopije.
- ✅ Sečenje bez alokacija: `podeli("temp=21;vlaga=40;...", ';')` vrati
  poglede u original (test: jedine alokacije su za sam vektor).
- ⚠️ Kad funkcija ipak mora da sačuva tekst (član, ključ u mapi), treba
  joj `std::string` -- tada `const std::string&` ili `std::string` po
  vrednosti + `std::move` (week2 s07) izbegavaju dvostruku kopiju.

---

# 3. `string_view` nije string: kraj i životni vek (kurs 232)

**Nema `'\0'` na kraju pogleda.** `data()` je samo pokazivač na prvi znak.

- Test: `substr(0, 4)` od `"temperatura"` je `temp`, ali `data()`
  ispisan kao C string daje `temperatura` -- C kod čita do `'\0'`
  originala. ❌ Nema `c_str()` (`errors/e03`). ✅ Za C API:
  `std::string(sv).c_str()`, ili funkcija koja prima i dužinu
  (`printf("%.*s", ...)`, `fwrite`) (zadatak z3; SL.str.3).

**Pogled ne produžava život originala.**

- ❌ Pogled na privremeni string: `std::string_view sv = std::string("...");`
  visi odmah posle tog iskaza (`ub/u01`, ASan: `heap-use-after-free`;
  za kratak string u SSO baferu na steku: `stack-use-after-scope`,
  provereno).
- ❌ Original se promenio: `s += ...` preko kapaciteta realocira bafer, a
  pogled gleda u stari (`ub/u02`) -- ista greška kao pokazivač u `vector`
  posle `push_back`.
- ❌ `string_view` kao **član** koji čuva ime iz lokalnog stringa, ili
  kao povratna vrednost koja gleda u lokalni string (zadatak z2). Bez
  ASan-a ispis je smeće, bez ikakve poruke.
- ✅ Pravilo: `string_view` za parametre i kratke, lokalne poglede; za
  sve što objekat čuva ili funkcija vraća posle svog kraja --
  `std::string`.

---

# 4. `std::filesystem::path` (kurs 233)

```cpp
namespace fs = std::filesystem;
fs::path p = fs::path("merenja") / "2024" / "hala3.temp.log";
p.parent_path();   // merenja/2024
p.filename();      // hala3.temp.log
p.stem();          // hala3.temp   -- samo POSLEDNJA ekstenzija se skida
p.extension();     // .log
```

- `/` spaja delove sa separatorom platforme; ❌ `+` ne postoji
  (`errors/e05`); `+=` dodaje znakove bez separatora.
- `replace_extension(".csv")`, obilazak delova `for (auto& deo : p)`
  (test: `[merenja] [2024] [hala3.temp.log]`).
- **Leksičke** operacije rade samo nad tekstom, ne gledaju disk:
  `lexically_normal()` skida `.` i `..` (test: `merenja/./2024/../2025/a.log`
  → `merenja/2025/a.log`), `lexically_relative(baza)` (test: `../2025/a.log`).
  (`fs::canonical` i `fs::relative` rade slično, ali gledaju disk i prate
  simboličke linkove; `canonical` traži da putanja postoji.)
- ⚠️ `std::cout << p` ispiše putanju **pod navodnicima** (test:
  `filename "hala3.temp.log"`) -- zbog razmaka u imenima. Bez navodnika:
  `p.string()` ili, prenosivo sa `/` i na Windows-u, `p.generic_string()`.

---

# 5. `directory_entry` i status (kurs 234)

```cpp
fs::directory_entry e(putanja);
e.exists();  e.is_regular_file();  e.is_directory();  e.file_size();
fs::status(putanja).permissions();
```

- `directory_entry` je putanja + podaci o fajlu koje biblioteka sme da
  zapamti (kešira) -- to je ono što `directory_iterator` daje u petlji.
- Test: `a.log` je fajl od 10 B, `arhiva` direktorijum, `nema.txt` ne
  postoji (sve tri provere `false`, bez izuzetka).
- ⚠️ Stanje na disku može da se promeni između provere i upotrebe (drugi
  proces obriše fajl) -- `exists()` pa `file_size()` nije atomično;
  zato postoje verzije sa `error_code`.

---

# 6. Funkcije za direktorijume i fajlove (kurs 235)

| Funkcija | Šta radi |
|---|---|
| `create_directory(p)` / `create_directories(p)` | jedan nivo / svi nivoi koji fale |
| `copy_file(a, b)`, `copy(a, b, opcije)` | kopija fajla / fajla ili stabla |
| `rename(a, b)` | premeštanje ili preimenovanje |
| `remove(p)` / `remove_all(p)` | fajl ili prazan direktorijum / sve, vrati broj obrisanih |
| `directory_iterator(p)` / `recursive_directory_iterator(p)` | sadržaj / sadržaj sa poddirektorijumima |
| `temp_directory_path()`, `current_path()` | privremeni / tekući direktorijum |

- Test: `create_directories`, `copy_file`, `rename`, pa obilazak; `remove_all`
  obriše 4 stavke.
- ⚠️ Redosled iteracije **nije određen** -- zavisi od sistema fajlova.
  `main.cpp` i zadatak z1 sortiraju pre ispisa.
- **Greške** (`[fs.err.report]`): svaka funkcija ima dve verzije:
  - bez `error_code` -- baca `filesystem_error` sa putanjom u `what()`
    (`runtime/r01`; test: `remove` nepraznog direktorijuma, kod
    `directory_not_empty`);
  - sa `std::error_code& ec` -- ne baca, postavi `ec` (test: `file_size`
    nepostojećeg vrati `static_cast<uintmax_t>(-1)`).
  ✅ `error_code` kad je neuspeh očekivan (fajl možda ne postoji);
  izuzetak kad je neuspeh zaista greška.

---

# 7. Dozvole (kurs 236)

```cpp
fs::permissions(f, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read,
                fs::perm_options::replace);                   // rw-r-----
fs::permissions(f, fs::perms::others_read, fs::perm_options::add);      // rw-r--r--
fs::permissions(f, fs::perms::owner_write, fs::perm_options::remove);   // r--r--r--
fs::status(f).permissions();                                  // čitanje
```

- `perms` su POSIX bitovi (vlasnik / grupa / ostali × čitanje / pisanje /
  izvršavanje); `perm_options` kaže da li se bitovi zamenjuju, dodaju ili
  skidaju (test: tri koraka, na kraju oktalno `444`).
- Nove fajlove sistem pravi sa dozvolama umanjenim za `umask` -- zato
  `main.cpp` prvo postavi dozvole sa `replace`, pa tek onda ispisuje.
- ⚠️ Dozvole ne znače uvek zabranu: `root` na Linux-u otvori za pisanje i
  fajl `r--r--r--` (provereno); na Windows-u od svega postoji samo "samo
  za čitanje".

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 231 | std::string_view - I | sekcije 1, 2; `errors/e01`, `e02`, `e04`; `runtime/r02`; zadatak z1 |
| 232 | std::string_view - II | sekcija 3; `errors/e03`; `ub/u01`, `u02`; zadaci z2, z3 |
| 233 | Filesystem - path | sekcija 4; `errors/e05` |
| 234 | Filesystem - directory_entry | sekcija 5; zadatak z1 |
| 235 | Filesystem - Directory Functions | sekcija 6; `runtime/r01`; zadatak z1 |
| 236 | Filesystem - Permissions | sekcija 7 |

---

# Pravilo za praksu

✅ Parametar koji samo čita tekst: `std::string_view` po vrednosti.

⚠️ `string_view` ne poseduje ništa: nikad član koji čuva, nikad povratna
vrednost koja gleda u lokalni string, nikad pogled na privremeni objekat.

⚠️ `data()` nije C string -- za C API napravi `std::string`.

✅ Brojevi iz teksta: `std::from_chars` (bez alokacije, bez izuzetka, bez
locale-a).

✅ Putanje preko `fs::path` i `/`, ne spajanjem stringova; za ispis
`generic_string()`.

✅ Filesystem funkcija sa `error_code` kad je neuspeh očekivan; redosled
iz `directory_iterator`-a nikad ne pretpostavljaj.

**Rezime:** `string_view` je jeftin pogled na tuđi tekst -- sva njegova
opasnost je u tome što ne zna da li original još postoji i što nema
`'\0'`. `std::filesystem` daje prenosive putanje i operacije nad
fajlovima, sa izborom između izuzetaka i `error_code`-a, a stvari koje
zavise od sistema (redosled, dozvole, separator) treba pisati tako da ne
zavise.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_konfig_i_logovi`](exercises/z1_konfig_i_logovi.cpp) | upotreba | parsiranje bez kopija preko string_view i from_chars, pregled log fajlova preko filesystem-a (sekcije 1, 2, 5, 6) | — |
| [`z2_pogled_u_prazno`](exercises/z2_pogled_u_prazno.cpp) | zašto | zašto string_view ne sme da bude član koji "čuva" ime (sekcija 3; ub/u01) | `-DNAIVNO` |
| [`z3_nije_c_string`](exercises/z3_nije_c_string.cpp) | zašto | zašto data() od string_view-a nije C string (sekcija 3; errors/e03) | `-DNAIVNO` |

## Zapažanja posle vežbe
