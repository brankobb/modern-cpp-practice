# Lekcija 19 — Životni vek objekta

Svaki objekat ima trenutak kad počinje da živi (konstruktor je završen) i
trenutak kad prestaje (počinje destruktor). Ta dva trenutka određuju šta je
bezbedno raditi sa objektom. Ova lekcija pokazuje **kada** tačno nastaju i
nestaju objekti svake vrste. Na tome počivaju kopiranje (lekcija 20), RAII (lekcija 21)
i move (lekcija 22).

**Izvori:** standard, delovi `[basic.stc]` (trajanje skladišta),
`[basic.life]` (životni vek), `[class.temporary]` (privremeni objekti),
`[class.base.init]` (redosled inicijalizacije), `[class.dtor]`,
`[stmt.dcl]` (static lokalni) i `[basic.start.term]` (uništavanje posle
`main`). Uz to *Effective C++* **Item 4** (static objekti i redosled) i
**Item 13** (resurse drže objekti).

**Kako vežbati:**

```
./build.sh 3-zivotni-vek-i-resursi/19-zivotni-vek-objekta/main.cpp
./check_cases.sh 3-zivotni-vek-i-resursi/19-zivotni-vek-objekta
```

- `ub/` (u01–u03): kod koji se kompajlira, a ASan ga hvata pri pokretanju.
  Greške životnog veka se skoro nikad ne vide pri kompajliranju, pa ova
  lekcija nema `errors/`.
- Srodno: lekcija 04 (reference i privremeni objekti, dangling), 08
  (redosled inicijalizacije globalnih između fajlova), 13 (dinamička
  memorija), 14 i 16 (redosled u klasi i hijerarhiji).

---

# 1. Trajanje skladišta

| Trajanje | Primer | Počinje | Završava |
|---|---|---|---|
| **automatic** | lokalna promenljiva, parametar | kad izvršavanje stigne do deklaracije | na kraju bloka `}` |
| **static** (globalna, `static` član) | `Tracer global("global");` | pre `main` | posle `main`, obrnutim redom |
| **static lokalna** | `static Tracer owner(...)` u funkciji | pri **prvom** prolazu kroz deklaraciju | posle `main` |
| **dynamic** | `new`, `make_unique` | na `new` | na `delete` (ili kad ga vlasnik obriše) |
| **thread** | `thread_local` | pri pokretanju niti | na kraju niti |

Iz ispisa: `global()` se pojavi pre prve linije `main`-a, a
`~static-lokalni() ~global()` posle poslednje.

**`static` lokalna promenljiva:**

- Pravi se **jednom**, pri prvom pozivu. Drugi poziv ne ispisuje ništa.
- Od C++11 je inicijalizacija **thread-safe**: ako dve niti istovremeno
  prvi put pozovu funkciju, jedna čeka drugu (`[stmt.dcl]`).
- Na tome počiva Meyers singleton (lekcija 08, sekcija 7).

---

# 2. Redosled

```
a() b() | ~b() ~a()                    // blok: obrnutim redom
x0() x1() x2() | ~x2() ~x1() ~x0()     // niz: od poslednjeg elementa
Engine-baza() pump() filter() Engine-telo | ~Engine-telo ~filter() ~pump() ~Engine-baza()
```

Pravilo je uvek isto: **uništava se obrnutim redom od pravljenja**. Tako
objekat koji je napravljen kasnije (i možda zavisi od ranijeg) nestaje
pre njega.

U klasi: baza, pa članovi **redom deklaracije** (ne redom u init listi,
lekcija 14), pa telo konstruktora. Destrukcija obrnuto.

---

# 3. Privremeni objekti

```cpp
std::cout << Tracer("temp").name().size();   // temp() 4 ~temp()  -- živi do ;
const Tracer& ref = makeTracer("produžen");   // živi do kraja scope-a reference
```

- Privremeni objekat živi do kraja **celog izraza** (do `;`), ne samo do
  kraja podizraza. Zato je `Tracer("temp").name()` bezbedno u istoj liniji.
- **Produženje života:** kad se privremeni **direktno** veže za `const T&`
  ili `T&&`, živi koliko i referenca. Ne važi kroz povratnu vrednost
  funkcije ni za člana-referencu (lekcija 04, sekcija 8; lekcija 14,
  `ub/u01`).
- C++17: `return Tracer(name);` i `Tracer arr[] = {Tracer("x0"), ...}` ne
  prave kopije (garantovan copy elision, lekcija 24). Zato `Tracer` u
  `main.cpp` može da ima obrisan copy konstruktor.

---

# 4. Izuzetak u konstruktoru

```
prvi() drugi() telo-baca ~drugi() ~prvi() | uhvaćen
```

- Objekat počinje da živi tek kad se konstruktor **završi**. Ako
  konstruktor baci izuzetak, objekat nikad nije postojao, pa se njegov
  destruktor **ne poziva**.
- Ali **već napravljeni članovi i baze** se uništavaju, obrnutim redom.
- Posledica: resurs zauzet u telu konstruktora sirovim pokazivačem
  (`data_ = new int[n];` pa nešto baci) curi, jer ga niko ne oslobodi.
  Resurs mora da drži **član** koji ima svoj destruktor (`std::vector`,
  `std::unique_ptr`). To je RAII (lekcija 21).

---

# 5. Kraj programa

| Kako se program završi | Lokalni objekti | `static` objekti |
|---|---|---|
| `return` iz `main` | ✅ uništeni | ✅ uništeni, obrnutim redom |
| `std::exit(0)` | ❌ **ne** uništavaju se | ✅ uništeni |
| `std::quick_exit`, `std::abort`, pad programa | ❌ | ❌ |
| izuzetak koji niko ne uhvati | nije određeno da li | ❌ (`std::terminate`) |

Test (`main.cpp`, sekcija 6): posle `std::exit(0)` se ispiše
`~static-lokalni() ~global()`, a `~lokalni-pre-exit()` nikad. Ako lokalni
objekat drži npr. otvoren fajl sa baferom, taj bafer se ne upiše.

⚠️ **Redosled uništavanja static objekata** je obrnut od redosleda
**završetka konstrukcije**. `static` lokalni objekat napravljen tek u
`main` se uništava **pre** globalnog objekta napravljenog pre `main`. Ako
destruktor globalnog objekta koristi takav static, koristi uništen objekat
(`ub/u03`).

---

# 6. Posle kraja života

Kad objekat prestane da živi, pokazivači i reference na njega i dalje
postoje i sadrže adresu, ali svaka upotreba je UB:

| Situacija | Primer | Šta ASan prijavi |
|---|---|---|
| lokalna posle bloka | `p = &value;` pa `}` pa `*p` | `stack-use-after-scope` (`ub/u01`) |
| destruktor pozvan dvaput | `name.~basic_string();` pa kraj bloka | `attempting double-free` (`ub/u02`) |
| static uništen pre korisnika | globalni dtor koristi kasnije napravljen static | `heap-use-after-free` (`ub/u03`) |
| objekat posle `delete` | lekcija 04, `ub/u07` | `heap-use-after-free` |
| referenca na lokalnu iz funkcije | lekcija 04, `ub/u04` | |

---

# Pravilo za praksu

✅ Objekat drži resurs, a destruktor ga oslobađa. Nikad "zauzmi u
konstruktoru pa se nadaj".

✅ Pokazivač ili referenca ne sme da živi duže od objekta. Kad nisi
siguran, čuvaj vrednost, ne adresu.

✅ Destruktor ručno samo posle placement new, i tačno jednom.

⚠️ `std::exit` ne uništava lokalne objekte. Iz `main` se izlazi sa
`return`.

⚠️ Destruktor static objekta ne treba da koristi druge static objekte.

**Rezime:** svaki objekat živi od kraja konstruktora do početka
destruktora, a jezik garantuje da se uništava obrnutim redom od
pravljenja: u bloku, u nizu, u klasi i posle `main`. Kad konstruktor ne
uspe, uništi se samo ono što je već napravljeno. Sve greške u ovoj lekciji
su isti problem: adresa objekta nadživi sam objekat.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_redosled_traga`](exercises/z1_redosled_traga.cpp) | upotreba | redosled pravljenja i uništavanja (sekcije 2, 3) | — |
| [`z2_izuzetak_u_konstruktoru`](exercises/z2_izuzetak_u_konstruktoru.cpp) | zašto | zašto destruktor ne čisti za konstruktorom koji je bacio (sekcija 4) | `-DNAIVNO` |
| [`z3_kraj_programa`](exercises/z3_kraj_programa.cpp) | zašto | zašto je redosled uništavanja static objekata bitan (sekcija 5) | `-DNAIVNO` |

## Zapažanja posle vežbe

