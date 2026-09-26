# Lekcija 27 — Forwarding i zamke životnog veka

Dva teme koje povezuje isto pitanje: **čiji je objekat i koliko živi**.
Prvi deo je kako šablon prosledi argument dalje, a da ne izgubi da li je
bio lvalue ili rvalue (`std::forward`). Drugi deo su mesta gde objekat
nestane, a nešto i dalje pokazuje na njega: `string_view`, iteratori,
lambde.

**Izvori:** standard, delovi `[temp.deduct.call]` (forwarding referenca),
`[dcl.ref]` (reference collapsing), `[forward]`,
`[string.view]` i pravila invalidacije u `[vector.modifiers]` i
`[associative.reqmts]`. Uz to *Effective Modern C++* **Item 24**
(univerzalne vs rvalue reference), **Item 25** (`move` za rvalue,
`forward` za univerzalne), **Item 28** (reference collapsing), **Item 30**
(kad forwarding ne radi), **Item 31** i **Item 32** (lambda capture).

**Kako vežbati:**

```
./build.sh 4-sabloni/27-forwarding-i-zivotni-vek/main.cpp
./check_cases.sh 4-sabloni/27-forwarding-i-zivotni-vek
```

- `errors/` (e01–e04): kod koji se **ne kompajlira**.
- `ub/` (u01–u04): kod koji se kompajlira, a ASan ga hvata.
- Srodno: lekcija 04 (produženje života, `ub/u05` `std::max`, `ub/u06`
  realokacija), 10 (dedukcija, `auto&&`, range-for i privremeni), 11
  (EMC 26), lekcija 22 (move).

---

# 1. Forwarding referenca (EMC Item 24)

`T&&` je **forwarding** (univerzalna) referenca **samo** kad je `T` parametar
šablona koji se **dedukuje** iz tog argumenta, i kad je oblik tačno `T&&`
(ili `auto&&`). Sve ostalo je obična rvalue referenca.

| Deklaracija | Vrsta | lvalue argument |
|---|---|---|
| `template <class T> void f(T&&)` | **forwarding** | ✅ `T = X&` |
| `auto&& x = expr;` | **forwarding** | ✅ |
| `void f(std::string&&)` | rvalue | ❌ |
| `template <class T> void f(std::vector<T>&&)` | rvalue | ❌ (`errors/e02`) |
| `template <class T> void f(const T&&)` | rvalue | ❌ (`errors/e03`) |
| član `void push(T&&)` u `template <class T> class Box` | rvalue (T je već poznat) | ❌ |

Test: `universal(name)` daje `T=std::string&`, `universal(constName)` daje
`T=const std::string&`, a `universal(std::string("x"))` daje `T=std::string`.

---

# 2. Reference collapsing (EMC Item 28)

Kad `T` postane referenca, `T&&` bi bila "referenca na referencu". Jezik
to sažme:

| `T` | `T&&` |
|---|---|
| `X` | `X&&` |
| `X&` | `X&` |
| `X&&` | `X&&` |

Pravilo: **ako je bilo gde `&`, rezultat je `&`**. Tako forwarding
referenca postane `X&` za lvalue, a `X&&` za rvalue. `main.cpp` to
proverava sa `static_assert`.

---

# 3. `std::forward` (EMC Item 25)

Unutar funkcije parametar ima ime, pa je **lvalue** (lekcija 22, sekcija 5).
`std::forward<T>(arg)` vrati kategoriju koju je argument imao na mestu
poziva: rvalue ako je `T` bez reference, lvalue ako je `T` referenca.

```
bez forward: lvalue -> inner(const&), rvalue -> inner(const&)   <- move izgubljen
sa forward:  lvalue -> inner(const&), rvalue -> inner(&&)
```

| Parametar | Kad ga predaješ dalje |
|---|---|
| `T&&` (forwarding) | `std::forward<T>(arg)` |
| `X&&` (rvalue) | `std::move(arg)` |

⚠️ **`std::move` na forwarding referenci** pomera i **lvalue** argumente
pozivaoca. Test: posle `setNameMove(mine)` je `mine.size() = 0`;
pozivalac nije tražio da mu se string isprazni.

**Savršeno prosleđivanje** (variadic): `std::make_unique`,
`emplace_back`, `std::thread` rade ovako:

```cpp
template <typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) { return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }
```

---

# 4. Kad forwarding ne radi (EMC Item 30)

| Argument | Direktan poziv | Kroz šablon |
|---|---|---|
| `{1, 2, 3}` | ✅ | ❌ nema tip, `T` se ne može dedukovati (`errors/e01`) |
| ime preopterećene funkcije | ✅ bira po argumentu | ❌ ime nema jedan tip (`errors/e04`) |
| `0` ili `NULL` kao null pokazivač | ✅ | ⚠️ dedukuje se `int` |
| bit-polje | ✅ | ❌ ne može referenca na bit-polje |

Rešenja: napiši tip (`std::vector<int>{1, 2, 3}`), `static_cast` na
konkretan pokazivač na funkciju ili lambda, `nullptr` umesto `0`.

---

# 5. `string_view` ne poseduje podatke

```cpp
std::string_view word = firstWord(sentence);   // ✅ sentence živi duže
std::string_view literal = "string literal";   // ✅ literal živi ceo program
std::string_view view = makeGreeting("Ana");   // ❌ privremeni string nestaje na ; (ub/u01)
```

- `string_view` je pokazivač + dužina. Produženje života privremenog
  (lekcija 19) važi samo za **reference**, a `string_view` je objekat.
- ✅ Odličan kao **parametar** funkcije (prima `std::string`, literal i
  deo stringa bez kopije).
- ⚠️ Opasan kao **povratna vrednost** ili **član**, osim kad je jasno ko
  drži podatke.
- clang `-Wall` upozori na `ub/u01` (`-Wdangling-gsl`), g++ ne. ASan uhvati
  čitanje `view[0]`, ali ne i `printf("%.*s", ...)`: čitanje unutar libc
  nije prošlo kroz instrumentisan kod (test).

---

# 6. Invalidacija iteratora

| Kontejner | Operacija | Šta postaje nevažeće |
|---|---|---|
| `vector`, `string` | `push_back`/`insert` sa realokacijom | **sve** (iteratori, pokazivači, reference) |
| `vector`, `string` | `insert` bez realokacije | od mesta umetanja do kraja |
| `vector`, `string` | `erase` | od obrisanog elementa do kraja |
| `deque` | `push_back`/`push_front` | iteratori (reference ostaju) |
| `list`, `map`, `set` | `insert` | ništa |
| `list`, `map`, `set` | `erase` | samo obrisani element |

- ❌ `push_back` u range-for nad istim vektorom (`ub/u02`).
- ❌ `v.erase(it)` pa `++it` (`ub/u04`).
- ✅ `it = v.erase(it);`, erase-remove idiom, ili C++20
  `std::erase_if(v, pred)`.
- Test: adresa elementa `std::map` ista posle 98 umetanja; kod `vector`-a
  se menja posle realokacije (lekcija 04, `ub/u06`).

---

# 7. Lambda capture (EMC Item 31, 32)

| Capture | Značenje | Rizik |
|---|---|---|
| `[x]` | kopija u trenutku pravljenja lambde | nema (osim ako je `x` pokazivač) |
| `[&x]` | referenca | ❌ visi ako lambda živi duže od `x` (`ub/u03`) |
| `[=]` | kopija svega korišćenog, **i `this`** kao pokazivač | ⚠️ članovi se čitaju kroz `this`, ne kopiraju |
| `[&]` | referenca na sve | ❌ isto kao `[&x]`, samo manje vidljivo |
| `[p = std::move(ptr)]` (C++14) | init capture: move u lambdu | nema; jedini način za move-only tipove |

✅ Lambda koja se čuva (u `std::function`, u niti, kao callback) hvata
**po vrednosti**. `[&]` samo kad se lambda koristi odmah (npr. u
`std::sort`).

---

# Pravilo za praksu

✅ `std::forward<T>` za `T&&` u šablonu, `std::move` za konkretan `X&&`.

✅ `string_view` kao parametar; kao rezultat ili član samo kad je
vlasnik podataka jasan.

✅ Posle operacije koja menja veličinu vektora, iteratore uzmi ponovo.

✅ Lambda koja živi duže: capture po vrednosti ili init capture.

⚠️ `T&&` nije uvek forwarding referenca: samo golo `T&&` sa dedukcijom.

**Rezime:** forwarding referenca plus `std::forward` omogućavaju šablonu
da prosledi argument dalje kao da ga je pozivalac dao direktno, kopiju ili
move, bez dupliranja funkcija. Druga polovina lekcije je ista greška u
tri oblika: `string_view`, iterator i lambda sa `[&]` ne poseduju ono na
šta pokazuju, pa žive samo dok živi vlasnik.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_savrseno_prosledjivanje`](exercises/z1_savrseno_prosledjivanje.cpp) | upotreba | forwarding reference, std::forward i variadic template (sekcije 1, 3) | — |
| [`z2_move_na_forwarding`](exercises/z2_move_na_forwarding.cpp) | zašto | zašto std::forward, a ne std::move, na forwarding referenci (sekcije 1, 3, EMC Item 25) | `-DNAIVNO` |
| [`z3_lambda_referenca`](exercises/z3_lambda_referenca.cpp) | zašto | zašto [&] u lambdi koja nadživi funkciju visi (sekcija 7, EMC Item 31) | `-DNAIVNO` |

## Zapažanja posle vežbe

