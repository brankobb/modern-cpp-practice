# Završna vežba dela 1 — Izveštaj o merenjima

Program čita tekst sa konfiguracijom kanala i merenjima (kao fajl sa
uređaja), proverava svaki red i ispisuje izveštaj: koliko je čega
ispravno, koje su greške i statistiku po kanalu. Nema klasa, pametnih
pokazivača ni izuzetaka -- samo ono što je bilo u lekcijama 01–13.

**Kako raditi:**

```
./build.sh 1-language-basics/capstone/task.cpp          # tvoj kod
./check_exercises.sh 1-language-basics/capstone            # provera: zadatak i rešenje
```

`task.cpp` se kompajlira i nerešen; koraci su u komentaru na vrhu
fajla, a na dnu je blok EXPECTED OUTPUT. Rešenje je u `solution.cpp` --
otvori ga tek kad tvoj izlaz bude isti, pa uporedi pristup, ne samo
rezultat. Očekuj 2–3 sedenja.

---

## Ulaz

```
channel temp min=-20 max=60 unit=C        <- konfiguracija kanala
temp 21.5                                 <- merenje: ime i vrednost
# komentar                                <- preskače se, kao i prazan red
```

- Kanal je ispravan ako ima `min` i `max` i ako je `min < max`; red
  `channel broken min=5` nije, i broji se kao neispravan red konfiguracije.
- Merenje ima najviše jednu grešku, proverenu ovim redom: **loš format**
  (nije tačno dve reči), **nepoznat kanal**, **nije broj** (`abc`, `2x`),
  **van opsega** kanala.

## Šta vežba spaja

| Korak | Šta radiš | Lekcija |
|---|---|---|
| 1 | tekst → broj, "ceo tekst mora biti broj"; red konfiguracije | lekcija 06, sekcije 3 i 5; lekcija 01, sekcija 7 |
| 2 | čitanje red po red, validacija, brojanje grešaka po vrsti | lekcija 04, sekcija 10; lekcija 09, sekcija 3; lekcija 07 |
| 2 | vrste grešaka kao `enum class`, nazivi u `constexpr` tabeli uz `static_assert` | lekcija 05, sekcija 4; lekcija 12, sekcija 6 |
| 3 | statistika po kanalu, range-for sa structured bindings | lekcija 10, sekcija 7 |
| 4 | formatiran izveštaj, overload `column` za broj i tekst | lekcija 01, sekcija 8; lekcija 11, sekcija 1 |
| — | sve u `namespace measurements`; ulaz kao raw string literal | lekcija 08, sekcija 1; lekcija 06, sekcija 2 |

## Na šta da paziš

- ⚠️ `std::stod("2x")` vrati 2 i ćuti; zato provera da posle broja ništa
  nije ostalo (`in >> extra` ne sme da uspe).
- ⚠️ Merenje pamti **indeks** kanala, a ne pokazivač na element vektora
  -- pokazivač bi visio čim vektor kanala naraste (lekcija 04, sekcija 12).
- ⚠️ `static_cast<std::size_t>(error)` kao indeks u tabelu radi samo dok
  tabela prati enum; `static_assert` to proverava pri kompajliranju --
  dodaj novu vrstu greške i vidi šta se desi.
- ✅ Statistika bez posebnih slučajeva za prvo merenje: `st.n == 0 || v < st.min`.

## Posle rešenja

1. Gde bi `std::optional<double>` (lekcija 37) bio bolji od
   `bool readNumber(const std::string&, double&)`?
2. Kako bi izgledao isti program kad bi `Channel` bio klasa sa invarijantom
   `min < max` (deo 2)?
3. Šta se menja ako ulaz dolazi iz pravog fajla (`std::ifstream`)? Koliko
   koda zavisi od toga odakle je `std::istream`?

## Zapažanja posle vežbe
