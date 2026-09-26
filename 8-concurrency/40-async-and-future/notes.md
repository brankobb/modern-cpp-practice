# Lekcija 40 — `std::async`, `std::future`, `std::promise` (kurs 196–201)

U lekciji 39 je rezultat iz niti išao kroz referencu, uz `join` i pažnju da
svaka nit piše u svoje mesto, a izuzetak iz niti je obarao program. **Rad
zasnovan na zadacima** (task-based) to rešava: `std::async` pokrene
funkciju i odmah vrati `std::future` -- "obećanje" rezultata. `get()`
sačeka i vrati vrednost, ili baci izuzetak koji je zadatak bacio.
`std::promise` je druga strana istog kanala: vrednost u future upisuje
tvoj kod, iz bilo koje niti.

Već obrađeno, ovde samo upućujemo:

- `std::thread`, argumenti se kopiraju, `std::ref`, data race, mutex:
  lekcija 39;
- `std::exception_ptr` i `std::current_exception`: lekcija 18, sekcija 9;
- move-only tipovi (`future`, `promise`, `packaged_task`): lekcija 22, sekcija 8.

**Izvori:** standard, `[futures]`: `[futures.async]` (`std::async`,
politike, destruktor koji čeka), `[futures.unique.future]`,
`[futures.shared.future]`, `[futures.promise]`, `[futures.task]`
(`packaged_task`), `[futures.errors]` (`future_errc`). *Effective Modern
C++* **Item 35** (zadaci umesto niti), **Item 36** (navedi
`std::launch::async` ako je asinhronost bitna), **Item 38** (destruktor
future-a se ponaša različito), **Item 39** (`future<void>` za
jednokratne signale). Core Guidelines **CP.4** (misli u zadacima, ne u
nitima), **CP.60** (`future` za vraćanje vrednosti iz konkurentnog
zadatka), **CP.61** (`async` za pokretanje konkurentnih zadataka).

**Kako vežbati:**

```
./build.sh 8-concurrency/40-async-and-future/main.cpp                # ISPRAVNI slučajevi, ASan+UBSan
./build.sh 8-concurrency/40-async-and-future/main.cpp --tsan         # isto, pod ThreadSanitizer-om
./check_cases.sh 8-concurrency/40-async-and-future                   # svi POGREŠNI slučajevi
./check_exercises.sh 8-concurrency/40-async-and-future               # vežbe
```

- `errors/` (e01–e05): kod koji se **ne kompajlira**.
- `ub/` (u01–u02): data race između zadataka (TSan, `// SANITIZER:
  thread`); future koji nadživi lokalnu promenljivu (ASan).
- `runtime/` (r01–r04): `std::terminate` zbog neuhvaćenog izuzetka --
  `get` dvaput, prekršeno obećanje, dvaput `set_value`, izuzetak iz
  zadatka bez `try`.
- Tekstovi `what()` (npr. "Broken promise") su iz libstdc++; `main.cpp`
  zato poredi `e.code()`, a ne tekst.

---

# 1. `std::async` i `std::future` (kurs 196)

```cpp
std::future<int> f = std::async(racunaj, 10);   // pokrene, odmah vrati future
int x = f.get();                                // čeka i vrati rezultat
```

- ❌ `async` ne vraća vrednost nego `future<T>` (`errors/e02`).
- `get()` **preuzme** rezultat: posle njega `valid() == false` (test).
  Drugi `get()` je po standardu UB; libstdc++ baci `future_error`
  (`runtime/r01`). ❌ Zato `get()` nije `const` (`errors/e05`).
- `future` je move-only: jedan rezultat, jedan čitalac (`errors/e01`).
- Uporedi sa lekcijom 39, sekcija 4: tamo `vector<long>`, `std::ref` i `join`;
  ovde `vector<future<long>>` i `get()` (test: zbir 1..1000 u 4 zadatka =
  500500). Nema deljenog stanja koje treba štititi.
- ⚠️ `async` ne štiti podatke koje zadaci dele -- dva zadatka nad istim
  brojačem su data race kao i sa nitima (`ub/u01`). ✅ Neka svaki zadatak
  **vrati** svoj rezultat (CP.60).

---

# 2. Argumenti, metode, `shared_future` (kurs 197)

- Argumenti se kopiraju kao kod `std::thread`: `int&` bez `std::ref` se
  ne kompajlira (`errors/e03`); sa `std::ref` original mora da živi dok
  zadatak radi.
- ⚠️ Future vraćen iz funkcije se premešta pozivaocu, pa zadatak radi i
  posle `return`. Capture po referenci na lokalnu promenljivu te funkcije
  tada visi (`ub/u02`, ASan: `stack-use-after-return`).
- Metoda: `std::async(&Kalibrator::primeni, &k, 10)` (test: 15).
- **`shared_future`** (`f.share()`): kopira se, i svaka kopija sme da zove
  `get()` koliko puta hoće (`get()` je `const` i vraća `const T&`). Test:
  tri zadatka čitaju isti rezultat 100.

---

# 3. Politike pokretanja (kurs 198)

| Politika | Kad se izvrši | U kojoj niti |
|---|---|---|
| `std::launch::async` | odmah | nova nit |
| `std::launch::deferred` | tek na `get()`/`wait()`; **nikad**, ako ih niko ne pozove | nit koja zove `get()` |
| bez politike (`async \| deferred`) | bira biblioteka | bira biblioteka |

- Test: `async` zadatak ima drugi id niti; `deferred` nije pokrenut pre
  `get()`, radi u `main` niti, a bez `get()` se nikad ne izvrši.
- ⚠️ Podrazumevana politika dozvoljava i `deferred` (EMC Item 36). Tada
  zadatak nije konkurentan, `thread_local` pripada pozivaocu, a petlja
  `while (f.wait_for(10ms) != ready)` se vrti zauvek (vraća `deferred`).
  libstdc++ bez politike pokrene novu nit (provereno sa g++ i clang,
  van `main.cpp`), ali to standard ne garantuje.
- ✅ Kad je bitno da radi paralelno: `std::launch::async` eksplicitno.

---

# 4. Čekanje: `wait`, `wait_for`, `wait_until` (kurs 199)

```cpp
f.wait();                          // čeka, ne uzima rezultat
f.wait_for(10ms);                  // future_status::ready / timeout / deferred
f.wait_until(steady_clock::now() + 10ms);
```

- Test: zadatak blokiran dok `main` ne da signal → `timeout`, dva puta;
  posle signala i `wait()` → `ready`; `deferred` zadatak → `deferred`
  (nije pokrenut, pa nema šta da se čeka).
- ✅ Čekanje sa rokom: `wait_for`, pa `get()` samo ako je `ready`
  (zadatak ex1).
- ⚠️ **Destruktor future-a iz `std::async` čeka kraj zadatka** (EMC Item
  38; `[futures.async]`). Test: posle uništenja future-a, bez `get()`,
  zadatak je već gotov. Posledice:
  - odbačen rezultat `std::async(...)` čeka odmah, u istom iskazu, pa
    "pokreni i zaboravi" radi **sekvencijalno** (zadatak ex2). libstdc++
    zato označava `std::async` sa `[[nodiscard]]`: g++ i clang upozore
    (`-Wunused-result`);
  - future zadatka koji nikad ne završi zaglavi program u destruktoru.
  - Future dobijen iz `promise`-a ili `packaged_task`-a NE čeka u
    destruktoru.

---

# 5. `std::promise` (kurs 200)

```cpp
std::promise<int> p;
std::future<int> f = p.get_future();
std::thread t([&p] { p.set_value(21); });   // upis iz bilo koje niti
f.get();                                     // 21
```

- `promise` je strana koja **piše**, `future` strana koja **čita**; kanal
  je jednokratan. ❌ Drugi `set_value` baca `future_error`
  (`promise_already_satisfied`, `runtime/r03`).
- `promise<void>`: signal bez vrednosti, `set_value()` bez argumenta (za
  `promise<int>` je to greška, `errors/e04`). Sa `shared_future<void>`
  jedan signal čeka više zadataka (sekcija 4 u `main.cpp`; EMC Item 39).
- ⚠️ `promise` uništen bez vrednosti upiše `future_error` sa kodom
  `broken_promise` -- future ne čeka zauvek, ali dobije samo "prekršeno
  obećanje" (test; neuhvaćen: `runtime/r02`; zadatak ex3).
- `promise` je move-only: u nit ide sa `std::move(p)` (zadatak ex3).
- **`packaged_task<int(int, int)>`**: funkcija + promise u jednom objektu;
  poziv upiše rezultat u njegov future. Zgodno kad zadatak pokreće neko
  drugi (bazen niti, red poslova). Test: 6 * 7 u niti = 42.

---

# 6. Izuzeci preko niti (kurs 201)

- `std::async` hvata izuzetak zadatka i sačuva ga u future; **`get()` ga
  baci ponovo**, u niti koja zove `get` (test: `invalid_argument` iz
  zadatka uhvaćen u `main`-u). Bez `try` oko `get()` → `terminate`
  (`runtime/r04`), ali tek u pozivaocu -- za razliku od `std::thread`-a,
  gde izuzetak obori program odmah (lekcija 39, `runtime/r02`).
- Sa `promise` to radiš ručno:

  ```cpp
  try { p.set_value(racunaj()); }
  catch (...) { p.set_exception(std::current_exception()); }   // lekcija 18, sekcija 9
  ```

  ili bez `throw`: `p.set_exception(std::make_exception_ptr(std::domain_error("...")))`
  (zadatak ex3).
- ✅ Svaka putanja postavlja **ili vrednost ili izuzetak** -- inače
  pozivalac dobije `broken_promise` umesto pravog razloga (zadatak ex3).

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 196 | Task Based Concurrency - Part I | sekcija 1; `errors/e01`, `e02`, `e05`; `ub/u01`; `runtime/r01`; zadatak ex1 |
| 197 | Task Based Concurrency - Part II | sekcija 2; `errors/e03`; `ub/u02` |
| 198 | Launch Policies | sekcija 3 |
| 199 | std::future Wait Functions | sekcija 4; zadaci ex1, ex2 |
| 200 | Using std::promise | sekcija 5; `errors/e04`; `runtime/r02`, `r03`; zadatak ex3 |
| 201 | Propagating Exceptions Across Threads | sekcija 6; `runtime/r04`; zadaci ex1, ex3 |

---

# Pravilo za praksu

✅ Za posao sa rezultatom: `std::async` + `future` umesto `std::thread` +
reference (EMC Item 35, CP.4, CP.60).

✅ `std::launch::async` kad zadatak mora da radi paralelno (EMC Item 36).

✅ Sačuvaj future iz `async`-a u promenljivu; `get()` tek kad su svi
zadaci pokrenuti.

✅ `promise`: na svakoj putanji `set_value` ili `set_exception`.

⚠️ `get()` jednom; za više čitalaca `shared_future`.

⚠️ `async` ne rešava deljene podatke: zadatak vraća svoj rezultat, a
zajedničko stanje i dalje ide pod mutex.

**Rezime:** `std::future` je kanal za jedan rezultat -- vrednost ili
izuzetak. `std::async` pravi i zadatak i kanal, `std::promise` daje
kanal koji puniš sam, `packaged_task` vezuje kanal za funkciju.
Nezgodni detalji su politika pokretanja (podrazumevana može biti
`deferred`) i destruktor future-a iz `async`-a, koji čeka.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVE`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_citanje_senzora`](exercises/ex1_citanje_senzora.cpp) | usage | paralelno čitanje senzora preko std::async, izuzeci kroz future, čekanje sa rokom (sekcije 1, 4, 6) | — |
| [`ex2_odbacen_future`](exercises/ex2_odbacen_future.cpp) | why | zašto "pokreni i zaboravi" sa std::async ne radi paralelno (sekcija 4, EMC Item 38) | `-DNAIVE` |
| [`ex3_obecanje_bez_greske`](exercises/ex3_obecanje_bez_greske.cpp) | why | zašto promise mora da dobije i GREŠKU, ne samo vrednost (sekcije 5, 6) | `-DNAIVE` |

## Zapažanja posle vežbe
