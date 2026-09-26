# Lekcija 32 — Pametni pokazivači

Sirov pokazivač ne kaže ko je vlasnik objekta, ni ko treba da ga obriše
(lekcija 04, sekcija 13; lekcija 13). Pametni pokazivači to izražavaju
**tipom**: `unique_ptr` je jedini vlasnik, `shared_ptr` jedan od više
vlasnika, `weak_ptr` posmatrač koji ne produžava život. Destruktor briše
objekat kad treba (RAII, lekcija 21).

**Izvori:** standard, delovi `[unique.ptr]`, `[util.smartptr.shared]`,
`[util.smartptr.weak]` i `[util.smartptr.enab]`. Uz to *Effective Modern
C++* **Item 18** (`unique_ptr`), **Item 19** (`shared_ptr`), **Item 20**
(`weak_ptr`), **Item 21** (`make_unique`/`make_shared`) i **Item 22**
(pimpl sa `unique_ptr`), i C++ Core Guidelines **R.20–R.37**. Nastavci
kursa 72–82.

**Kako vežbati:**

```
./build.sh 6-smart-pointers/32-smart-pointers/main.cpp
./check_cases.sh 6-smart-pointers/32-smart-pointers
```

- `errors/` (e01–e04): kod koji se **ne kompajlira**.
- `ub/` (u01–u04): kod koji se kompajlira, a ASan/LSan ga hvata (u01 je
  curenje, ne UB).
- `main.cpp` zamenjuje globalni `operator new` samo da bi prebrojao
  alokacije (sekcija 3).

---

# 1. Ko je vlasnik (R.3, R.20, R.30–R.33)

| Tip | Značenje | Kopija | Veličina (test) |
|---|---|---|---|
| `T*`, `T&` | **posmatram**, ne posedujem | — | 8 |
| `std::unique_ptr<T>` | **jedini** vlasnik | ❌, samo move | 8 (kao sirov pokazivač) |
| `std::shared_ptr<T>` | **jedan od** vlasnika | ✅ (atomski +1) | 16 (objekat + kontrolni blok) |
| `std::weak_ptr<T>` | posmatrač koji zna da li objekat još živi | ✅ | 16 |

**Kako funkcija kaže šta radi sa objektom:**

| Parametar | Značenje |
|---|---|
| `const Widget&` / `Widget&` | koristi, objekat sigurno postoji |
| `const Widget*` / `Widget*` | koristi, može biti `nullptr` |
| `std::unique_ptr<Widget>` (po vrednosti) | **preuzima** vlasništvo; poziv `consume(std::move(p))` |
| `std::unique_ptr<Widget>&` | menja **pokazivač** (reset, zamena), retko |
| `std::shared_ptr<Widget>` (po vrednosti) | **deli** vlasništvo (čuva kopiju) |
| `const std::shared_ptr<Widget>&` | možda će napraviti kopiju, a možda ne |

✅ Funkcija koja samo **koristi** objekat prima referencu ili sirov
pokazivač, a ne pametni pokazivač (R.30). Tako radi sa objektom bez
obzira na to ko ga poseduje.

---

# 2. `unique_ptr` (kurs 73, 75)

```cpp
auto a = std::make_unique<Widget>(1);   // napravi
inspect(*a);                            // posmatrač: referenca
inspectMaybe(a.get());                  // posmatrač: sirov pokazivač
auto b = std::move(a);                  // prenos; a == nullptr
consume(std::move(b));                  // vlasništvo ode u funkciju
a.reset();                              // obriši sada
Widget* raw = a.release();              // odrekni se bez brisanja (retko)
```

- Bez dodatne cene u odnosu na sirov pokazivač (ista veličina, isti kod).
- Fabrika vraća `unique_ptr`: pozivalac može da ga pretvori u
  `shared_ptr` ako mu treba (`std::shared_ptr<T> s = createWidget();`).
- ⚠️ `get()` daje **posmatrača**. Posle `reset()` ili uništenja vlasnika
  visi (`ub/u04`).
- **Pimpl** (EMC Item 22): `std::unique_ptr<Impl>` sa nepotpunim `Impl`
  zahteva da destruktor bude **deklarisan** u header-u, a definisan
  (`= default`) u `.cpp` posle definicije `Impl`. Inače se ne kompajlira
  (`errors/e04`).

---

# 3. `shared_ptr` i make funkcije (kurs 74, 76, 82)

```cpp
auto first = std::make_shared<Widget>(10);   // use_count = 1
auto second = first;                          // use_count = 2 (atomski)
```

- **Kontrolni blok**: brojač vlasnika (`use_count`), brojač posmatrača
  (weak count), deleter. Objekat se briše kad `use_count` padne na 0.
- Kopiranje `shared_ptr` je **atomska** operacija (bezbedno iz više
  niti), pa je skuplje od kopiranja pokazivača. Prosleđuj po `const&`
  kad funkcija ne čuva kopiju, ili samo `Widget&`.
- ⚠️ Dva `shared_ptr` napravljena od **istog sirovog pokazivača** imaju
  dva kontrolna bloka i dva brisanja (`ub/u02`).

**Make funkcije (EMC Item 21):**

| | Test: alokacija |
|---|---|
| `std::make_shared<T>()` | **1** (objekat i kontrolni blok zajedno) |
| `std::shared_ptr<T>(new T)` | 2 |
| `std::make_unique<T>()` | 1 |

✅ `make_unique` / `make_shared` umesto `new`: kraće, bez `new`/`delete`
u kodu, i bez ponavljanja tipa.

Kada **ne** koristiti make funkcije:

- treba custom deleter (make funkcije ga ne primaju);
- inicijalizacija vitičastim zagradama (`make_unique<std::vector<int>>(3, 1)`
  pravi `(3, 1)`, ne `{3, 1}`);
- agregat u C++17: `make_unique<Point>(1, 2)` se ne kompajlira, jer koristi
  zagrade (`errors/e02`); u C++20 radi (P0960);
- ⚠️ `make_shared` + dugo živi `weak_ptr` + veliki objekat: pošto su
  objekat i kontrolni blok **jedna** alokacija, memorija se oslobađa tek
  kad nestane i poslednji `weak_ptr`. Test: destruktor se pozove kad
  nestane poslednji `shared_ptr`, a memorija se oslobodi tek posle
  `weak.reset()`.

---

# 4. `weak_ptr` (kurs 77, 78)

```cpp
std::weak_ptr<Widget> observer = owner;       // ne menja use_count
if (auto locked = observer.lock()) { ... }    // privremeni vlasnik, ili prazno
observer.expired();                           // da li je objekat nestao
```

- Nema `*` ni `->` (`errors/e03`). Jedini put do objekta je `lock()`, koji
  **atomski** proveri i napravi `shared_ptr`. "Proveri `expired()` pa
  koristi" bi imalo trku sa drugom niti.
- Tipična upotreba: keš, posmatrači (observer pattern), i razbijanje
  ciklusa.

---

# 5. Kružne reference (kurs 79)

```cpp
a->partner = b;   // shared_ptr
b->partner = a;   // shared_ptr -> ciklus: use_count nikad 0, ~Node se ne poziva (ub/u01)
```

✅ Bar jedan smer `weak_ptr`. U hijerarhiji roditelj–dete: roditelj
**poseduje** decu (`unique_ptr` ili `shared_ptr`), a dete zna roditelja
kao `weak_ptr` ili sirov pokazivač (dete ne živi duže od roditelja).

---

# 6. Deleter (kurs 80)

| | Deleter je deo tipa? | Veličina (test) |
|---|---|---|
| `unique_ptr<FILE, FileCloser>` (prazna struktura) | **da** | 8 |
| `unique_ptr<FILE, void(*)(FILE*)>` | da | 16 (čuva i pokazivač na funkciju) |
| `unique_ptr<Widget, decltype(lambda)>` (lambda bez capture-a) | da | 8 |
| `shared_ptr<Widget>` sa bilo kojim deleterom | **ne** (u kontrolnom bloku) | 16 |

- `unique_ptr` sa različitim deleterima su **različiti tipovi**; ne mogu
  u isti vektor.
- `shared_ptr` sa različitim deleterima je **isti** tip (test: oba u
  jednom `std::vector<std::shared_ptr<Widget>>`).
- Deleter je način da se RAII primeni na C API: `FILE*` + `fclose`,
  `SDL_Window*` + `SDL_DestroyWindow`, handle + `CloseHandle` (lekcija 21).

---

# 7. Nizovi (kurs 81)

```cpp
auto numbers = std::make_unique<int[]>(4);            // delete[], operator[], nule
std::shared_ptr<int[]> shared(new int[3]{1, 2, 3});   // C++17
```

- `unique_ptr<int[]>` nema `*` ni `->` (`errors/e01`).
- ❌ `std::unique_ptr<int> p(new int[5]);` briše sa `delete` umesto
  `delete[]` (`ub/u03`). `make_unique<int[]>` ne može da pogreši.
- ✅ Kad veličina treba da se menja ili treba `size()`: `std::vector`.

---

# 8. `enable_shared_from_this`

Objekat koji treba da da `shared_ptr` na **sebe** (npr. da registruje
callback koji ga drži živim) ne sme `shared_ptr<T>(this)`: to je drugi
kontrolni blok (`ub/u02`).

```cpp
class Session : public std::enable_shared_from_this<Session> {
    std::shared_ptr<Session> self() { return shared_from_this(); }
};
```

Radi samo ako objekat **već** pripada nekom `shared_ptr`-u. Na objektu na
steku `shared_from_this()` od C++17 baca `std::bad_weak_ptr` (test).

---

# Mapa na kurs

| Nastavak kursa | Sekcija |
|---|---|
| 72 Raw Pointers | 1 (i lekcije 04, 13) |
| 73 `std::unique_ptr` | 2 |
| 74 Sharing Pointers, 75 Sharing `unique_ptr` | 1, 2 |
| 76 `std::shared_ptr` | 3 |
| 77 Weak Ownership, 78 `weak_ptr` Internals | 4 |
| 79 Circular References | 5 |
| 80 Deleter | 6 |
| 81 Dynamic Arrays | 7 |
| 82 Make Functions | 3 |

---

# Pravilo za praksu

✅ Podrazumevano `std::unique_ptr` (preko `make_unique`); `shared_ptr`
samo kad je vlasništvo zaista deljeno.

✅ Funkcije koje samo koriste objekat primaju `T&` ili `T*`, ne pametni
pokazivač.

✅ Nikad dva pametna pokazivača iz istog sirovog pokazivača.

✅ Ciklus: bar jedan smer `weak_ptr`.

⚠️ `get()` je posmatrač, ne produžava život.

⚠️ ASan vidi samo **instrumentisan** kod: čitanje oslobođenog
`std::string` kroz `size()` iz neinstrumentisanog libstdc++ prošlo je
bez prijave (test), a isto čitanje obične strukture je prijavljeno.

**Rezime:** pametni pokazivač je RAII za objekat na heap-u, a njegov tip
dokumentuje vlasništvo. `unique_ptr` je besplatan i treba da bude
podrazumevani izbor, `shared_ptr` plaća kontrolni blok i atomske
brojače za deljeno vlasništvo, a `weak_ptr` posmatra bez vlasništva i
razbija cikluse. Sirov pokazivač i referenca ostaju za posmatrače.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVE`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_ownership`](exercises/ex1_ownership.cpp) | usage | unique_ptr, shared_ptr i weak_ptr po nameni (sekcije 1-4) | — |
| [`ex2_circular_reference`](exercises/ex2_circular_reference.cpp) | why | zašto weak_ptr za "pokazivač nazad" (sekcija 5) | `-DNAIVE` |
| [`ex3_shared_from_this`](exercises/ex3_shared_from_this.cpp) | why | zašto enable_shared_from_this, a ne shared_ptr(this) (sekcija 8) | `-DNAIVE` |

## Zapažanja posle vežbe

