# Lekcija 39 — Niti, `std::mutex`, `lock_guard` (kurs 189–195)

C++11 je u jezik uneo **niti** (`<thread>`) i **model memorije**: pravila
šta jedna nit sme da vidi od upisa druge. Pre toga je sve išlo preko
biblioteka operativnog sistema (pthreads, Win32). Ova lekcija pokriva
niti na niskom nivou: pravljenje, argumente, vraćanje rezultata i
zaštitu deljenih podataka mutex-om. Zadaci (`std::async`, `future`,
`promise`) su u lekciji 40.

Već obrađeno, ovde samo upućujemo:

- RAII i stack unwinding (osnova za `lock_guard`): lekcija 21, sekcije 1 i 2;
- RAII zaključavanje na mikrokontroleru (prekidi): lekcija 33, Deo 2;
- lambda capture po vrednosti i po referenci: lekcija 30, sekcija 5;
- `std::ref` i "kopira sve argumente" (isto pravilo kao `std::bind`): lekcija 31,
  sekcija 4;
- move-only tipovi (`std::thread` je jedan od njih): lekcija 22, sekcija 8.

**Izvori:** standard, `[thread]`: `[thread.thread.class]` (konstruktor,
`join`, `detach`, destruktor), `[thread.thread.this]` (`this_thread`),
`[thread.mutex.requirements]`, `[thread.lock.guard]`,
`[thread.lock.scoped]`, `[thread.lock.unique]`; `[intro.multithread]` i
`[intro.races]` (data race je UB). *Effective Modern C++* **Item 37**
(neka `std::thread` bude unjoinable na svim putanjama). Core Guidelines
**CP.2** (izbegavaj data race), **CP.20** (RAII, nikad čist
`lock()`/`unlock()`), **CP.21** (`std::lock`/`scoped_lock` za više
mutex-a), **CP.23** (nit kao kontejner sa opsegom -- join), **CP.26**
(ne `detach`), **CP.31** (malo podataka između niti prosleđuj po
vrednosti).

**Kako vežbati:**

```
./build.sh 8-concurrency/39-threads/main.cpp                      # ISPRAVNI slučajevi, ASan+UBSan
./build.sh 8-concurrency/39-threads/main.cpp --tsan               # isto, pod ThreadSanitizer-om
./check_cases.sh 8-concurrency/39-threads                         # svi POGREŠNI slučajevi
./check_exercises.sh 8-concurrency/39-threads                     # vežbe
```

- `errors/` (e01–e05): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): data race i redosled zaključavanja (u01, u02) hvata
  samo **ThreadSanitizer** (`// SANITIZER: thread` u zaglavlju fajla,
  `--tsan` u `build.sh`); ASan ih ne vidi, jer je memorija ispravna. TSan
  i ASan ne mogu u isti build. `u03` (detach) hvata ASan.
- `runtime/` (r01–r03): `std::terminate` -- nit bez `join`, izuzetak iz
  niti, dupli `join`.
- TSan postoji na Linux-u (g++ i clang) i macOS-u (clang), ne i za MinGW
  (Windows): tamo `ub/u01`, `u02` i demonstraciju u zadatku ex2 pokreni u
  WSL-u.

---

# 1. Osnove (kurs 189)

- **Konkurentnost**: više poslova napreduje u isto vreme (i na jednom
  jezgru, smenjivanjem). **Paralelizam**: stvarno istovremeno, na više
  jezgara. `std::thread::hardware_concurrency()` daje broj niti koje
  hardver izvršava istovremeno -- ili 0 ako se ne zna.
- Sve niti procesa dele **memoriju** (globalne promenljive, heap, i tuđi
  stek, preko pokazivača); svaka ima **svoj stek** i registre.
- Zašto: da se ne čeka (U/I, mreža, senzor), i da se iskoriste jezgra.
  Cena: deljeni podaci moraju da se štite, a greške zavise od
  raspoređivača, pa se teško ponavljaju.
- ⚠️ Redosled izvršavanja niti **nije određen**. Zato niti u `main.cpp`
  ne pišu na `cout`: upisuju u promenljive, a `main` ispiše posle
  `join()`.

---

# 2. Pravljenje niti (kurs 190)

```cpp
std::thread t1(funkcija);
std::thread t2([&x] { x = 2; });            // lambda
std::thread t3(Funktor{&y});                // funkcijski objekat
std::thread t4(&Senzor::citaj, &s, 4);      // metoda: objekat, pa argumenti
t1.join();                                  // čeka kraj niti
```

- Nit počinje da radi **odmah** u konstruktoru.
- Pre kraja života objekta mora `join()` (čekaj) ili `detach()` (pusti je
  da radi sama). ❌ Inače destruktor pozove `std::terminate`
  (`runtime/r01`; EMC Item 37).
  - ⚠️ Izuzetak između pravljenja niti i `join()` preskoči `join` --
    zato RAII čuvar koji radi `join` u destruktoru (zadatak ex1), ili
    C++20 `std::jthread`, koji to radi sam.
- `joinable()`: `true` dok je objekat vezan za nit (test: pre `join`
  `true`, posle `false`). ❌ Drugi `join` baca `std::system_error`
  (`runtime/r03`).
- ❌ Metoda bez objekta se ne kompajlira (`errors/e05`).
- `std::thread` je **move-only** (kao `unique_ptr`): jedna nit, jedan
  vlasnik (`errors/e02`). Test: posle `b = std::move(a)` je
  `a.joinable() == false`. Zato može u `std::vector<std::thread>` preko
  `emplace_back`.
- ⚠️ `detach()`: niko više ne čeka nit, a ona nastavlja i kad pozivalac
  završi. Ako koristi reference na njegove lokalne promenljive, čita
  mrtav stek (`ub/u03`, ASan: `stack-use-after-return`). CP.26: ne
  koristi `detach` osim ako nit ne koristi ništa što pripada pozivaocu.

---

# 3. Prosleđivanje argumenata (kurs 191)

Konstruktor `std::thread`-a **kopira** (ili premešta) sve argumente u
prostor nove niti i funkciji ih predaje kao rvalue -- isto pravilo kao
`std::bind` (lekcija 31, sekcija 4).

- ❌ Parametar `int&` sa običnim argumentom se ne kompajlira: rvalue se ne
  veže za `int&` (`errors/e01`). ✅ `std::ref(brojac)` kaže "prosledi
  referencu" (test: `brojac` postane 1).
- ⚠️ Parametar `const int&` se kompajlira, ali vidi **kopiju** (test:
  `&x == &brojac` je `false`). Tiho, bez greške.
- ✅ Move-only argument: `std::thread t(preuzmi, std::move(p), ...)` --
  posle toga je `p` prazan (test).
- ⚠️ `std::ref` i `[&]` znače: original mora da živi dok nit radi. Sa
  `join()` pre izlaska iz opsega to je ispunjeno; sa `detach()` obično
  nije (`ub/u03`).
- ✅ CP.31: malo podataka prosledi po vrednosti -- kopija ne može da
  "istekne".

---

# 4. Vraćanje rezultata iz niti (kurs 192)

Povratna vrednost funkcije niti se **odbacuje**. Rezultat se vraća kroz
parametar-referencu (`std::ref`), capture po referenci ili član
funkcijskog objekta.

```cpp
std::vector<long> delovi(4);                            // svaka nit SVOJ element
niti.emplace_back(zbirDela, std::cref(v), od, doKraja, std::ref(delovi[i]));
for (auto& t : niti) t.join();                          // tek posle ovoga čitaj delovi
```

- `join()` je tačka sinhronizacije: sve što je nit upisala vidljivo je
  niti koja je uradila `join` ([thread.thread.member]: kraj niti
  "synchronizes with" povratak iz `join`). Čitanje pre `join`-a je data
  race.
- Ako svaka nit piše **samo u svoj element**, nema deljenja i ne treba
  mutex (test: delovi `31375 93875 156375 218875`, ukupno `500500`;
  zadatak ex1).
- Ovo je nezgrapno za jednu vrednost, a nit ne može ovako da prenese
  izuzetak (`runtime/r02`). Bolje: `std::async` i `std::future` (lekcija 40).

---

# 5. `std::mutex` (kurs 193)

**Data race**: dve niti pristupaju istoj memoriji, bar jedna piše, a
nema sinhronizacije. To je **UB** ([intro.races]).

- ❌ `++brojac` iz dve niti gubi uvećanja: to su tri koraka (pročitaj,
  dodaj, upiši), pa se niti prepliću (`ub/u01`; bez TSan-a, 5
  pokretanja: 104728 do 179652 umesto 200000; zadatak ex2).
- ✅ `std::mutex`: samo jedna nit može da ga drži. `lock()` čeka dok se ne
  oslobodi, `unlock()` ga pušta, `try_lock()` ne čeka, vrati `false` ako
  je zauzet (test).
- Kritičnu sekciju drži što kraćom -- dok jedna nit drži mutex, ostale
  čekaju.
- ⚠️ Čist `lock()`/`unlock()` je krhak: `throw` ili `return` između njih
  ostavi mutex zaključan zauvek (zadatak ex3). CP.20: uvek RAII
  (sekcija 6).
- ❌ Mutex se ne kopira; klasa sa mutex članom nema podrazumevanu kopiju
  (`errors/e03`).
- Zaštita pripada **podacima**: mutex i ono što čuva stavi u istu klasu,
  i svaki pristup (i čitanje) ide pod njim (zadaci ex1, ex2). Metoda koja
  samo čita je `const`, pa mutex mora biti `mutable`.

---

# 6. `std::lock_guard` i srodni (kurs 194)

```cpp
std::lock_guard<std::mutex> g(m);    // lock u konstruktoru, unlock u destruktoru
std::scoped_lock l(a.m, b.m);        // C++17: više mutex-a odjednom, bez deadlock-a
std::unique_lock<std::mutex> ul(m);  // ume i ul.unlock(), ul.lock(), odloženo zaključavanje
```

- `lock_guard` je RAII (lekcija 21): otključa na svakom izlazu iz opsega, i
  kad se baci izuzetak (zadatak ex3). Nema `unlock()` (`errors/e04`) --
  za kraće držanje napravi manji opseg `{ ... }` ili koristi
  `unique_lock` (test: posle `ul.unlock()` je `owns_lock() == false`).
- ⚠️ **Deadlock**: nit 1 drži `a` i čeka `b`, nit 2 drži `b` i čeka `a`.
  Uzrok je suprotan redosled zaključavanja (`ub/u02`: TSan prijavi
  `lock-order-inversion` čak i kad se program slučajno završi).
- ✅ Dva rešenja (CP.21): uvek isti redosled, ili `std::scoped_lock` sa
  svim mutex-ima odjednom -- zaključa ih bez deadlock-a bez obzira na
  redosled argumenata (test: 1000 prenosa u oba smera, stanja `100 100`).

---

# 7. Metode `std::thread`-a i `std::this_thread` (kurs 195)

| | Šta radi |
|---|---|
| `t.get_id()` | id niti; posle `join`/`detach` je `std::thread::id{}` ("nije nit") |
| `std::this_thread::get_id()` | id niti koja se izvršava |
| `std::this_thread::sleep_for(d)` / `sleep_until(t)` | spava **bar** toliko |
| `std::this_thread::yield()` | savet raspoređivaču da pusti druge niti |
| `t.native_handle()` | handle operativnog sistema (`pthread_t` na Linux-u), za ono što standard nema (prioritet, afinitet) |
| `std::thread::hardware_concurrency()` | broj hardverskih niti, ili 0 |
| `t.swap(u)`, `std::swap(t, u)` | zamena vlasništva |

- Test: id viđen iznutra (`this_thread::get_id()`) jednak je `t.get_id()`
  spolja, a različit od id-a `main`-a.
- ⚠️ `sleep_for` garantuje samo **minimum** (test: izmereno vreme sa
  `steady_clock` je bar 20 ms). ❌ Nije način sinhronizacije: "sačekaj
  50 ms, nit je sigurno gotova" je data race koji obično prođe.
  Sinhronizacija je `join`, mutex, `future` (lekcija 40).

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 189 | Concurrency Basics | sekcija 1 |
| 190 | Thread Creation (std::thread) | sekcija 2; `errors/e02`, `e05`; `ub/u03`; `runtime/r01`, `r03`; zadatak ex1 |
| 191 | Passing Arguments To Threads | sekcija 3; `errors/e01` |
| 192 | Returning Value from Threads | sekcija 4; `runtime/r02`; zadatak ex1 |
| 193 | Thread Synchronization (std::mutex) | sekcija 5; `errors/e03`; `ub/u01`; zadatak ex2 |
| 194 | std::lock_guard | sekcija 6; `errors/e04`; `ub/u02`; zadatak ex3 |
| 195 | std::thread Functions & std::this_thread Namespace | sekcija 7 |

---

# Pravilo za praksu

✅ Svaka nit se `join`-uje na **svakoj** putanji -- RAII čuvar ili C++20
`std::jthread` (EMC Item 37, CP.23). `detach` samo kad nit ne koristi
ništa od pozivaoca (CP.26).

✅ Argumenti se kopiraju: `std::ref` samo svesno, i original mora da živi
dok nit radi.

✅ Najbolji deljeni podatak je onaj koji ne postoji: svaka nit svoj deo,
rezultat posle `join`-a.

✅ Deljeni podaci pod mutex-om, uvek preko `lock_guard`/`scoped_lock`,
nikad čist `lock()`/`unlock()` (CP.20).

⚠️ Više mutex-a: isti redosled svuda, ili `std::scoped_lock` (CP.21).

⚠️ Program sa nitima proveri i sa `--tsan`: data race obično "radi" na
testu, a ASan ga ne vidi.

**Rezime:** `std::thread` pokreće funkciju u novoj niti, kopira joj
argumente i mora se `join`-ovati. Niti dele memoriju, pa svaki podatak
koji više niti menja treba mutex, zaključan preko RAII omotača. Rezultat
iz niti se ovde vraća ručno, kroz reference; `std::async` i `future` u lekciji 40
rade to čistije, zajedno sa izuzecima.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_paralelna_obrada`](exercises/ex1_paralelna_obrada.cpp) | usage | podela posla na niti, bezbedan deljeni dnevnik, join u destruktoru (sekcije 2, 4, 6; runtime/r01) | — |
| [`ex2_izgubljena_uvecanja`](exercises/ex2_izgubljena_uvecanja.cpp) | why | zašto ++ nad deljenom promenljivom treba mutex (sekcije 5, 6; ub/u01) | `-DNAIVNO` |
| [`ex3_izuzetak_drzi_mutex`](exercises/ex3_izuzetak_drzi_mutex.cpp) | why | zašto lock_guard, a ne lock()/unlock() (sekcija 6) | `-DNAIVNO` |

## Zapažanja posle vežbe
