# Korak 1 — Tranzicija sa C na C++ (temelj)

## Cilj koraka

Da prestaneš da razmišljaš kao C programer koji piše C++ sintaksu i da
počneš da razmišljaš u C++ modelu:

> **resurs je objekat, objekat ima životni ciklus, životni ciklus je vezan za scope.**

Ako ovaj korak preskočiš, sve kasnije (move, RAII, pametni pokazivači)
izgleda kao magija. Ako ga savladaš, sve kasnije je logična posledica.

| Fajl | Šta je |
|---|---|
| `notes.md` | ovaj tekst: teme, šta moraš da razumeš, zadatak |
| `demos.cpp` | kratak primer za svaku temu (sekcija `// ----- N` = tema N ispod), sa blokom EXPECTED OUTPUT |
| `task/raii_file.cpp` | **zadatak**: kostur koji se kompajlira, ti pišeš klasu `File` |
| `solutions/raii_file.cpp` | rešenje (otvori tek kad ti radi) |
| `solutions/cleanup_goto.c` | bonus: isti posao u C-u sa `goto cleanup` |

```
./build.sh roadmap/step-1-c-to-cpp/demos.cpp
./build.sh roadmap/step-1-c-to-cpp/task/raii_file.cpp
```

---

## Šta učiš

### 1. RAII — najvažniji koncept u C++-u

- Resurs (memorija, fajl, socket, mutex, periferija/registar) se **dobija u
  konstruktoru** i **oslobađa u destruktoru**.
- Destruktor se poziva **automatski** kad objekat izađe iz scope-a: na
  kraju bloka, na `return`, `break`, `continue`, `goto` van bloka, i tokom
  stack unwinding-a kad leti izuzetak.
- Posledica: nema ručnog cleanup-a, nema `goto cleanup`, nema
  `if (err) { free(x); return -1; }` na 20 mesta.

```cpp
int early_return(bool fail) {
    Tracer a{"a"};
    Tracer b{"b"};
    if (fail) return -1;   // poziva se ~b(), pa ~a() -- obrnuto od konstrukcije
    return 0;              // isto
}
```

**Moraš da razumeš:** RAII nije "pattern" koji neko izmislio, nego
posledica osnovnog pravila jezika: *destruktor automatskog objekta se
poziva deterministički, u tačno definisanom trenutku*. To je razlika
između C++-a i jezika sa garbage collector-om (Java, C#, Go), gde ne znaš
**kad** će se resurs osloboditi. U embedded-u, gde je mutex ili DMA kanal
ograničen resurs, to je jedino prihvatljivo.

Dve stvari koje iznenade C programera:
- ako **konstruktor baci** izuzetak, objekat nikad nije postojao, pa se
  **njegov destruktor ne poziva** (ali se pozivaju destruktori članova koji
  su već bili konstruisani). Zato konstruktor ne čisti ništa što nije
  dobio;
- destruktor **ne sme da baci** izuzetak (podrazumevano je `noexcept`):
  ako baci tokom unwinding-a, program ide u `std::terminate`.

Dublje: `3-lifetime-and-resources/21-raii/notes.md` (sekcije 1–3) i
`3-lifetime-and-resources/19-object-lifetime/notes.md` (sekcija 2).

### 2. Reference vs pokazivači

| | `T&` | `T*` |
|---|---|---|
| može da bude null | ne | da |
| može da se preusmeri na drugi objekat | **ne** (`r = x;` upisuje u objekat) | da |
| mora da se inicijalizuje | da | ne (ali treba) |
| aritmetika | ne | da |
| značenje u API-ju | "objekat sigurno postoji" | "možda ga nema" ili "niz" |

```cpp
void inc_ref(int& x)  { ++x; }                       // nema provere: ne može null
void inc_ptr(int* x)  { if (x != nullptr) ++*x; }    // mora provera
void print(const Config& c);   // ne kopira, ne menja, ne može null
```

**Moraš da razumeš:**
- `const T&` je podrazumevani način da primiš "veći" objekat koji samo
  čitaš: nema kopije, nema null-a, nema izmene. Za male tipove (`int`,
  `double`, mali `struct`) prosleđuj po vrednosti.
- `T*` u parametru znači "može da bude `nullptr`" — i onda **moraš** da
  proveriš.
- **Vraćanje reference na lokalni objekat = dangling referenca** (UB).
  Lokalni objekat umire na `}`, referenca ostaje da pokazuje u prazno.
  Isto važi za pokazivač na lokalni objekat (to već znaš iz C-a).
- `int& r = n; r = other;` **ne** prevezuje `r` na `other`, nego upisuje
  vrednost `other` u `n`. Referenca je drugo ime za `n` zauvek.

Dublje: `1-language-basics/04-pointers-and-references/notes.md` (sekcije 7–11).

### 3. `nullptr`, `enum class`, `constexpr`

- `nullptr` je tipa `std::nullptr_t` i **ne** konvertuje se u `int`. `NULL`
  je često samo `0` (ili `0L`), pa `f(NULL)` može da pozove `f(int)` umesto
  `f(int*)`.
- `enum class` je **scoped** (`Led::Off`, `Motor::Off` — nema kolizije) i
  **ne konvertuje se implicitno** u `int` (`int i = Led::Red;` se ne
  kompajlira). Podloga može da se zada: `enum class Led : std::uint8_t`.
- `constexpr` promenljiva je **poznata pri kompajliranju**; `constexpr`
  funkcija **može** da se izvrši pri kompajliranju (ako su argumenti
  konstante), a može i u runtime-u.

```cpp
constexpr std::uint32_t divisor(std::uint32_t clock, std::uint32_t baud) { return clock / (16 * baud); }
static_assert(divisor(16'000'000, 115200) == 8, "pogrešan delilac za UART");
```

**Moraš da razumeš:** `constexpr` nije `const`.
- `const` = "ne menjam kroz ovo ime". Vrednost može da bude poznata tek u
  runtime-u (`const int n = read_sensor();`).
- `constexpr` = "vrednost je poznata pri kompajliranju" (i zato je i `const`).

U embedded-u `constexpr` zamenjuje `#define` za konstante: ima tip, ima
scope, vidi ga debugger, a uz `static_assert` dobijaš proveru konfiguracije
(delilac takta, veličina bafera stepen dvojke...) **pre** nego što flešuješ.

Dublje: `1-language-basics/05-compound-types/notes.md` (sekcija 4),
`1-language-basics/12-constexpr/notes.md` (sekcije 1, 4 i 6),
`1-language-basics/09-const/notes.md` (sekcija 9).

### 4. `const` korektnost

```cpp
class Counter {
public:
    int value() const { return value_; }   // ne menja objekat -> sme na const objektu
    void increment() { ++value_; }         // menja -> ne sme na const objektu
};
void print_counter(const Counter& c) { c.value(); /* c.increment(); -- greška */ }
```

- `const` metoda: `int size() const;` — unutra je `this` tipa `const T*`.
- `const` parametar: `void f(const T& x);`
- pokazivači — čitaj **s desna na levo**:
  - `const T* p` — pokazivač na const `T`: `*p` ne menjaš, `p` možeš;
  - `T* const p` — const pokazivač: `p` ne menjaš, `*p` možeš;
  - `const T* const p` — ni jedno ni drugo.

**Moraš da razumeš:** `const` je **ugovor** i dokumentacija, ne ukras. Ako
metoda nije `const`, ne možeš je pozvati preko `const T&` — a `const T&` je
kako će svi prosleđivati tvoj objekat. Zato: **svaka metoda koja ne menja
objekat mora da bude `const`**, od prvog dana. Naknadno dodavanje `const`-a
se lančano širi kroz ceo kod.

Dublje: `1-language-basics/09-const/notes.md` (sekcije 2, 3 i 4).

### 5. Preopterećenje funkcija i operatora (osnove)

- Isto ime, različiti parametri → **overload**: `show(int)`, `show(double)`,
  `show(Vec2)`.
- Operator je samo funkcija sa posebnim imenom: `Vec2 operator+(Vec2, Vec2)`,
  `bool operator==(...)`, `operator[]`, `operator()`, `operator<<`.
- `operator=` (dodela) **nije** konstruktor: konstruktor pravi novi objekat,
  `operator=` menja postojeći (vidi temu 8).

**Moraš da razumeš:** overload se razrešava **pri kompajliranju**, po
**statičkim tipovima** argumenata. Nema nikakve odluke u runtime-u i nema
cene. Runtime izbor funkcije je `virtual` (Korak 2).

Dublje: `1-language-basics/11-functions-advanced/notes.md` (sekcije 1 i 2),
`2-classes/15-operator-overloading/notes.md` (sekcije 1 i 2).

### 6. Imenski prostori

```cpp
namespace uart { void init(); }
namespace spi  { void init(); }     // isto ime, nema kolizije
uart::init();
using uart::init;                   // using-DEKLARACIJA: uvodi samo jedno ime
using namespace std;                // using-DIREKTIVA: uvodi SVA imena -- izbegavaj
namespace { int helper_calls = 0; } // anonimni namespace = "static" u C-u (internal linkage)
```

**Moraš da razumeš:** `using namespace std;` u **header-u** je zlo — svako
ko uključi tvoj header dobija sva `std` imena, bez izbora, i nove kolizije
kad standard doda novo ime. U `.cpp` fajlu je tolerisano (lokalno je), ali
mnogi embedded standardi (MISRA, AUTOSAR) i tu zabranjuju. Piši `std::`.

Dublje: `1-language-basics/08-namespaces-and-linkage/notes.md` (sekcije 1, 2 i 4).

### 7. `struct` vs `class`

- Jezički: **ista stvar**, jedina razlika je podrazumevani pristup
  (`struct` → `public`, `class` → `private`, i isto za nasleđivanje).
- Konvencija (Core Guidelines C.2): `struct` za pasivne podatke bez
  invarijante (`Point`, konfiguracija, poruka); `class` kad postoji
  **invarijanta** — pravilo koje mora uvek da važi (`Percent` je 0..100,
  `File` uvek drži otvoren fajl) — pa su podaci privatni i menjaju se samo
  kroz metode koje čuvaju to pravilo.

Dublje: `2-classes/14-class-basics/notes.md` (sekcija 1).

### 8. Inicijalizacija

| Zapis | Šta je | Za `int` |
|---|---|---|
| `T x;` | default-init | **neodređena vrednost** (čitanje = UB) |
| `T x{};` | value-init | `0` |
| `T x{a};` | direct-list-init | zabranjuje narrowing (`int x{3.5};` je greška) |
| `T x(a);` | direct-init | dozvoljava narrowing |
| `T x = a;` | copy-init | poziva (copy/converting) **konstruktor**, ne `operator=` |
| `T x();` | **deklaracija funkcije** (most vexing parse) | — |

**Default izbor: `T x{};` / `T x{...};`.**

**Moraš da razumeš:** `T x = y;` je **inicijalizacija** — poziva
konstruktor. `x = y;` na već postojećem `x` je **dodela** — poziva
`operator=`. Znak `=` u deklaraciji nije dodela.

Dublje: `1-language-basics/03-initialization/notes.md` (sekcije 1–6 i 16).

---

## Šta MORAŠ da razumeš (suština koraka)

1. **Destruktor je deterministički.** Poziva se tačno kad objekat izađe iz
   scope-a, u obrnutom redosledu od konstrukcije. To je osnova svega.
2. **Resurs = objekat.** Ne postoji "resurs" odvojeno od objekta koji ga
   drži. Ako vidiš goli `FILE*`/`malloc` koji neko "treba da oslobodi" —
   to je C u C++ sintaksi.
3. **Kopiranje je podrazumevano.** Ako ne kažeš drugačije, C++ kopira
   (prosleđivanje po vrednosti, `T b = a;`, vraćanje). U C-u je kopiranje
   strukture plitko i "očigledno"; u C++-u kopija klase poziva **tvoj**
   kod — ili kod koji kompajler sam napiše, a koji za resurse **greši**.
4. **`const` je ugovor, ne ukras.**
5. **Inicijalizacija nije dodela.** `T x = y;` poziva konstruktor, ne `operator=`.

---

## Zadatak: "RAII wrapper za C resurs"

Resurs: `FILE*` (svi ga znaju; posle možeš isto za `int fd` ili `malloc`).
Kostur: `task/raii_file.cpp`. Koraci su u komentaru na vrhu fajla;
`main()` je već napisan, zakomentarisan po koracima.

**Zahtevi:**

1. Konstruktor otvara fajl (`fopen`). Ako ne uspe — baca
   `std::runtime_error`. Objekat `File` sa `nullptr` unutra **ne sme da
   postoji** (to je invarijanta klase).
2. Destruktor zatvara (`fclose`) i ispisuje poruku.
3. Kopiranje zabranjeno: `File(const File&) = delete;` i
   `File& operator=(const File&) = delete;` — dva objekta ne smeju da drže
   isti `FILE*` (oba bi zvala `fclose` → double close).
4. Move za sada takođe `= delete` (dodajemo ga u Koraku 3).
   Napomena: kad obrišeš copy, kompajler **ne generiše** move, pa bi tip
   bio nepomerljiv i bez eksplicitnog `= delete` za move — ali napiši ga,
   da namera bude vidljiva.
5. `std::FILE* get() const noexcept` za pristup resursu (da možeš da
   pozoveš bilo koju C funkciju: `fgets(buf, n, f.get())`).
6. `main()` (već napisan u kosturu) pokazuje:
   - normalan izlaz iz scope-a,
   - **rani `return`** (dva puta, nijedan `fclose`),
   - **izuzetak** posred rada sa dva fajla → zatvaraju se obrnutim redom,
     **pre** nego što `catch` blok počne,
   - neuspeh konstruktora → destruktor se **ne** poziva,
   - brojač `File::open_count()` na kraju je `0`: dokaz da ništa nije
     procurelo (ne veruj samo printf-u).

Pokreni sa `-DTRY_COPY` i pročitaj grešku kompajlera — to je `= delete` na delu.

**Zašto ovaj zadatak:** pokriva RAII, `const` korektnost, `= delete`,
destruktor i scope, i pokazuje zašto je C++ bezbedniji od C-a **bez
ikakvog overhead-a** — kompajler na svaki izlaz ubaci isti `fclose` koji bi
u C-u pisao ručno.

**Bonus:** `solutions/cleanup_goto.c` — isti `copy_and_fail` u C-u sa
`goto cleanup`. Napiši ga prvo sam, pa uporedi. Prebroj: koliko mesta u C
verziji mora da bude tačno da ne bi bilo curenja? Koliko u C++ verziji?

```
gcc -std=c11 -Wall -Wextra -g -fsanitize=address,undefined \
    roadmap/step-1-c-to-cpp/solutions/cleanup_goto.c -o /tmp/cleanup && /tmp/cleanup
```

## Provera pre Koraka 2

Odgovori bez gledanja:

1. U kom redosledu se pozivaju destruktori tri lokalna objekta? Šta ako
   izuzetak izleti posle drugog?
2. Zašto se destruktor `File`-a ne poziva kad `fopen` u konstruktoru ne uspe?
3. Zašto je `get()` `const`, a `write_line()` nije? Može li se kroz `get()`
   ipak pisati u fajl? Šta onda `const` zapravo štiti?
4. Šta bi se desilo da **nisi** obrisao copy konstruktor, a napišeš
   `File f2 = f1;`?
5. `const int n = read();` vs `constexpr int n = 5;` — koji može u `static_assert`?
6. Zašto `int x;` u funkciji, pa `printf("%d", x);` nije "samo smeće", nego UB?

## Zapažanja posle vežbe

<!-- tvoje beleške -->
