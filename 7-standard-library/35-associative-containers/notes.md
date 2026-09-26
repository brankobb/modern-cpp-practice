# Lekcija 35 — Asocijativni i neuređeni kontejneri (kurs 172–178)

Asocijativni kontejneri čuvaju elemente po **ključu**, ne po poziciji:
traženje po vrednosti je brzo, a ne O(n) kao kod `vector`-a. Postoje dve
porodice:

- **uređeni** (`set`, `multiset`, `map`, `multimap`): balansirano stablo
  (u praksi red-black), elementi sortirani, operacije O(log n), traže
  **poredak** (`operator<`);
- **neuređeni** (`unordered_*`, C++11): heš tabela, redosled nije
  određen, operacije O(1) u proseku, traže **heš** i **jednakost**.

Sekvencijalni kontejneri su u lekciji 34, algoritmi i složenost u lekciji 36.

**Izvori:** standard, `[associative.reqmts]`, `[unord.req]`, `[set]`,
`[multiset]`, `[map]`, `[multimap]`, `[unord.set]`, `[unord.map]`,
`[unord.hash]` (`std::hash`), `[container.node]` (C++17 node handle).
*Effective STL* **Item 19** (ekvivalencija vs jednakost), **Item 22**
(ne menjaj ključ u setu/mapi), **Item 24** (`operator[]` vs `insert` u
mapi). Core Guidelines **SL.con.2**.

**Kako vežbati:**

```
./build.sh 7-standard-library/35-associative-containers/main.cpp        # svi ISPRAVNI slučajevi
./check_cases.sh 7-standard-library/35-associative-containers           # svi POGREŠNI slučajevi
./check_exercises.sh 7-standard-library/35-associative-containers       # vežbe
```

- `errors/` (e01–e04): kod koji se **ne kompajlira**.
- `ub/` (u01–u02): iterator posle `erase`; `u02` se hvata samo debug
  režimom libstdc++ (`-D_GLIBCXX_DEBUG`, u zaglavlju fajla).
- Broj bucket-a u `main.cpp` je za libstdc++; libc++ može da da drugi.

---

# 1. `std::set` i `std::multiset` (kurs 172)

```cpp
std::set<int> s{5, 1, 4, 1, 3};      // 1 3 4 5 -- sortiran, bez duplikata
auto [it, ubaceno] = s.insert(4);    // ubaceno == false, it pokazuje na postojeću 4
s.count(3);  s.find(7) == s.end();   // traženje: O(log n)
s.lower_bound(2);                    // prvi >= 2  -> 3
s.upper_bound(4);                    // prvi >  4  -> 5
```

- `insert` vraća `pair<iterator, bool>`: da li je ubačeno, i gde je
  element (novi ili postojeći).
- `multiset` dozvoljava duplikate.
  - ⚠️ `ms.erase(2)` briše **sve** dvojke (test: obrisao 3);
  - `ms.erase(ms.find(2))` briše **jednu** (test).
- ❌ Element seta je `const`: promena bi pokvarila poredak (`errors/e01`,
  Effective STL Item 22). Menja se brisanjem pa ubacivanjem, ili C++17
  `extract` (sekcija 6).
- C++20: `s.contains(x)`.

---

# 2. Poredak i ekvivalencija

```cpp
std::set<int, std::greater<int>> opadajuce;              // 3 2 1
std::set<std::string, PoDuzini> poDuzini;                // funkcijski objekat
std::set<std::string, decltype(lambda)> imena(lambda);   // lambda: tip + objekat
```

- Drugi argument šablona je poredak (podrazumevano `std::less<T>`, tj.
  `operator<`). Mora biti **strog** (lekcija 15, zadatak ex3). Bez poretka
  set ne može ni da ubaci element (`errors/e03`).
- ⚠️ Set ne pita `==`. Dva elementa su "ista" (**ekvivalentna**) kad
  `!(a < b) && !(b < a)` (Effective STL Item 19). Sa poretkom po dužini
  `"cc"` se ne ubaci, jer je "isto" što i `"aa"` (test). Sa poretkom bez
  obzira na velika slova, `"ANA"` je "isto" što i `"Ana"`.

---

# 3. `std::map` i `std::multimap` (kurs 173)

Parovi ključ → vrednost, sortirani po ključu; `value_type` je
`std::pair<const Kljuc, Vrednost>` (ključ je `const`).

| Operacija | Ako ključ ne postoji | Ako postoji |
|---|---|---|
| `m[k]` | **ubaci** `{k, Vrednost{}}`, vrati referencu | vrati referencu |
| `m.at(k)` | baci `std::out_of_range` | vrati referencu |
| `m.find(k)` | vrati `end()` | vrati iterator |
| `m.insert({k, v})` | ubaci | **ne menja** (vrati `false`) |
| `m.insert_or_assign(k, v)` (C++17) | ubaci | prepiši |
| `m.try_emplace(k, args...)` (C++17) | napravi vrednost od `args` | ne radi ništa, ni ne pravi vrednost |

- ⚠️ `m[k]` samo za čitanje **menja mapu**: `kanali["nepoznat"]` vrati 0 i
  ubaci ključ (test: size 3 → 4; zadatak ex2). Zato `[]` ne postoji za
  `const` mapu (lekcija 09, `errors/e08`). Za čitanje: `find` ili `at`.
- `[]` traži da vrednost ima podrazumevani konstruktor.
- Iteracija: `for (const auto& [kljuc, vrednost] : m)` (lekcija 10).
- `multimap`: više vrednosti po ključu. Nema `[]` (`errors/e04`);
  `equal_range(k)` daje opseg svih sa ključem `k`, redom ubacivanja
  (test: 21 22 20).

---

# 4. Neuređeni kontejneri (kurs 175–176)

`unordered_set`, `unordered_map`, `unordered_multiset`,
`unordered_multimap`: heš tabela.

- Element ide u **bucket** (pregradu) određen njegovim hešom:
  `bucket = hash(k) % bucket_count()`. Traženje: heš → bucket → poređenje
  sa elementima u bucket-u (`==`). Prosečno O(1), najgore O(n).
- **`load_factor()`** = `size() / bucket_count()`. Kad pređe
  `max_load_factor()` (podrazumevano 1.0), tabela se **rehešira**: više
  bucket-a, svi elementi se raspoređuju ponovo.
  - Rehash poništava **iteratore**. Reference na elemente ostaju važeće:
    čvorovi se ne premeštaju.
  - `reserve(n)` unapred pravi dovoljno bucket-a za `n` elemenata (test:
    posle `reserve(100)` i 100 ubacivanja `bucket_count` se nije menjao).
- ⚠️ **Redosled iteracije nije određen**, i menja se posle rehash-a. Za
  ispis ili poređenje sa očekivanim izlazom: prebaci u vektor i sortiraj
  (`main.cpp`, sekcija 4).
- Iteratori su samo forward (nema `--`).

---

# 5. `std::hash` i sopstveni heš (kurs 177)

- `std::hash<T>` postoji za ugrađene tipove, `std::string`, pokazivače,
  pametne pokazivače, `enum`-e... Za sopstveni tip **ne postoji**:
  `unordered_set<Tacka>` se ne kompajlira (`errors/e02`).
- Sopstveni heš je funkcijski objekat, drugi argument šablona; uz njega
  ide i `operator==` (ili treći argument):

  ```cpp
  struct HesTacke {
      std::size_t operator()(const Tacka& t) const noexcept {
          std::size_t h = std::hash<int>{}(t.x);
          return h ^ (std::hash<int>{}(t.y) + 0x9e3779b9 + (h << 6) + (h >> 2));
      }
  };
  std::unordered_set<Tacka, HesTacke> tacke;
  ```

  (Kombinovanje po uzoru na `boost::hash_combine`. Alternativa:
  specijalizacija `template <> struct std::hash<Tacka>`.)
- ✅ Pravila: jednaki objekti **moraju** imati jednak heš; različiti treba
  da imaju različit što češće.
- ⚠️ Loš heš ne kvari rezultat, nego brzinu. Heš koji uvek vraća 42 stavi
  svih 100 elemenata u jedan bucket (test), pa je traženje linearno.
  Heš `(x + y) % 4` za mrežu 30 x 30 daje 113 poređenja po traženju, a
  dobar heš 1 (test, libstdc++; zadatak ex3).

---

# 6. C++17: `extract` i `merge`

```cpp
auto cvor = uredjaji.extract(1);   // izvadi čvor iz mape (bez dealokacije)
cvor.key() = 10;                   // sada ključ sme da se menja
uredjaji.insert(std::move(cvor));  // vrati -- ista memorija, bez kopije vrednosti
a.merge(b);                        // premesti čvorove iz b u a; duplikati ostaju u b
```

- Jedini način da se promeni ključ bez brisanja i nove alokacije.
- `merge` ne kopira elemente -- premesti čvorove (test: `b` posle merge-a
  ima samo 3, jer je 3 već bio u `a`).

---

# Pregled: koji kontejner

| Treba ti | Izbor | Traženje | Redosled |
|---|---|---|---|
| skup vrednosti, sortiran | `set` | O(log n) | sortiran |
| ključ → vrednost, sortirano | `map` | O(log n) | sortiran po ključu |
| isto, sa duplikatima | `multiset`, `multimap` | O(log n) | sortiran |
| skup / mapa, bez potrebe za redosledom | `unordered_set`, `unordered_map` | O(1) prosek | nije određen |
| opseg ključeva (`lower_bound`...) | `set`, `map` | O(log n) | sortiran |
| mali broj elemenata | sortiran `vector` + `std::lower_bound` | O(log n) | sortiran |

✅ `unordered_map` kad ti ne treba redosled ni opsezi, a ključ ima dobar
heš. `map` kad treba sortiran ispis, opseg ključeva, ili kad ključ nema
prirodan heš.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 172 | std::set & std::multiset | sekcije 1, 2; `errors/e01`, `e03` |
| 173 | std::map & std::multimap | sekcija 3; `errors/e04`; `ub/u01`, `u02`; zadaci ex1, ex2 |
| 174 | Associative Containers Demo Code | `main.cpp` |
| 175–176 | Unordered Containers I–II | sekcija 4 |
| 177 | std::hash | sekcija 5; `errors/e02`; zadaci ex1, ex3 |
| 178 | Unordered Containers Demo Code | `main.cpp` |

---

# Pravilo za praksu

✅ Čitanje iz mape: `find` (ili `at`), ne `[]`.

✅ Ubacivanje bez prepisivanja: `insert`/`try_emplace`; sa prepisivanjem:
`insert_or_assign` ili `[]`.

✅ Brisanje u petlji: `it = m.erase(it);`.

✅ Sopstveni ključ u `unordered_*`: heš koji meša sva polja + `==`. U
`set`/`map`: strog `operator<`.

⚠️ Set i mapa porede **ekvivalencijom** preko poretka, ne `==`.

⚠️ `multiset::erase(vrednost)` briše sve jednake.

⚠️ Redosled u `unordered_*` nije određen -- ne oslanjaj se na njega.

**Rezime:** uređeni kontejneri su stablo -- sortirano, O(log n), traže
poredak. Neuređeni su heš tabela -- O(1) u proseku, bez redosleda, traže
heš i jednakost, a brzina zavisi od kvaliteta heša. U mapi `[]` ubacuje,
pa se za čitanje koristi `find`.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_indeks_reci`](exercises/ex1_indeks_reci.cpp) | usage | map za brojanje, map<string, set<int>> za indeks, unordered_map sa sopstvenim hešom (sekcije 1, 3, 4, 5) | — |
| [`ex2_indeks_ubacuje`](exercises/ex2_indeks_ubacuje.cpp) | why | zašto se u mapi ne proverava sa [] (sekcija 3) | `-DNAIVNO` |
| [`ex3_los_hes`](exercises/ex3_los_hes.cpp) | why | zašto je kvalitet heša bitan (sekcije 4, 5) | `-DNAIVNO` |

## Zapažanja posle vežbe
