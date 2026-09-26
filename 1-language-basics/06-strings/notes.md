# Lekcija 06 — Stringovi: literali, `std::string`, string streams, korisnički literali

Tekst u C++-u postoji u tri oblika: **literal** (niz `char`-ova koji živi
ceo program), **`std::string`** (vlasnik teksta na heap-u ili u samom
objektu) i **`std::string_view`** (pogled na tuđi tekst, lekcija 27). Ova
lekcija pokriva prva dva, plus čitanje i pravljenje teksta preko stream-ova
i sopstvene literale kao `2.5_km`.

**Izvori:** standard, delovi `[lex.string]` (literali i raw stringovi),
`[basic.string]`, `[string.conversions]`, `[charconv]`, `[string.streams]`
i `[over.literal]` (korisnički literali). Uz to C++ Core Guidelines
**SL.str.1–SL.str.5** i **SL.io.1–SL.io.3**. Nastavci kursa 85, 86, 88 i 90.

**Kako vežbati:**

```
./build.sh 1-language-basics/06-strings/main.cpp
./check_cases.sh 1-language-basics/06-strings
```

- `errors/` (e01–e06): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): kod koji se kompajlira, a ASan ga hvata.
- Srodno: lekcija 22 (SSO i move), lekcija 27 (`string_view`), 15
  (`operator>>` za sopstveni tip), 08 (`std::literals` je inline namespace).

---

# 1. String literali

| Literal | Tip | Napomena |
|---|---|---|
| `"abc"` | `const char[4]` | 3 znaka + `'\0'`; živi ceo program; **lvalue** |
| `"a" "b"` | `const char[3]` | susedni literali se spajaju pri kompajliranju |
| `u8"abc"` | C++17: `const char[4]`; **C++20: `const char8_t[4]`** | u C++20 ne ide u `const char*` (`errors/e02`) |
| `u"abc"`, `U"abc"`, `L"abc"` | `char16_t`, `char32_t`, `wchar_t` | za API-je koji to traže (Windows: `wchar_t`) |
| `"abc"s` | `std::string` | sekcija 6 |
| `"abc"sv` | `std::string_view` | sekcija 6 |

- ❌ `"Zdravo, " + "svete"` se ne kompajlira: dva pokazivača se ne sabiraju
  (`errors/e01`). `+` za tekst radi samo kad je bar jedan operand
  `std::string`.
- Escape sekvence: `\n`, `\t`, `\"`, `\\`, `\0`.

---

# 2. Raw string literali (kurs 85)

```cpp
R"(C:\Users\ana\fajl.txt)"       // backslash NIJE escape
R"(\d{3}-\d{4})"                 // regex bez duplih backslash-eva
R"x(poziv f(")") -- sadrži )" )x"   // sopstveni graničnik x
R"(prvi red
drugi red)"                      // novi red je deo teksta
```

- Tekst je sve između `R"graničnik(` i `)graničnik"`. Graničnik je
  prazan ili do 16 znakova po izboru.
- ⚠️ Bez graničnika raw string se završava na **prvom** `)"` u tekstu
  (`errors/e03`), a poruke kompajlera pokazuju na posledicu, ne na
  uzrok.
- Tipična upotreba: putanje na Windows-u, regularni izrazi, JSON/SQL/HTML
  ugrađen u kod.

---

# 3. `std::string` (kurs 86)

| Operacija | Primer | Napomena |
|---|---|---|
| dodavanje | `s += ", svete";`, `s.append(...)`, `s + t` | |
| umetanje / brisanje / zamena | `insert(0, ">> ")`, `erase(pos, n)`, `replace(pos, n, "x")` | |
| deo | `substr(pos, n)` | pravi **novi** string (kopija); za pogled bez kopije `string_view` |
| traženje | `find`, `rfind`, `find_first_of` | rezultat `std::string::npos` kad nema |
| pristup | `s[i]` bez provere, `s.at(i)` baca `std::out_of_range` | `s[s.size()]` je `'\0'`; iznad je UB (`ub/u03`) |
| poređenje | `==`, `<`, `compare` | leksikografski, po **bajtovima** |
| C++20 | `starts_with`, `ends_with` | C++23: `contains` |

⚠️ **`npos`** je najveći `std::size_t`. Poredi se sa `std::string::npos`,
nikad sa `-1` tipa `int` i ne čuva se u `int` (gubi se vrednost ili znak).

⚠️ **`size()` broji bajtove, ne slova.** Izvorni fajl je UTF-8, pa je
`"čaša".size()` jednako 6 (test). Za broj slova, sečenje po slovima i
velika/mala slova van ASCII treba biblioteka za Unicode.

⚠️ **Konstrukcija i dodela nisu simetrične:** `std::string s = 'a';` se ne
kompajlira (`errors/e06`), ali `s = 'a';` i čak `s = 65;` rade (test:
`"A"`, bez upozorenja). Za string od jednog znaka: `std::string(1, 'a')`
ili `std::string{'a'}`.

**`c_str()` / `data()`:** pokazivač na interni bafer, za C API.

- Važi samo dok se string **ne promeni** i dok **postoji**. Posle `+=`
  koje realocira pokazuje na oslobođen bafer (`ub/u01`), a iz
  privremenog stringa visi odmah posle `;` (`ub/u02`, clang upozori sa
  `-Wdangling-gsl`).
- Kratak string (SSO) ne alocira, pa ASan tu grešku ne vidi, iako je po
  standardu isto UB.

**Kapacitet:** `reserve(n)` unapred, kad se string gradi u petlji. Kratki
stringovi (u libstdc++ do 15 bajtova) ne alociraju uopšte (lekcija 22).

---

# 4. Brojevi i tekst

| Funkcija | Greška | Napomena |
|---|---|---|
| `std::to_string(x)` | — | `to_string(1.5)` daje `"1.500000"` (format `%f`) |
| `std::stoi`, `stol`, `stod` | **izuzetak** `invalid_argument` / `out_of_range` | ⚠️ čita dok može: `stoi("42abc")` daje 42, ostatak tiho ignoriše (proveri `pos`) |
| `std::from_chars` (C++17) | vraća `std::errc` u rezultatu | bez izuzetaka, bez alokacije, ne zavisi od locale-a; najbrže |
| `std::to_chars` (C++17) | isto | obrnut smer |
| `std::ostringstream` | — | sekcija 5, za formatiranje |

✅ Ulaz od korisnika ili iz fajla: `from_chars` i provera da je pročitan
**ceo** tekst (`ptr == kraj`).

---

# 5. String streams (kurs 88)

```cpp
std::ostringstream out;                   // pravljenje teksta
out << "cena=" << std::fixed << std::setprecision(2) << 3.14159;
out.str();                                // "cena=3.14"

std::istringstream in("10 20 x 30");      // čitanje iz teksta
in >> a >> b >> c;                        // c = 0, in.fail() == true: "x" nije broj

for (std::string field; std::getline(csv, field, ',');) { ... }   // deljenje po graničniku
```

- Isti operatori kao za `std::cin`/`std::cout`, pa rade i za sopstvene
  tipove sa `operator<<`/`>>` (lekcija 15).
- Kad čitanje ne uspe, promenljiva dobija **0** (od C++11) i stream
  ulazi u **fail** stanje. Sva sledeća čitanja ne rade ništa dok se ne
  pozove `clear()`.
- ⚠️ **Ponovna upotreba:** `ss.str("novi tekst")` menja sadržaj, ali
  **ne briše** fail/eof stanje. Test: bez `clear()` čitanje ne uspe (`y=0`),
  sa `clear()` uspe (`y=7`). Najčistije je napraviti nov stream.
- ✅ Za format bez stream-ova: C++20 `std::format`. Test (g++ 13 i clang
  18 sa libstdc++ 13, `-std=c++20`): `std::format("cena={:.2f} kolicina={:4}", 3.14159, 7)`
  daje isto što i `ostringstream` gore. U C++17 ne postoji.

---

# 6. Korisnički literali (kurs 90)

**Standardni:** traže `using namespace` (`errors/e04`), jer su u
`std::literals::*`:

| Literal | Tip | `using namespace` |
|---|---|---|
| `"tekst"s` | `std::string` | `std::string_literals` |
| `"tekst"sv` | `std::string_view` | `std::string_view_literals` |
| `1500ms`, `2s`, `5min`, `1h` | `std::chrono::duration` | `std::chrono_literals` |

- `"a\0b"s.size()` je **3**, a `std::string("a\0b").size()` je **1**:
  konstruktor iz `const char*` staje na prvi `'\0'`, a literal zna svoju
  dužinu (test).

**Sopstveni:**

```cpp
constexpr Meters operator""_km(long double v) { return Meters{v * 1000}; }       // 2.5_km
constexpr Meters operator""_km(unsigned long long v) { return Meters{v * 1000.0L}; } // 3_km
```

- Sufiks **mora** da počne sa `_`. Sufiksi bez `_` su rezervisani za
  standard: g++ upozori (`-Wliteral-suffix`), a clang sa
  `-pedantic-errors` odbije (test).
- Dozvoljeni parametri su tačno propisani: `unsigned long long` za cele
  brojeve, `long double` za decimalne, `(const char*, std::size_t)` za
  stringove. `double` nije dozvoljen (`errors/e05`).
- `constexpr` literal se računa pri kompajliranju (lekcija 12).
- Svrha: jedinice u tipu (`2.5_km` + `400.0_m` ne može slučajno da se
  sabere sa sekundama), kao što `std::chrono` radi za vreme.

---

# Mapa na kurs

| Nastavak kursa | Sekcija |
|---|---|
| 85 Raw Strings | 2 |
| 86 `std::string` | 3, 4 |
| 88 String Streams | 5 |
| 90 User-Defined Literals | 6 |

---

# Pravilo za praksu

✅ `std::string` za tekst koji poseduješ; `std::string_view` kao parametar
funkcije koja samo čita.

✅ Raw string za putanje i regex.

✅ `from_chars` za pouzdano parsiranje brojeva; proveri da je pročitan ceo
ulaz.

✅ `.at(i)` kad indeks dolazi spolja.

⚠️ `c_str()` ne čuvaj: važi do sledeće izmene ili do kraja života stringa.

⚠️ `size()` su bajtovi, ne slova.

⚠️ Stream posle greške: `clear()` pre ponovne upotrebe.

**Rezime:** literal je niz `char`-ova koji živi ceo program, `std::string`
je vlasnik teksta koji zna svoju dužinu i sam upravlja memorijom, a
`string_view` je pogled bez vlasništva. Zamke su iste kao kod vektora:
pokazivač u bafer (`c_str()`, `data()`) važi samo do sledeće izmene.
Stream-ovi i korisnički literali su način da se tekst i vrednosti sa
jedinicama pretvaraju jedni u druge bez ručnog parsiranja.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_parsiranje_konfiguracije`](exercises/ex1_parsiranje_konfiguracije.cpp) | usage | std::string, getline sa graničnikom, string streams i raw string (sekcije 2, 3, 5) | — |
| [`ex2_from_chars`](exercises/ex2_from_chars.cpp) | why | zašto atoi (i stoi bez provere) nije parsiranje (sekcija 4) | `-DNAIVNO` |
| [`ex3_jedinice_literal`](exercises/ex3_jedinice_literal.cpp) | why | zašto jedinice u tipu i korisnički literali (sekcija 6) | `-DNAIVNO` |

## Zapažanja posle vežbe

