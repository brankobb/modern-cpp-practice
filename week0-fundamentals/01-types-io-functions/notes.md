# 01 — Primitivni tipovi, ulaz/izlaz, funkcije (kurs 14–17)

Brz pregled onoga što se u osnovama najčešće pogrešno pretpostavi: koliki
je koji tip, šta se dešava sa `char` i `short` u aritmetici, kad je
prekoračenje definisano a kad UB, zašto `-1 < 0u` nije tačno, kako
`std::cin` "zaglavi" posle lošeg unosa i šta funkcija sme, a šta ne sme da
uradi sa povratnom vrednošću.

**Izvori:** standard, delovi `[basic.fundamental]` (tipovi i opsezi),
`[conv.prom]` i `[expr.arith.conv]` (promocije i uobičajene aritmetičke
konverzije), `[expr.pre]` (prekoračenje je UB), `[expr.mul]` i
`[expr.shift]` (deljenje i pomeranje), `[istream.formatted]` (formatiran
ulaz), `[stmt.return]` i `[dcl.attr.nodiscard]` (povratna vrednost). Core
Guidelines **ES.100–ES.106** (aritmetika: ne mešaj signed i unsigned, ne
oslanjaj se na prekoračenje) i **F.20** (vraćaj rezultat, ne izlazni
parametar).

**Kako vežbati:**

```
./build.sh week0-fundamentals/01-types-io-functions/main.cpp      # svi ISPRAVNI slučajevi
./check_cases.sh week0-fundamentals/01-types-io-functions         # svi POGREŠNI slučajevi
./check_exercises.sh week0-fundamentals/01-types-io-functions     # vežbe
```

- `errors/` (e01–e06): kod koji se **ne kompajlira**.
- `ub/` (u01–u06): kod koji se kompajlira, a UBSan ga hvata pri pokretanju.
- Ulaz u `main.cpp` ide kroz `std::istringstream` umesto `std::cin`, da bi
  izlaz bio isti pri svakom pokretanju. `std::cin` je takođe `istream`,
  pa se ponaša isto.

---

# 1. Fundamentalni tipovi i veličine

Standard garantuje samo **minimalne** širine i redosled:

| Tip | Najmanje | Linux x86-64 (test) |
|---|---|---|
| `char` | 8 bita, `sizeof(char) == 1` uvek | 1 |
| `short` | 16 bita | 2 |
| `int` | 16 bita | 4 |
| `long` | 32 bita | 8 |
| `long long` | 64 bita | 8 |
| pokazivač | — | 8 |

- ⚠️ **`long` nije svuda isti.** Linux 64-bit (model LP64) ima 8-bajtni
  `long`, a Windows 64-bit (LLP64) 4-bajtni (ovo drugo ovde nije
  provereno). Kod koji pretpostavi `sizeof(long) == 8` ne radi isto na
  obe platforme.
- ✅ Kad je širina bitna (protokol, registar, fajl format), koristi
  `<cstdint>`: `std::int32_t`, `std::uint8_t`, `std::uint64_t`...
- ✅ Granice tipa: `std::numeric_limits<T>::min()` / `max()` (`<limits>`).
  Radi za svaki tip, i u šablonima, za razliku od makroa `INT_MAX`.

---

# 2. `char` i bajtovi

- ⚠️ Da li je obični `char` signed ili unsigned **zavisi od platforme**
  (`[basic.fundamental]`). Na x86 je signed: `char(200)` kao `int` je
  **-56** (test). Na ARM-u (npr. mikrokontroleri) je obično unsigned, pa
  isti kod daje 200. `char`, `signed char` i `unsigned char` su **tri
  različita tipa**.
- ⚠️ `std::uint8_t` je (na g++ i clang) `unsigned char`, pa ga `std::cout`
  ispiše kao **znak**: `std::uint8_t x = 65; std::cout << x;` daje `A`
  (test). Za broj: `+x` ili `int(x)`.
- ✅ Za bajtove koristi `unsigned char` ili `std::uint8_t` (aritmetika) ili
  `std::byte` (C++17, samo bitske operacije), nikad obični `char`.

---

# 3. Integralne promocije i uobičajene aritmetičke konverzije

**Promocija** (`[conv.prom]`): `bool`, `char`, `short` i njihove unsigned
varijante se u aritmetici prvo pretvore u `int` (ako `int` može da drži
sve njihove vrednosti).

```cpp
unsigned char a = 200, b = 100;
auto s = a + b;                  // int 300, ne unsigned char 44 (test)
```

**Uobičajene aritmetičke konverzije** (`[expr.arith.conv]`): kad se sretnu
dva različita tipa, oba idu u "veći". Između signed i unsigned istog ranga
pobeđuje **unsigned**:

```cpp
-1 < 0u        // false! -1 postane unsigned 4294967295 (test)
-1L < 0u       // true na Linux-u: long (64 bita) može da drži svaki unsigned,
               // pa ide u long. Na Windows-u (long 32 bita) bi išlo u unsigned long.
```

- ❌ Mešanje signed i unsigned u poređenju. g++ `-Wall` i clang `-Wextra`
  upozore (`-Wsign-compare`) kad su operandi promenljive; za `-1 < 0u`
  (dve konstante) clang ne upozori (test). Česta zamka: `int i` protiv
  `v.size()`, ili temperatura protiv unsigned praga (zadatak z2).
- ✅ Oba operanda istog tipa (eksplicitni `static_cast` posle provere da
  vrednost staje), ili C++20 `std::cmp_less(-1, 0u)` (`<utility>`) koji
  poredi matematički tačno.
- ⚠️ Promocija može da **napravi** UB: `unsigned short a = 65535; a * a`
  se računa u `int`, a 65535 · 65535 ne staje u `int`, pa je to
  prekoračenje signed int-a (`ub/u02`), iako su oba operanda unsigned.

---

# 4. Prekoračenje: signed je UB, unsigned je modulo

- ❌ **Signed prekoračenje je UB** (`[expr.pre]`): `INT_MAX + 1` (`ub/u01`).
  Kompajler sme da pretpostavi da se nikad ne dešava, pa npr. uslov
  `x + 1 > x` sme da zameni sa `true`. UBSan ga hvata pri izvršavanju, a
  u konstantnom izrazu je greška pri kompajliranju (lekcija 16).
- ✅ **Unsigned je definisan**: računa se po modulu 2ⁿ. `UINT_MAX + 1u == 0`
  (test). Zato se koristi za heš, CRC i brojače koji "prelaze preko".
- ❌ Pomeranje za **≥ širina tipa** je UB: `1 << 32` za 32-bitni `int`
  (`ub/u03`).
- ✅ Pre operacije proveri granicu, ili koristi širi tip.

---

# 5. Deljenje i ostatak

- `7 / 2 == 3`: celobrojno deljenje odseca razlomljeni deo **ka nuli**, pa
  `-7 / 2 == -3` i `-7 % 2 == -1` (od C++11 garantovano, test).
- ❌ Deljenje nulom je UB (`ub/u04`). Za `int`, UBSan: "division by zero".
- ❌ `INT_MIN / -1` je UB: rezultat (2147483648) ne staje u `int`
  (`ub/u05`).
- ✅ `7.0 / 2 == 3.5`: čim je jedan operand `double`, deljenje je u
  `double`. Cast ide **pre** deljenja: `static_cast<double>(a) / b`
  (lekcija 14, zadatak z1).

---

# 6. Brojevi u pokretnom zarezu

- ⚠️ `0.1 + 0.2 == 0.3` je **false**: `0.1 + 0.2` je
  `0.30000000000000004` (test, 17 cifara). 0.1 nema tačan binarni zapis.
- ✅ Poređenje sa tolerancijom: `std::abs(a - b) < 1e-9` (tolerancija
  zavisi od veličine brojeva i od toga koliko je računanja iza njih).
- ⚠️ `NaN == NaN` je **false** (test). Proverava se sa `std::isnan(x)`.
- ❌ `double` → `int` van opsega je UB (lekcija 14, `ub/u02`).

---

# 7. Ulaz: stanje stream-a

`in >> x` pokuša da pročita formatiran broj. Ako ne uspe:

- postavi **`failbit`**, a `x` dobije **0** (od C++11);
- ⚠️ **sva sledeća čitanja ne rade ništa** dok ne pozoveš `in.clear()`
  (test: posle neuspelog čitanja, `in >> z` ostavi `z` nepromenjen);
- loš tekst **ostaje u baferu**. Posle `clear()` ga preskoči sa
  `in.ignore(std::numeric_limits<std::streamsize>::max(), '\n')` (ili do
  razmaka).

Broj van opsega (`"99999999999"` u `int`) takođe postavi `failbit`, a `x`
dobije **najbližu granicu**: `INT_MAX`, odnosno `INT_MIN` za negativan
(test).

```cpp
if (!(in >> x)) { in.clear(); in.ignore(max, '\n'); }   // ✅ proveri svako čitanje
while (in >> x) { ... }                                  // ✅ čitaj dok uspeva
```

⚠️ **`>>` pa `getline`:** `in >> n` ostavi `'\n'` u baferu, pa sledeći
`std::getline(in, ime)` odmah pročita **prazan** red (test: `ime=[]`).
Rešenje: `std::getline(in >> std::ws, ime)`. `std::ws` preskoči sve
praznine pre čitanja (zadatak z3).

---

# 8. Izlaz: manipulatori

| Manipulator | Traje | Primer |
|---|---|---|
| `std::setw(n)` | **samo sledeći** ispis | `std::setw(5) << 42` → `   42` |
| `std::setprecision(n)`, `std::fixed` | dok ga ne promeniš | `std::fixed << std::setprecision(2) << 3.14159` → `3.14` |
| `std::hex`, `std::dec`, `std::oct` | dok ga ne promeniš | `std::hex << 255` → `ff` |
| `std::boolalpha` | dok ga ne promeniš | `true` umesto `1` |
| `std::setfill(c)` | dok ga ne promeniš | `std::setfill('0') << std::setw(3) << 7` → `007` |

- ⚠️ Većina manipulatora je "lepljiva": posle `std::cout << std::hex`
  **svaki** sledeći `int` se ispisuje heksadecimalno, i u delu koda koji
  za to ne zna. Vrati sa `std::dec` (test u `main.cpp`).
- ✅ `'\n'` umesto `std::endl` kad ne treba odmah isprazniti bafer:
  `std::endl` = `'\n'` + `flush`. `std::cerr` nema bafer.

---

# 9. Funkcije: deklaracija, definicija, povratna vrednost

- Funkcija mora biti **deklarisana pre poziva** (`errors/e03`). Deklaracija
  (prototip) je dovoljna; definicija može kasnije ili u drugom `.cpp`
  fajlu (lekcija 06).
- Parametri se podrazumevano prenose **po vrednosti** (kopija). Reference i
  pokazivači su u lekciji 04, izbor oblika parametra u week2 s07.
- ✅ Vraćaj rezultat kao povratnu vrednost (F.20), ne kroz izlazni
  parametar. Više vrednosti: `struct` ili `std::pair`/`std::tuple` sa
  structured bindings (lekcija 08).
- ❌ `void` funkcija ne vraća vrednost (`errors/e01`), a ne-`void` mora da
  vrati vrednost u `return` (`errors/e02`).
- ❌ **Izlazak sa kraja ne-`void` funkcije bez `return` je UB**
  (`[stmt.return]`, `ub/u06`). Kompajler samo upozori (`-Wreturn-type`, u
  `-Wall`), a UBSan pri izvršavanju: "execution reached the end of a
  value-returning function without returning a value". (Izuzetak je
  `main`: bez `return` vraća 0.)
- ✅ `[[nodiscard]]` (C++17): ignorisanje povratne vrednosti je
  upozorenje (`-Wunused-result`). Za funkcije čiji je rezultat jedini
  signal greške (`errors/e04`, proverava se sa `-Werror`).
- ⚠️ **Redosled računanja argumenata nije određen.** `h(g("A"), g("B"))`
  ispiše `BA` sa g++, a `AB` sa clang (test). Od C++17 to nije UB (svaki
  argument se izračuna ceo pre sledećeg), ali redosled bira kompajler. Za
  `f(i++, i++)` oba kompajlera upozore (g++ `-Wsequence-point`, clang
  `-Wunsequenced`) i daju 21 odnosno 12 (test). Ne piši argumente koji
  zavise jedni od drugih.

---

# Pravilo za praksu

✅ Kad je širina bitna, `<cstdint>` tipovi. Za bajtove `unsigned char` /
`std::uint8_t`, nikad obični `char`.

✅ Ne mešaj signed i unsigned u poređenju i aritmetici (ES.100). Veličine
iz `size()` poredi sa `std::size_t`, ili prvo proveri znak.

✅ Svako čitanje iz stream-a proveri. Posle greške `clear()` + `ignore()`.
Posle `>>`, a pre `getline`, `std::ws`.

✅ `[[nodiscard]]` na funkcijama čiji rezultat ne sme da se ignoriše.

⚠️ Signed prekoračenje, deljenje nulom, `INT_MIN / -1`, pomeranje za ≥
širinu i izlazak sa kraja ne-`void` funkcije su UB. UBSan ih hvata samo
kad se ta linija izvrši.

⚠️ `-Wall` upozorenja o znaku, `return`-u i redosledu nisu kozmetika: svako
od njih je ovde bag.

**Rezime:** standard garantuje samo minimalne veličine tipova, a
aritmetika prvo promoviše male tipove u `int`, pa izjednači tipove, pri
čemu unsigned "pobeđuje" signed. Unsigned prekoračenje je definisano,
signed je UB. Stream posle greške ne radi ništa dok ga ne očistiš.
Funkcija koja ne vrati vrednost, a trebalo je, je UB, ne greška.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_statistika_ulaza`](exercises/z1_statistika_ulaza.cpp) | upotreba | čitanje brojeva sa proverom, struct kao rezultat, formatiran ispis (sekcije 7, 8, 9) | — |
| [`z2_signed_unsigned`](exercises/z2_signed_unsigned.cpp) | zašto | zašto se signed i unsigned ne mešaju u poređenju (sekcija 3) | `-DNAIVNO` |
| [`z3_getline_posle_citanja`](exercises/z3_getline_posle_citanja.cpp) | zašto | zašto getline posle >> pročita prazan red (sekcija 7) | `-DNAIVNO` |

## Zapažanja posle vežbe
