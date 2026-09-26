# Lekcija 20 — Kopiranje

Kada se pravi kopija, šta kompajler sam napiše da bi je napravio, kada
to nije dovoljno (klasa poseduje resurs), i kako se kopiranje piše ili
zabranjuje. Pravila su iz C++03 i važe i danas. Move (C++11) je u lekciji 22.

**Izvori:** standard, delovi `[class.copy.ctor]`, `[class.copy.assign]` i
`[class.dtor]` (kada su implicitno definisani ili obrisani). Uz to
*Effective C++* **Item 5** (šta kompajler piše), **Item 6** (zabrana
kopiranja), **Item 11** (dodela samom sebi) i **Item 12** (kopiraj sve
delove), i C++ Core Guidelines **C.21** (rule of 3/5) i **C.20** (rule of 0).

**Kako vežbati:**

```
./build.sh 3-lifetime-and-resources/20-copying/main.cpp
./check_cases.sh 3-lifetime-and-resources/20-copying
```

- `errors/` (e01–e04): kod koji se **ne kompajlira**.
- `ub/` (u01–u02): kod koji se kompajlira, a ASan ga hvata.
- Srodno: lekcija 14 (copy ctor, `= delete`), 15 (`operator=`, dodela samom
  sebi), 16 (slicing).

---

# 1. Šta kompajler sam napiše (EC++ Item 5)

Za klasu bez ijedne napisane specijalne funkcije kompajler napiše:

| Funkcija | Šta radi |
|---|---|
| podrazumevani konstruktor | pravi svaki član njegovim podrazumevanim konstruktorom (samo ako nema nijednog tvog konstruktora) |
| copy konstruktor `T(const T&)` | kopira **član po član**, redom deklaracije, prvo baze |
| copy dodela `T& operator=(const T&)` | dodeljuje **član po član** |
| destruktor | uništava članove obrnutim redom |
| (C++11) move konstruktor i dodela | lekcija 22; pravila kada postoje: lekcija 23 |

Za `Person { std::string name; std::vector<int> scores; }` član po član
znači **duboku** kopiju, jer `string` i `vector` sami kopiraju sadržaj.
Test: izmena kopije ne menja original.

---

# 2. Kada se pravi kopija

| Kod | Šta se poziva |
|---|---|
| `T b = a;`, `T b(a);`, `T b{a};` | copy **konstruktor** (b tek nastaje) |
| `f(a)` za `void f(T)` | copy konstruktor (parametar) |
| `for (T item : v)` | copy konstruktor **za svaki element** |
| `catch (T e)` | copy konstruktor |
| `v.push_back(a)` | copy konstruktor (u vektor) |
| `return local;` | obično **izostavljeno** (copy elision, lekcija 24) |
| `b = a;` (b postoji) | copy **dodela** |
| `f(a)` za `void f(const T&)`, `for (const T& item : v)` | **ništa** |

Test iz `main.cpp`: range-for po vrednosti nad 3 elementa = 3 kopije, po
`const&` = 0.

---

# 3. Kada kompajler NE može da napiše kopiju

Tada funkciju **obriše** (`= delete` implicitno), i greška se javi tek
kad je neko pozove:

| Član klase | Copy konstruktor | Copy dodela |
|---|---|---|
| `const int id` | ✅ (id se inicijalizuje) | ❌ (`errors/e01`) |
| `int& target` | ✅ | ❌ (`errors/e02`) |
| `std::unique_ptr`, `std::mutex`, `std::thread` | ❌ (`errors/e03`) | ❌ |
| korisnički move konstruktor ili move dodela | ❌ | ❌ (lekcija 23) |

---

# 4. Plitka vs duboka kopija: rule of 3

Klasa koja poseduje resurs preko **sirovog** pokazivača (`char* data_`):

- Kompajlerova kopija kopira **pokazivač** (plitka kopija). Dva objekta
  onda "poseduju" istu memoriju: izmena jednog menja drugi, a oba
  destruktora je brišu (`ub/u01`, `attempting double-free`). Kod dodele
  se uz to izgubi stara memorija levog objekta (curenje).
- **Duboka kopija**: nova memorija i kopija **sadržaja**.

**Rule of 3** (C.21): ako klasa treba sopstveni **destruktor**, **copy
konstruktor** ili **copy dodelu**, gotovo sigurno treba sva tri. Razlog
je isti za sve: klasa ručno upravlja resursom.

```cpp
Name(const Name& other) : data_(copyOf(other.data_)) {}
Name& operator=(const Name& other) {
    if (this == &other) return *this;     // a = a (EC++ Item 11)
    char* fresh = copyOf(other.data_);    // prvo nova memorija...
    delete[] data_;                       // ...pa tek onda stara
    data_ = fresh;
    return *this;
}
~Name() { delete[] data_; }
```

Redosled "prvo alociraj, pa obriši" čuva objekat netaknutim ako `new`
baci izuzetak (strong garancija). Copy-and-swap to radi kraće: lekcija 21.

⚠️ Plitka kopija ne mora da bude pokazivač na heap. Klasa koja čuva
**pokazivač na sopstveni član** (`char* cursor_ = buf_;`) posle kopije
pokazuje u **tuđi** bafer (`ub/u02`). Rešenje: indeks umesto pokazivača.

---

# 5. Kopiraj sve delove (EC++ Item 12)

Kad pišeš copy konstruktor ili dodelu sam, kompajler više ne pomaže:

- **Novi član** dodat u klasu, a zaboravljen u copy konstruktoru, dobija
  podrazumevanu vrednost umesto kopije.
- **Bazna klasa** koja nije navedena u init listi pravi se
  **podrazumevanim** konstruktorom, a ne copy konstruktorom. Ako ga baza
  nema, greška (`errors/e04`); ako ga ima, kopija tiho dobija pogrešan
  bazni deo. Test: `Forgetful` kopija ima `id=0` i prazan `tag`, a **ni g++
  ni clang** sa `-Wall -Wextra` ne upozore.
- U dodeli se bazni deo kopira pozivom `Base::operator=(other);`.

```cpp
Careful(const Careful& other) : Base(other), tag_(other.tag_), extra_(other.extra_) {}
```

✅ Najbolja zaštita: **ne piši** copy konstruktor kad kompajlerov radi
ispravno (rule of 0, sledeća sekcija).

---

# 6. Zabrana kopiranja i rule of 0

- Klasa koja predstavlja **jedinstven** resurs (konekcija, fajl, nit)
  zabrani kopiju: `T(const T&) = delete; T& operator=(const T&) = delete;`
  (EC++ Item 6, lekcija 14).
- **Rule of 0** (C.20): klasa koja ne upravlja resursom direktno, nego
  ima članove koji to rade sami (`std::string`, `std::vector`,
  `std::unique_ptr`), ne piše **nijednu** specijalnu funkciju. Kompajlerove
  su tačne, i ne mogu da zaborave novi član.

| Klasa drži | Treba |
|---|---|
| `std::string`, `std::vector`, drugi "pametni" članovi | ništa (rule of 0) |
| sirov vlasnički pokazivač | rule of 3 (i move, rule of 5, lekcije 22–25), ili zameni pokazivač pametnim |
| jedinstven resurs | `= delete` za kopiju (i po potrebi move) |

---

# Pravilo za praksu

✅ Rule of 0: članovi koji sami upravljaju resursom, bez sopstvenih
specijalnih funkcija.

✅ Ako ipak pišeš jednu od tri (destruktor, copy ctor, copy dodela),
napiši sve tri (rule of 3), ili zabrani kopiju.

✅ Sopstveni copy konstruktor kopira **baznu klasu** i **svaki član**.

✅ Parametri i range-for po `const&` kad kopija nije potrebna.

⚠️ `const` član i referenca kao član brišu dodelu.

⚠️ Pokazivač na sopstveni član se posle kopije pokazuje u original.

**Rezime:** kopija je po podrazumevanju "član po član". To je tačno kad
svaki član sam zna da se kopira, a pogrešno kad klasa ručno poseduje
resurs: tada dva objekta dele isti resurs i oba ga oslobađaju. Rešenje je
ili ručna duboka kopija (rule of 3), ili zabrana kopije, ili, najbolje,
članovi koji se sami ispravno kopiraju (rule of 0).

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_rule_of_three`](exercises/ex1_rule_of_three.cpp) | usage | rule of 3: duboka kopija, copy-and-swap, destruktor (sekcija 4) | — |
| [`ex2_kopiraj_sve_delove`](exercises/ex2_kopiraj_sve_delove.cpp) | why | zašto ručna kopija mora da kopira i baznu klasu (sekcija 5, EC++ Item 12) | `-DNAIVNO` |
| [`ex3_referenca_clan`](exercises/ex3_referenca_clan.cpp) | why | zašto član-referenca ukida dodelu (sekcija 3) | `-DNAIVNO` |

## Zapažanja posle vežbe

