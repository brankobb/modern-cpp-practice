# 17 — `std::vector` i `std::initializer_list`

`std::vector` je podrazumevani kontejner u C++-u: dinamički niz koji sam
upravlja memorijom, sa elementima jedan do drugog. Pojavljivao se u
mnogim lekcijama; ovde je njegov API na jednom mestu, uz
`std::initializer_list`, mehanizam koji stoji iza `std::vector<int> v{1, 2, 3}`.

**Izvori:** standard, delovi `[vector]`, `[sequence.reqmts]`,
`[support.initlist]` i `[dcl.init.list]` (životni vek niza iza liste).
Uz to C++ Core Guidelines **SL.con.1–SL.con.3** i **ES.23**; *Effective
Modern C++* **Item 7** (`{}` i `initializer_list`) i **Item 42**
(`emplace`). Nastavci kursa 92 i 93.

**Kako vežbati:**

```
./build.sh week0-fundamentals/17-vector-initializer-list/main.cpp
./check_cases.sh week0-fundamentals/17-vector-initializer-list
```

- `errors/` (e01–e04): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): kod koji se kompajlira, a ASan/UBSan ga hvata (u01
  tek uz dodatni flag, sekcija 7).

**Vektor u drugim lekcijama:**

| Tema | Gde |
|---|---|
| `vector<int>(3)` vs `vector<int>{3}` | 03, sekcija 11 |
| pokazivač na element i realokacija | 04, `ub/u06` |
| `vector<bool>` (proxy) | 08 |
| 2D: `vector<vector<int>>` vs jedan blok | 10, sekcija 6 |
| realokacija kopira ako move nije `noexcept` | week1 s05, week2 s06 |
| `push_back` vs `emplace_back` | week2 s07 |
| invalidacija iteratora, erase u petlji | week2 s09 |

---

# 1. Pravljenje

| Kod | Rezultat |
|---|---|
| `std::vector<int> v;` | prazan, bez alokacije |
| `std::vector<int> v(3);` | `[0 0 0]` (value-init) |
| `std::vector<int> v(3, 7);` | `[7 7 7]` |
| `std::vector<int> v{3, 7};` | `[3 7]` (initializer_list pobeđuje, lekcija 03) |
| `std::vector<int> v(first, last);` | kopija opsega iteratora |

---

# 2. `size` i `capacity`

- `size()` je broj elemenata, `capacity()` koliko ih stane pre sledeće
  realokacije.
- Kad `push_back` prekorači kapacitet: nova, veća memorija, premeštanje
  svih elemenata, oslobađanje stare. Test (libstdc++): `1 2 4 8 16 32`, pa
  je `push_back` u proseku O(1).
- `reserve(n)` unapred: jedna alokacija umesto više, i iteratori ostaju
  važeći dok se ne pređe `n`.
- `clear()` briše elemente, ali **ne vraća memoriju** (test: capacity
  ostaje 32). `shrink_to_fit()` je samo zahtev (u libstdc++ ga ispunjava,
  test: 0).

---

# 3. `reserve` vs `resize`

| | `size()` posle | Elementi | Napomena |
|---|---|---|---|
| `reserve(5)` | isto kao pre | **ne postoje** | `v[0]` je UB (`ub/u01`) |
| `resize(5)` | 5 | napravljeni podrazumevanim konstruktorom | tip bez njega: ❌ (`errors/e04`) |
| `resize(3, 9)` | 3 | kopije zadate vrednosti | |

Najčešća greška sa vektorom: `reserve` pa pisanje preko `[]`, kao da je
`resize`.

---

# 4. Pristup

| | Provera | Na praznom / van opsega |
|---|---|---|
| `v[i]` | ❌ | UB |
| `v.at(i)` | ✅ | `std::out_of_range` |
| `front()`, `back()` | ❌ | UB na praznom (`ub/u02`) |
| `data()` | — | pokazivač na prvi element; `nullptr` ili bilo šta za prazan |

`data()` daje `T*` na uzastopnu memoriju, za C API-je (`fwrite`, `memcpy`,
bibliotečke funkcije koje traže pokazivač + dužinu).

---

# 5. Izmene

| Operacija | Cena | Napomena |
|---|---|---|
| `push_back`, `emplace_back` | O(1) amortizovano | `emplace_back(args...)` pravi element na mestu |
| `pop_back` | O(1) | UB na praznom |
| `insert(pos, x)`, `erase(pos)` | **O(n)**: pomera sve iza | invalidira iteratore od `pos` |
| `erase(remove_if(...), end())` | O(n) jednom | erase-remove; C++20 `std::erase_if(v, pred)` |
| `clear` | O(n) (destruktori) | kapacitet ostaje |

---

# 6. `std::initializer_list` (kurs 92)

```cpp
int sum(std::initializer_list<int> values);          // sum({1, 2, 3, 4})
Polygon(std::initializer_list<int> sides) : sides_(sides) {}   // Polygon{3, 4, 5}
```

- To je **pogled** na privremeni niz: pokazivač + dužina, kao
  `string_view`. Kopiranje `initializer_list`-a ne kopira elemente.
- Elementi su **`const`**, pa se iz njih ne može pomerati:
  - `std::vector<Tracked>{t, t, t}` napravi 3 **kopije** (test), a
    `reserve` + `emplace_back` nijednu;
  - `std::vector<std::unique_ptr<int>>{...}` se ne kompajlira
    (`errors/e01`); move-only tipovi idu preko `push_back`/`emplace_back`.
- **Životni vek niza:**
  - Kao parametar funkcije: niz živi do kraja poziva. ✅
  - `auto list = {1, 2, 3};`: niz živi koliko i `list`. ✅
  - Kao **član agregata** (`Config c{{8080, 8081}};`): život se produži,
    ispravno (test).
  - Sačuvan u član **kroz konstruktor**: niz nestane posle konstruktora,
    član visi (`ub/u03`), bez upozorenja kompajlera.
  - ✅ Pravilo: `initializer_list` se odmah kopira u pravi kontejner
    (`std::vector`), nikad se ne čuva.
- `{}` konstruktor sa `initializer_list` ima prioritet nad ostalim
  (lekcija 03, sekcija 11; EMC Item 7).

---

# 7. Hvatanje grešaka sa vektorom: šta ASan vidi, a šta ne

| Greška | Običan ASan | Dodatni flag (libstdc++) |
|---|---|---|
| `v[i]` iza `capacity()` | ✅ `heap-buffer-overflow` | |
| `v[i]` iza `size()`, a unutar `capacity()` (npr. posle `reserve`) | ❌ **ne vidi** (test: ispiše `42 size=0`) | `-D_GLIBCXX_SANITIZE_VECTOR`: `container-overflow` |
| bilo koji `v[i]` van `size()`, `front()` na praznom | UBSan ponekad (null) | `-D_GLIBCXX_ASSERTIONS`: assertion odmah |

`-D_GLIBCXX_ASSERTIONS` uključuje jeftine provere u `operator[]`,
`front`, `back`, `pop_back` i drugim funkcijama libstdc++. Vredi ga
uključiti u debug build-ovima (neki distributivni paketi ga uključuju i u
release). `main.cpp` radi bez prijava i sa njim (test).

---

# Mapa na kurs

| Nastavak kursa | Sekcija |
|---|---|
| 92 `std::initializer_list` | 6 (i lekcija 03) |
| 93 Dynamic Array (`std::vector`) | 1–5, 7 |

---

# Pravilo za praksu

✅ `std::vector` kao podrazumevani kontejner (SL.con.2).

✅ `reserve` kad znaš broj elemenata; `resize` kad elementi treba da
postoje.

✅ `at()` kad indeks dolazi spolja; `[]` u petlji do `size()`.

✅ `initializer_list` samo kao parametar; odmah kopirati u vektor.

✅ Debug build sa `-D_GLIBCXX_ASSERTIONS`.

⚠️ `clear()` ne oslobađa memoriju.

⚠️ `{}` sa listom kopira elemente i ne radi za move-only tipove.

**Rezime:** vektor je niz na heap-u koji zna svoju veličinu i sam raste
udvostručavanjem, pa je `push_back` jeftin, a `insert`/`erase` u sredini
skupi. Sve zamke su iste vrste: razlika između alocirane memorije
(`capacity`) i postojećih elemenata (`size`), i pogledi (`initializer_list`,
iteratori, `data()`) koji žive kraće nego što izgleda.

## Zapažanja posle vežbe

