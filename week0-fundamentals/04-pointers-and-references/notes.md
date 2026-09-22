# 04 — Pokazivači i reference

Pokazivači i reference su način da se do objekta dođe **indirektno**. Iz
toga dolazi većina moći C++-a (bez kopiranja, deljenje, polimorfizam), ali
i većina njegovih najopasnijih bagova. Ti bagovi se najčešće **ne vide pri
kompajliranju**, nego tek pri izvršavanju, kao undefined behavior (UB).

**Izvori:** standard, delovi `[dcl.ptr]` (pokazivači), `[dcl.ref]`
(reference), `[dcl.init.ref]` (vezivanje referenci), `[class.temporary]`
(životni vek privremenih), `[expr.add]` (pointer arithmetic), `[conv.ptr]`
(konverzije i null pokazivač) i `[dcl.mptr]` (pokazivač na člana). Uz to
*Effective Modern C++* **Item 8** ("Prefer nullptr to 0 and NULL") i
*Effective C++* **Item 16** (isti oblik `new`/`delete`), **Item 20**
(`const&` umesto vrednosti) i **Item 21** (ne vraćaj referencu kad moraš da
vratiš objekat).

**Kako vežbati:**

```
./build.sh week0-fundamentals/04-pointers-and-references/main.cpp        # svi ISPRAVNI slučajevi
./check_cases.sh week0-fundamentals/04-pointers-and-references           # svi POGREŠNI slučajevi
```

Pogrešni slučajevi su podeljeni u dve grupe, jer se kod pokazivača greške
javljaju na dva mesta:

- `errors/` (e01–e20): kod koji se **ne kompajlira**. `check_cases.sh`
  proverava da pada na g++ i clang++, i to iz razloga opisanog u fajlu.
- `ub/` (u01–u11): kod koji se **kompajlira**, ali je UB. `check_cases.sh`
  ga pokreće sa ASan/UBSan i proverava da sanitizer prijavi tačno taj bag.

Na Windows-u pokreni jedan fajl, npr.
`.\build.ps1 week0-fundamentals\04-pointers-and-references\ub\u07_use_after_free.cpp -Compiler clang++`.
LeakSanitizer (u10) na Windows-u ne postoji.

> **UB nije "program pukne".** UB znači da standard ne kaže ništa o tome šta
> se dešava. Program može da pukne, da ispiše staru vrednost, ili da radi
> ispravno do dana kad promeniš kompajler. Zato se ovakvi bagovi hvataju
> alatima (ASan/UBSan), a ne gledanjem izlaza.

---

# 1. Pokazivač: osnove

Pokazivač je **objekat** koji čuva adresu drugog objekta.

```cpp
int x = 5;
int* p = &x;   // & -- adresa od x
*p = 7;        // * -- pristup objektu na toj adresi; sada je x == 7
```

- Pokazivač ima **sopstvenu** adresu i veličinu (`sizeof(p)` je 8 na 64-bit
  sistemu) i može da se **preusmeri**: `p = &y;`.
- Tipiziran je: `int*` čita i piše `int`. ❌ Nema implicitne konverzije
  između pokazivača na različite tipove: `double* pd = &x;` ne radi
  (`errors/e02`).
- ❌ Adresa se uzima samo od objekta koji je negde smešten (lvalue):
  `int* p = &5;` ne radi (`errors/e01`).

⚠️ Zamka u deklaraciji: `*` pripada **imenu**, ne tipu.

```cpp
int* a, b;     // a je int*, b je OBIČAN int
int *a, *b;    // oba su pokazivači
```

Zato se preporučuje jedna deklaracija po liniji.

---

# 2. Null pokazivač i `nullptr` (EMC Item 8)

```cpp
int* p = nullptr;   // "ne pokazuje ni na šta"
int* q{};           // value-init -> takođe nullptr (lekcija 03)
int* r;             // lokalno: NEODREĐENA adresa ("divlji" pokazivač)
```

- ⚠️ Dereferenciranje null pokazivača je UB (`ub/u01`). Uvek proveri pre
  upotrebe: `if (p) { ... }`.
- ⚠️ Neinicijalizovan pokazivač je gori od null-a: ne može ni da se
  proveri. Sanitizeri ga ne hvataju pouzdano, pa pokazivač **uvek**
  inicijalizuj.

**Zašto `nullptr`, a ne `0` ili `NULL`:** `0` je `int`, a `NULL` je
celobrojna konstanta (`0`, `0L` ili `__null`). Samo `nullptr` ima svoj tip,
`std::nullptr_t`, koji ide **samo** u pokazivače.

```cpp
void f(int);
void f(char*);

f(0);        // f(int)
f(NULL);     // ❌ dvosmisleno -- ne kompajlira se (errors/e07)
f(nullptr);  // f(char*)

int n = nullptr;  // ❌ nullptr ne ide u int (errors/e06) -- i to je dobro
```

EMC Item 8 pokazuje i težu zamku: kad `0` prođe kroz template, dedukuje se
kao `int` i više nije "null pokazivač":

```cpp
template <typename F, typename P> void call(F func, P param) { func(param); }
void g(int*);

call(g, 0);        // ❌ int -> int* ne radi (errors/e08)
call(g, nullptr);  // ✅
```

---

# 3. Pointer arithmetic i nizovi

Niz se u većini izraza pretvara u pokazivač na prvi element
(*array-to-pointer decay*).

```cpp
int arr[5] = {10, 20, 30, 40, 50};
int* p = arr;       // == &arr[0]
*(p + 1);           // 20 -- korak je sizeof(int), ne 1 bajt
p[2];               // 30 -- p[i] je samo drugi zapis za *(p + i)
(arr + 5) - arr;    // 5  -- std::ptrdiff_t, broj ELEMENATA
```

Pravila iz `[expr.add]`:

- ✅ Pokazivač sme da ide od prvog elementa do **jednog iza poslednjeg**
  (`arr + 5`). Taj "one-past-end" pokazivač sme da se napravi i poredi,
  tako rade `end()` iteratori.
- ⚠️ Dereferenciranje `arr + 5` je UB (`ub/u02`, ASan:
  *stack-buffer-overflow*).
- ⚠️ Pravljenje pokazivača dalje od toga (`arr + 6`, `arr - 1`) je UB, čak
  i bez dereferenciranja. Sanitizeri to obično ne hvataju.
- Oduzimanje pokazivača ima smisla samo unutar **istog** niza.
- `<` između pokazivača na **nepovezane** objekte ima nespecificiran
  rezultat (nije UB, ali ne znači ništa). `std::less<T*>` garantuje
  dosledan poredak, pa zato radi kao ključ u `std::map`.

⚠️ **Niz kao parametar funkcije je pokazivač:**

```cpp
void f(int arr[]);   // isto što i void f(int* arr)
sizeof(arr);         // 8 u f -- veličina pokazivača, ne niza (-Wall upozori)
```

Rešenja: prosledi i dužinu, koristi `std::size(arr)` tamo gde je niz još
niz, prosledi referencu na niz (`template <size_t N> f(int (&arr)[N])`),
ili koristi `std::array`, `std::vector` ili `std::span` (C++20).

---

# 4. `void*`

```cpp
int x = 42;
void* vp = &x;                    // ✅ bilo koji T* -> void* je implicitno
int* ip = static_cast<int*>(vp);  // ✅ nazad mora eksplicitno
```

- ❌ `int* ip = vp;` ne radi u C++-u, iako u C-u radi (`errors/e03`).
- ❌ `*vp` ne radi, jer `void` nema tip za čitanje (`errors/e04`).
- ❌ `vp + 1` ne radi, jer `void` nema veličinu (`errors/e05`). g++ to bez
  `-pedantic-errors` pušta kao GNU ekstenziju, a clang ga u C++-u uvek odbija.

U modernom C++ kodu `void*` se retko sreće; umesto njega se koriste
template-i i `std::any`.

---

# 5. Pokazivač na pokazivač

```cpp
int x = 5;
int* p = &x;
int** pp = &p;
**pp = 6;       // x == 6
```

Tipična upotreba je funkcija koja menja **pozivaočev pokazivač**:

```cpp
void allocateOld(int** out) { *out = new int(1); }   // C stil
void allocateRef(int*& out) { out = new int(2); }    // C++: referenca na pokazivač
std::unique_ptr<int> allocateModern();               // najbolje: vrati vlasnika
```

---

# 6. `const` i pokazivači

Čitaj **s desna na levo**:

```cpp
const int* p1;        // pokazivač na const int
int* const p2 = &x;   // const pokazivač na int
const int* const p3;  // const pokazivač na const int
```

| | menjanje `*p` | preusmeravanje `p` |
|---|---|---|
| `const int* p` | ❌ `errors/e09` | ✅ |
| `int* const p` | ✅ | ❌ `errors/e10` |
| `const int* const p` | ❌ | ❌ |

- ✅ Dodavanje `const` je implicitno: `int*` → `const int*`.
- ❌ Skidanje nije: `int* p = &constObj;` ne radi (`errors/e11`).
- `const int* p = &x;` ne čini `x` konstantnim. Samo zabranjuje izmenu
  **kroz** `p`.

Detaljnije: lekcija 07.

---

# 7. Reference: osnove

Referenca je **drugo ime (alias)** za postojeći objekat.

```cpp
int x = 10;
int& r = x;   // r JE x
r = 20;       // x == 20
&r == &x;     // true -- ista adresa
```

Pravila:

- ❌ Mora da se veže **odmah**: `int& r;` ne radi (`errors/e12`).
- **Ne može da se preusmeri.** `r = y;` ne vezuje `r` za `y`, nego kopira
  vrednost `y` u `x`.
- U ispravnom programu nema "null reference". Referenca ipak može da
  **visi** (sekcije 8 i 11).

**Referenca nije objekat** (`[dcl.ref]`), pa ne postoje:

- ❌ niz referenci `int& arr[3]` (`errors/e13`)
- ❌ pokazivač na referencu `int&* p` (`errors/e14`)
- ❌ referenca na referencu `int& & rr` (`errors/e15`)

Postoji referenca **na pokazivač**: `int*& rp = p;`. Kroz alias i template
važi *reference collapsing*: `using R = int&; R& rr = x;` je `int&`. To je
osnova perfect forwarding-a (week2 s09).

---

# 8. Vezivanje referenci i životni vek privremenih

| Referenca | Veže se za | Primer |
|---|---|---|
| `T&` | samo lvalue (imenovani objekat) | `int& r = x;` ✅ &nbsp; `int& r = 5;` ❌ `errors/e16` |
| `const T&` | sve, uključujući privremene | `const int& r = 5;` ✅ |
| `T&&` | samo rvalue (privremeni, `std::move`) | `int&& r = 5;` ✅ &nbsp; `int&& r = x;` ❌ `errors/e17` |

❌ Ni `const` se ne sme izgubiti: `const int c = 1; int& r = c;` ne radi
(`errors/e20`).

**Produženje životnog veka** (`[class.temporary]`): kad se privremeni
objekat veže **direktno** za `const T&` ili `T&&`, živi koliko i referenca.

```cpp
const std::string& s = std::string("privremeni");  // ✅ s je validan do kraja scope-a
std::string&& t = std::string("rvalue");            // ✅ i t može da se menja
```

⚠️ **Produženje NE prolazi kroz funkciju:**

```cpp
const int& r = std::max(1, 2);   // UB: 1 i 2 nestaju na kraju izraza, r visi (ub/u05)
int r = std::max(1, 2);          // ✅ kopija
```

g++ ovde upozori (`-Wdangling-reference`), a ASan prijavi
*stack-use-after-scope*.

⚠️ **Iznenađenje sa konverzijom:**

```cpp
double d = 1.5;
const int& ri = d;   // ✅ ali ri je vezan za PRIVREMENU kopiju int(1), ne za d
d = 2.5;             // ri je i dalje 1
int& ri2 = d;        // ❌ errors/e19
```

---

# 9. Referenca vs pokazivač

| | Pokazivač | Referenca |
|---|---|---|
| Može da bude "prazan" | da (`nullptr`) | ne |
| Može da se preusmeri | da | ne |
| Mora da se inicijalizuje | ne (ali treba) | da |
| Aritmetika | da | ne |
| Sopstveni objekat (adresa, `sizeof`) | da | ne |
| Može da visi | da | da |

**Pravilo:** referenca kad objekat **mora** da postoji, a pokazivač kad
"nema objekta" ima smisla ili kad se preusmerava. Kod polimorfizma i
referenca i pokazivač čuvaju pravi tip objekta, a kopija po vrednosti ga
"odseca" (slicing, lekcija 12).

---

# 10. Prosleđivanje parametara (EC++ Item 20)

| Parametar | Kopira? | Može "prazno"? | Kada |
|---|---|---|---|
| `T` | da | ne | mali tipovi (`int`, iteratori), ili kad ionako praviš kopiju |
| `const T&` | ne | ne | **podrazumevano** za veće objekte koje samo čitaš |
| `T&` | ne | ne | izlazni parametar koji funkcija menja |
| `T*` | ne | da | opcioni parametar: funkcija mora da proveri `nullptr` |

- `const T&` prima i privremene objekte: `byConstRef(CopyCounter{});` ✅
- ❌ `T&` ne prima privremene: `increment(5);` ne radi (`errors/e18`), jer
  bi izmena otišla u objekat koji odmah nestaje.

---

# 11. Vraćanje pokazivača i referenci (EC++ Item 21)

✅ Bezbedno je vratiti referencu na nešto što **živi duže od poziva**:

```cpp
Counter& add(int v) { value_ += v; return *this; }   // *this -- chaining
const int& value() const { return value_; }         // član objekta
static int& instances() { static int n = 0; return n; }  // static
```

⚠️ **Nikad** adresu ili referencu na **lokalnu** promenljivu:

```cpp
int* make() { int local = 42; return &local; }   // UB (ub/u03)
int& make() { int local = 42; return local; }    // UB (ub/u04)
```

Kompajler upozori (`-Wreturn-local-addr`), i to upozorenje treba tretirati
kao grešku. Zanimljivo je da g++ ovde **namerno vraća `nullptr`** (kod je
ionako UB), pa se program sruši na null dereferenciranju.

⚠️ Getter koji vraća referencu na član je ispravan, ali ne i poziv nad
**privremenim** objektom:

```cpp
const std::string& n = makePerson().getName();   // UB: Person nestaje, n visi (ub/u11)
std::string n = makePerson().getName();          // ✅ kopija
```

---

# 12. Pokazivači na elemente kontejnera

`std::vector` pri rastu iznad kapaciteta seli sve elemente u novi blok
memorije, a stari oslobađa. Svaki pokazivač, referenca ili iterator na
stari blok tada visi.

```cpp
int* first = &v[0];
v.push_back(x);      // može da realocira
*first;              // UB (ub/u06, ASan: heap-use-after-free)
```

Rešenja: čuvaj **indeks**, uzmi pokazivač **posle** poslednje izmene
veličine, ili unapred pozovi `v.reserve(n)`. Više o invalidaciji u week2
s09.

---

# 13. Vlasništvo: `new`/`delete` i pametni pokazivači

| Bag | Primer | Šta prijavi ASan |
|---|---|---|
| use-after-free | `delete p; *p;` | heap-use-after-free (`ub/u07`) |
| double free | `delete p; delete q;` (q == p) | double-free (`ub/u08`) |
| pogrešan oblik (EC++ Item 16) | `new int[5]` + `delete` | alloc-dealloc-mismatch (`ub/u09`) |
| curenje memorije | `new` bez `delete` | LeakSanitizer: detected memory leaks (`ub/u10`) |

- `delete p;` **ne postavlja** `p` na `nullptr`; `p` i dalje čuva staru
  adresu.
- `new` ide sa `delete`, a `new[]` sa `delete[]`.
- LeakSanitizer je konzervativan: ako vrednost pokazivača slučajno ostane
  negde u memoriji, taj objekat ne prijavi. Zato `u10` curi u petlji.

**Moderni C++:** vlasništvo izražavaju `std::unique_ptr` i
`std::shared_ptr` (week2 s08), a **sirov pokazivač znači "posmatram, ne
posedujem"** (C++ Core Guidelines R.3):

```cpp
auto owned = std::make_unique<int>(42);  // oslobađa se sam
int* observer = owned.get();             // ne-vlasnik; ne sme da ga obriše
```

---

# 14. Pokazivač na člana klase

Pokazivač na člana ne čuva adresu nego **"koji član"**. Tek sa objektom
dobija konkretnu vrednost.

```cpp
int Point::* coord = &Point::x;
void (Point::* shiftFn)(int) = &Point::shift;

p.*coord = 5;        // član x objekta p
(p.*shiftFn)(1);     // poziv člana; zagrade su obavezne
(ptr->*shiftFn)(1);  // isto preko pokazivača na objekat
```

Retko se piše ručno, ali ga koriste biblioteke (`std::invoke`,
`std::mem_fn`, projekcije u `std::ranges`).

---

# Moderni C++ stil

```cpp
void print(const Widget& w);        // mora da postoji, samo čita
void update(Widget& w);             // mora da postoji, menja
void maybeLog(const Logger* log);   // opciono -- proveri nullptr
auto w = std::make_unique<Widget>();         // vlasništvo
void process(std::span<const int> data);     // C++20: pokazivač + dužina u jednom
```

---

# Pravilo za praksu

✅ Parametri: `const T&` podrazumevano, `T&` za izlazne, `T*` samo kad je
opciono.

✅ Uvek `nullptr`, nikad `0` ili `NULL` za pokazivače.

✅ Svaki pokazivač odmah inicijalizuj (`= nullptr` ili stvarna adresa).

✅ Vlasništvo preko `std::unique_ptr` / `std::shared_ptr`; sirov pokazivač
nikad ne `delete`-uješ.

⚠️ Ne vraćaj pokazivač ni referencu na lokalnu promenljivu, a warning
`-Wreturn-local-addr` tretiraj kao grešku.

⚠️ Ne čuvaj pokazivač ni referencu na element `std::vector`-a preko
`push_back`.

⚠️ `const T&` produžava život privremenog samo kad se veže **direktno**,
ne kroz povratnu vrednost funkcije.

⚠️ Pokreći kod sa ASan/UBSan (`build.sh` to radi). Većina bagova iz ove
lekcije se ne vidi pri kompajliranju.

**Rezime:** pokazivač je objekat koji čuva adresu; može da bude prazan i da
se preusmeri. Referenca je drugo ime za postojeći objekat; ne može ni jedno
ni drugo. Oba mogu da **vise** kad objekat nestane, i to je najčešći izvor
UB-a. Referencu koristi kad objekat mora da postoji, pokazivač kad je
opciono, a vlasništvo prepusti pametnim pokazivačima.

## Zapažanja posle vežbe
