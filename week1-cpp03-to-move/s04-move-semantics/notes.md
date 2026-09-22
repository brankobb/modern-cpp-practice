# Sesija 4 — Move semantika

Kopija pravi **nov** resurs (s02). Kad izvor posle toga niko ne koristi,
to je bačen posao: bolje je **preuzeti** njegov resurs. Move semantika
(C++11) daje jeziku način da prepozna "ovaj objekat se više ne koristi" i
da tada pozove move umesto kopije.

**Izvori:** standard, delovi `[basic.lval]` (kategorije vrednosti),
`[dcl.init.ref]` (vezivanje referenci), `[class.copy.ctor]` i
`[class.copy.assign]` (move konstruktor i dodela), `[utility]`
(`std::move`, `std::exchange`) i `[lib.types.movedfrom]`. Uz to
*Effective Modern C++* **Item 23** (`std::move` ne pomera), **Item 25**
(`std::move` za rvalue reference) i **Item 29** (move nije uvek jeftin), i
C++ Core Guidelines **C.64–C.66** i **ES.56**.

**Kako vežbati:**

```
./build.sh week1-cpp03-to-move/s04-move-semantics/main.cpp
./check_cases.sh week1-cpp03-to-move/s04-move-semantics
```

- `errors/` (e01–e04): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): kod koji se kompajlira, a ASan/UBSan ga hvata.
- Srodno: lekcija 04 (reference), 09 (overload po `&` i `&&`, EMC 26),
  week2 s06 (kada kompajler piše move), s07 (return i parametri), s09
  (`std::forward`).

---

# 1. Kategorije vrednosti

Svaki izraz ima tip **i** kategoriju. Kategorija odlučuje da li sme da se
"isprazni".

| Kategorija | Šta je | Primer | `decltype((izraz))` |
|---|---|---|---|
| **lvalue** | ima identitet (ime, adresu) | `s`, `firstName()` (vraća `T&`), `"abc"`, `*p` | `T&` |
| **prvalue** | čista privremena vrednost | `42`, `x + 1`, `makeName()` (vraća `T`) | `T` |
| **xvalue** | ima identitet, ali **sme da se isprazni** | `std::move(s)`, funkcija koja vraća `T&&` | `T&&` |

`rvalue` = prvalue ili xvalue. **rvalue je ono što sme da se pomeri.**

⚠️ String literal `"abc"` je **lvalue** (niz u statičkoj memoriji).

---

# 2. Vezivanje referenci

| Parametar | lvalue | const lvalue | rvalue |
|---|---|---|---|
| `T&` | ✅ | ❌ | ❌ |
| `const T&` | ✅ | ✅ | ✅ (ali slabiji izbor od `T&&`) |
| `T&&` | ❌ (`errors/e01`, `e04`) | ❌ | ✅ |

Kad postoje oba overload-a, rvalue bira `T&&`, a lvalue `const T&`. Na
tome počiva cela move semantika: copy konstruktor prima `const T&`, a move
konstruktor `T&&`.

---

# 3. `std::move` je samo cast (EMC Item 23)

```cpp
template <typename T>
std::remove_reference_t<T>&& move(T&& t) noexcept { return static_cast<std::remove_reference_t<T>&&>(t); }
```

- `std::move(s)` **ništa ne pomera**. Samo kaže "ovaj izraz je xvalue",
  pa overload resolution bira move konstruktor ili dodelu. Test: posle
  `auto&& r = std::move(s);` je `s.size()` i dalje 40.
- Pomeranje radi tek funkcija koja primi `T&&` (npr. move konstruktor
  `std::string`-a).
- ⚠️ `std::move` na **const** objektu daje `const T&&`. On ne može u
  `T&&` (move), ali može u `const T&` (kopija), pa se **tiho kopira**.
  Test: `Buffer copy = std::move(frozen);` ispiše `[copy ctor]`.

---

# 4. Move konstruktor i move dodela

```cpp
Buffer(Buffer&& other) noexcept
    : data_(std::exchange(other.data_, nullptr)),   // uzmi i isprazni izvor u istom koraku
      size_(std::exchange(other.size_, 0)) {}

Buffer& operator=(Buffer&& other) noexcept {
    if (this != &other) {                           // a = std::move(a) (ub/u03)
        delete[] data_;
        data_ = std::exchange(other.data_, nullptr);
        size_ = std::exchange(other.size_, 0);
    }
    return *this;
}
```

Pravila:

- **Izvor ostaje ispravan** (C.64): njegov destruktor mora da radi, a
  dodela nove vrednosti mora da radi. Za klasu sa pokazivačem to znači
  `nullptr`. Bez toga su dva vlasnika i `attempting double-free`
  (`ub/u01`).
- **`noexcept`** (C.66): move ne alocira, pa ne treba ni da baca. Bez
  `noexcept` ga `std::vector` pri realokaciji **ne koristi** i kopira
  umesto toga (week2 s06).
- **Dodela samom sebi**: `a = std::move(a)` ne sme da pokvari objekat
  (C.65). Prvo `delete`, pa preuzimanje istog pokazivača čita oslobođenu
  memoriju (`ub/u03`).
- **Kada se poziva move, a kada kopija**:

| Kod | Poziva |
|---|---|
| `Buffer b = std::move(a);` | move konstruktor |
| `Buffer c = b;` | copy konstruktor |
| `c = makeBuffer(7);` (privremeni) | move dodela, **bez** `std::move` |
| `a = b;` | copy dodela |
| `return local;` | izostavljeno ili move, nikad kopija (week2 s07) |

- ⚠️ Čim klasa **deklariše** move konstruktor ili move dodelu, kompajler
  **obriše** kopiju (`errors/e03`). Ceo skup pravila je u week2 s06.

---

# 5. Imenovana rvalue referenca je lvalue

```cpp
void storeCopying(Buffer&& value) { storage.push_back(value); }            // [copy ctor] !
void storeMoving(Buffer&& value)  { storage.push_back(std::move(value)); } // [move ctor]
```

Tip parametra je `Buffer&&`, ali **izraz** `value` ima ime, pa je
**lvalue**. Da se ne bi pomerio slučajno (npr. pa koristio ponovo u
sledećoj liniji), jezik traži da se `std::move` napiše eksplicitno, na
mestu gde se objekat poslednji put koristi (EMC Item 25).

---

# 6. Moved-from stanje

- Standard za tipove iz biblioteke kaže samo: **ispravno, ali nepoznato**
  (`[lib.types.movedfrom]`). Dozvoljeno je: uništiti ga, dodeliti mu novu
  vrednost, pozvati funkcije bez preduslova (`size()`, `empty()`, `clear()`).
- ⚠️ U praksi je moved-from `std::string` prazan (libstdc++, test), ali
  standard to ne obećava. Kod ne sme da se oslanja na sadržaj.
- `std::unique_ptr` i `std::shared_ptr` su izuzetak: posle move-a su
  **garantovano** `nullptr`. `*owner` je tada UB (`ub/u02`), a ni g++ ni
  clang sa `-Wall` ne upozore. Provera `bugprone-use-after-move` postoji u
  clang-tidy.

---

# 7. Move nije uvek jeftin (EMC Item 29)

| Tip | Move |
|---|---|
| `std::vector`, dugačak `std::string`, `unique_ptr` | preuzme pokazivač: O(1) |
| kratak `std::string` (SSO: znakovi u samom objektu) | kopira znakove. Test: posle move-a drugi bafer |
| `std::array<int, 1000>` | kopira svih 1000 elemenata (4000 bajtova) |
| `int`, `double`, trivijalne strukture | isto što i kopija |
| tip bez move konstruktora | kopija |

Move pomaže samo kad objekat drži resurs **preko pokazivača**. Za
objekte koji su "sami svoji podaci" move i kopija su isti posao.

---

# 8. Tipovi koji se samo pomeraju

`std::unique_ptr`, `std::thread`, `std::fstream`, `std::unique_lock` se
**ne mogu kopirati**, samo pomerati (`errors/e02`). U kontejner idu sa
`std::move(p)` ili kao privremeni (`push_back(std::make_unique<int>(8))`).
Tako se vlasništvo prenosi eksplicitno, i u kodu se vidi gde.

---

# Mapa na kurs

| Nastavak kursa | Sekcija |
|---|---|
| 53 L-values, R-values & R-value References | 1, 2 |
| 54 Move Semantics Basics | 4, 5 |
| 55 Move Semantics Implementation | 4 |
| 58 `std::move` Function | 3, 6 |
| 56 Rule of 5 & 0 | s05 |
| 57 Copy Elision | week2 s07 |

---

# Pravilo za praksu

✅ Move konstruktor i dodela su `noexcept` i ostavljaju izvor ispravnim
(`std::exchange(ptr, nullptr)`).

✅ `std::move` samo na mestu **poslednje** upotrebe objekta.

✅ Unutar funkcije koja prima `T&&`: `std::move(param)` kad ga predaješ
dalje.

⚠️ `std::move` na `const` objektu tiho kopira.

⚠️ Posle `std::move(x)` objekat `x` samo dobija novu vrednost ili se
uništava.

⚠️ Deklarisan move briše kompajlerovu kopiju.

**Rezime:** move je optimizacija kopije za slučaj kad izvor više ne
treba. Jezik ga bira sam za privremene objekte, a za imenovane samo kad
to eksplicitno kažeš sa `std::move`, koji je samo cast. Tipovi koji drže
resurs preko pokazivača dobijaju move u O(1), a svi ostali isto što i
kopiju.

## Zapažanja posle vežbe

