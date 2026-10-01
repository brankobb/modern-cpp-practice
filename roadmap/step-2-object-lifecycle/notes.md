# Korak 2 — Objektni C++ i životni ciklus objekta

## Cilj koraka

Da savladaš Rule of 0/3/5, da znaš **koja specijalna funkcija se poziva
u kom trenutku**, i da umeš da napišeš klasu koja se ponaša ispravno u
svim situacijama: kopiranje, dodela, privremeni objekti, scope.

Ovo je korak na kom C programeri najčešće padaju na intervjuu, jer misle
da je "klasa = struct sa funkcijama". Nije. Klasa ima **životni ciklus**
koji jezik poziva automatski — konstruktor, kopiranje, pomeranje,
destrukcija — i ti odlučuješ šta se u svakoj od tih tačaka dešava.

| Fajl | Šta je |
|---|---|
| `notes.md` | ovaj tekst |
| `demos.cpp` | `Noisy` klasa koja ispisuje svaku specijalnu funkciju + primeri za svaku temu (sekcija `// ----- N` = tema N) |
| `task/buffer.cpp` | **zadatak**: kostur, ti pišeš `Buffer` (Rule of 5) |
| `solutions/buffer.cpp` | rešenje; sa `-DNAIVE_ASSIGN` pokazuje zašto je naivna dodela pogrešna |
| `solutions/bonus1_rule_of_zero.cpp` | `unique_ptr` verzija i `vector` verzija |
| `solutions/bonus2_virtual_dtor.cpp` | `-DNO_VIRTUAL`: ASan pokaže šta se desi bez virtual destruktora |
| `solutions/bonus3_ring_buffer.cpp` | `RingBuffer` bez heap-a, trivially copyable |

```
./build.sh roadmap/step-2-object-lifecycle/demos.cpp
./build.sh roadmap/step-2-object-lifecycle/task/buffer.cpp
```

**Najbolji način da učiš ovaj korak:** pokreni `demos.cpp`, pa za svaki
red u sekciji 1 **pre** nego što pogledaš izlaz reci naglas koja funkcija
će se pozvati. Gde pogrešiš — tu je rupa.

---

## Šta učiš

### 1. Konstruktori — svi tipovi

| Konstruktor | Potpis | Kada se poziva |
|---|---|---|
| default | `T()` | `T x;`, `T x{};`, član bez inicijalizatora, `new T[n]` |
| sa parametrima | `T(int a, int b)` | `T x{1, 2};` |
| **copy** | `T(const T& other)` | **novi** objekat iz postojećeg lvalue-a: `T b = a;`, `T b{a};`, prosleđivanje po vrednosti, `return` lokalnog (ako nema elizije/move-a) |
| **move** | `T(T&& other)` | novi objekat iz rvalue-a: `T b = std::move(a);`, privremeni objekat (ako nema elizije) — detaljno u Koraku 3 |
| konverzije | `T(int a)` | **implicitno**: `T x = 5;`, `f(5)` za `void f(T)` — osim ako je `explicit` |
| delegirajući | `T() : T(42) {}` | prvo se izvrši ciljni konstruktor, pa telo ovog |
| `= default` | `T() = default;` | "napiši ga ti, kompajleru, kao da ga nisam deklarisao" |
| `= delete` | `T(const T&) = delete;` | učestvuje u overload resolution, ali poziv je greška |

**Moraš da razumeš:**

```cpp
Noisy c = b;   // c NE POSTOJI pre ovog reda -> copy KONSTRUKTOR
c = b;         // c VEĆ postoji             -> copy DODELA (operator=)
```

Znak `=` u deklaraciji **nije** dodela. To su dve različite funkcije, sa
različitim poslom: konstruktor pravi objekat od nule (nema starog stanja),
dodela mora da se reši **starog** stanja (stari resurs) pre nego što
preuzme novo.

Iz `demos.cpp`, sekcija 1 — šta jezik zaista poziva:

| Kod | Poziva se |
|---|---|
| `Noisy c = b;` | copy ctor |
| `c = b;` | copy assign |
| `Noisy d = std::move(c);` | move ctor |
| `a = Noisy{2};` | ctor (privremeni) → **move** assign → dtor privremenog |
| `Noisy e = Noisy{3};` | **samo** ctor (C++17 garantovana elizija) |
| `Noisy f = make(4);` (`return Noisy{v};`) | **samo** ctor |
| `by_value(b);` | copy ctor u parametar, dtor parametra na kraju funkcije |
| `by_cref(b);` | ništa |

Dublje: `2-classes/14-class-basics/notes.md` (sekcije 2, 8, 9 i 10),
`3-lifetime-and-resources/20-copying/notes.md` (sekcija 2).

### 2. Destruktor

- `~T()`. Poziva se automatski: kraj scope-a (automatski objekti), `delete`
  (dinamički), kraj programa (statički), kraj punog izraza (privremeni).
- **Redosled: obrnut od konstrukcije.** Lokalni objekti u bloku; članovi
  klase (obrnuto od **deklaracije**); baza/izvedena (prvo `~Derived`, pa
  `~Base`); elementi niza (od poslednjeg ka prvom).
- Destruktor je podrazumevano `noexcept` — ne bacaj iz njega.

**Moraš da razumeš:** ako klasa ima virtualnu funkciju (koristi se
polimorfno, preko `Base*`/`Base&`), destruktor **mora** biti virtualan.
Inače je `delete base_ptr;` za objekat tipa `Derived` **undefined
behavior** — u praksi se `~Derived` ne pozove (resursi `Derived`-a
procure), a oslobađa se pogrešna veličina memorije. Vidi Bonus 2 i
`2-classes/16-inheritance-and-polymorphism/notes.md` (sekcija 5).

### 3. Rule of 0 / 3 / 5

Specijalne funkcije koje kompajler može sam da napiše: default ctor,
**destruktor, copy ctor, copy assign, move ctor, move assign**.

- **Rule of 0:** klasa koja **ne upravlja resursom direktno** ne piše
  nijednu od njih. Njeni članovi (`std::string`, `std::vector`,
  `std::unique_ptr`, tvoj `File`) već znaju da se kopiraju/pomeraju/
  unište, a kompajler-generisane verzije samo pozovu to isto za svaki član.
  **Ovo je cilj.**
- **Rule of 3 (C++03):** ako pišeš **bilo koju** od destruktor / copy ctor /
  copy assign — skoro sigurno ti trebaju sve tri. Destruktor koji radi
  `delete[]` znači da je podrazumevana (plitka) kopija pogrešna.
- **Rule of 5 (C++11):** isto, plus move ctor i move assign.

**Moraš da razumeš** zašto je podrazumevana kopija pogrešna za resurs:

```cpp
class Bad {
    std::uint8_t* data_;
public:
    explicit Bad(std::size_t n) : data_{new std::uint8_t[n]} {}
    ~Bad() { delete[] data_; }
    // copy ctor nije napisan -> kompajler kopira POKAZIVAČ (član po član)
};
Bad a{10};
Bad b = a;     // a.data_ == b.data_
               // na kraju scope-a: ~b briše, pa ~a briše ISTU memoriju -> double free
```

Dve ispravne opcije: **zabrani** kopiranje (`= delete`, kao `File` u Koraku 1)
ili ga **implementiraj** (deep copy, kao `Buffer` u zadatku).

Zamka koju svi promaše — šta kompajler generiše kad nešto deklarišeš:
- deklarišeš destruktor ili copy operaciju → move se **ne generiše**
  (move zahtevi tiho padaju na kopiju);
- deklarišeš move operaciju → copy postaje **obrisan**.

Zato: ili ne pišeš ništa (Rule of 0), ili pišeš/`= default`/`= delete`
**svih pet** — da se vidi namera. Detaljna tabela je u
`3-lifetime-and-resources/23-special-member-generation/notes.md` (sekcija 1).

### 4. Copy assignment operator

```cpp
T& operator=(const T& other);
```

Mora da:
1. oslobodi **stari** resurs (objekat već postoji — to je razlika od konstruktora);
2. napravi kopiju resursa iz `other`;
3. radi ispravno i kad je `&other == this` (**self-assignment**: `a = a;`,
   ili realnije `v[i] = v[j]` kad je `i == j`, `*p = *q` kad pokazuju na isto);
4. vrati `*this` po referenci (za `a = b = c;`).

Naivna verzija je pogrešna baš za self-assignment:

```cpp
delete[] data_;                       // ako je other == *this, upravo smo obrisali izvor
data_ = new std::uint8_t[other.size_];
std::copy_n(other.data_, other.size_, data_);   // other.data_ je sad NOVA memorija -> kopira smeće u sebe
```

Pokreni `solutions/buffer.cpp -DNAIVE_ASSIGN` i pogledaj test 6.

Dve ispravne verzije:

```cpp
// (a) prvo nova memorija, pa brisanje stare -- self-safe bez if-a, strong garancija
std::uint8_t* fresh = new std::uint8_t[other.size_];
std::copy_n(other.data_, other.size_, fresh);
delete[] data_;
data_ = fresh;
size_ = other.size_;
return *this;

// (b) copy-and-swap -- kopiju pravi copy ctor, zamenu noexcept swap
T& operator=(const T& other) {
    T tmp{other};      // ako baci, *this je netaknut
    swap(tmp);         // noexcept
    return *this;      // ~tmp oslobađa STARI resurs
}
```

**Moraš da razumeš:** `operator=` se poziva samo nad objektom koji već
postoji i već drži resurs. Svaki put kad pišeš `operator=`, pitaj se:
"šta se desi sa starim resursom?" i "šta ako je `other` ja?".

Dublje: `2-classes/15-operator-overloading/notes.md` (sekcija 4),
`3-lifetime-and-resources/21-raii/notes.md` (sekcija 4).

### 5. Nasleđivanje

```cpp
class Shape {
public:
    virtual ~Shape();                          // polimorfna baza => virtual destruktor
    virtual const char* name() const;          // može da se preklopi
    virtual double area() const = 0;           // čisto virtuelna => Shape je apstraktna
};
class Circle final : public Shape {            // final: od Circle se ne nasleđuje
public:
    const char* name() const override;         // override: kompajler proveri da baza to ima
    double area() const override;
};
```

- **Redosled:** konstrukcija `Base` → članovi `Derived` → telo `Derived`;
  destrukcija tačno obrnuto.
- **vtable / vptr:** svaka klasa sa virtual funkcijama ima tabelu
  pokazivača na funkcije (jednu po klasi, u `.rodata`); svaki **objekat**
  dobija skriveni pokazivač `vptr` na tabelu svoje klase. Poziv
  `s->name()` = učitaj vptr, učitaj adresu iz tabele, indirektan poziv.
  Cena: +1 pokazivač po objektu (`demos.cpp`, sekcija 5:
  `sizeof(WithVirtual) > sizeof(Plain)`), indirektan poziv, i (najskuplje)
  kompajler ne može da inline-uje.
- **`override`** uvek kad preklapaš — bez njega pogrešan potpis (zaboravljen
  `const`) pravi **novu** funkciju umesto da preklopi, tiho.
- **Slicing:** `Animal a = dog;` ili prosleđivanje `Dog` u `f(Animal a)` po
  vrednosti kopira **samo `Animal` deo**. Virtualni pozivi posle toga idu
  na `Animal`, ne na `Dog` (`demos.cpp`: "by value: ..."). Polimorfne
  objekte prosleđuj po referenci ili pokazivaču, nikad po vrednosti.

**Moraš da razumeš:** `virtual` ima cenu. U embedded-u ga koristiš kad ti
**stvarno** treba izbor implementacije u runtime-u (npr. drajver izabran
po konfiguraciji); kad se tip zna pri kompajliranju, šabloni (Korak 4)
daju isto bez vptr-a. Ali moraš da znaš kako radi, jer je to pitanje na
svakom intervjuu.

Dublje: `2-classes/16-inheritance-and-polymorphism/notes.md` (sekcije 2, 4, 7, 8 i 12).

### 6. `explicit`

```cpp
struct Meters  { explicit Meters(double v); };
struct Celsius { Celsius(double v); };          // bez explicit
void set_distance(Meters m);
void set_temperature(Celsius c);

set_distance(5.0);        // greška: double se ne pretvara u Meters sam od sebe
set_distance(Meters{5});  // ok, namera je vidljiva
set_temperature(36.6);    // kompajlira se -- 36.6 čega?
```

**Pravilo:** svaki konstruktor koji može da se pozove sa jednim argumentom
je `explicit`, osim ako je implicitna konverzija stvarno prirodna (npr.
`std::string` iz `const char*`). Isto za `explicit operator bool()`.

Probaj `demos.cpp -DTRY_IMPLICIT`. Dublje:
`2-classes/17-type-conversions/notes.md` (sekcija 5).

### 7. `this`, `static` članovi, `friend`

- `this` — pokazivač na objekat nad kojim je metoda pozvana; u `const`
  metodi je `const T*`. `return *this;` omogućava lančano pozivanje
  (`s.set_gain(4).set_offset(-2)`).
- `static` član — jedan za celu klasu, ne po objektu (brojač živih
  objekata, `next_id_` u zadatku). `static` metoda nema `this`.
  C++17: `static inline int count = 0;` definiše ga direktno u klasi.
- `friend` — funkcija ili klasa koja sme da pristupi privatnim članovima.
  Najčešće za `operator<<` i simetrične operatore (`a == b`). Ne krši
  enkapsulaciju — deo je interfejsa klase, deklarisan u njoj.

Dublje: `2-classes/14-class-basics/notes.md` (sekcije 5 i 6),
`2-classes/15-operator-overloading/notes.md` (sekcija 3).

### 8. Inicijalizaciona lista članova

```cpp
class Device {
public:
    Device(int id, int& counter) : first_{"first"}, second_{"second"}, id_{id}, counter_{counter} {}
private:
    Part first_;       // 1.
    Part second_;      // 2.
    const int id_;     // 3. const     -> MORA u listu
    int& counter_;     // 4. referenca -> MORA u listu
    int retries_ = 3;  // default member initializer (C++11), ako ga lista ne pomene
};
```

**Moraš da razumeš:**
- **Redosled inicijalizacije = redosled DEKLARACIJE članova**, ne redosled
  u listi. Lista u drugačijem redosledu je laž koju kompajler ignoriše
  (i upozori: `-Wreorder`). Ako jedan član zavisi od drugog, zavisnost mora
  da prati deklaraciju (`demos.cpp -DREORDER`: `width_{height_ * 2}` čita
  još neinicijalizovan `height_`).
- Član koji nije u listi i nema default initializer → **default-init**: za
  klasu se pozove default konstruktor, za `int`/pokazivač ostaje smeće.
- Dodela u telu konstruktora (`x_ = a;`) **nije** inicijalizacija: član je
  već default-konstruisan, pa ga dodeljuješ ponovo. Za `const` i reference
  je to nemoguće, za skupe tipove je dvostruki posao.

Dublje: `1-language-basics/03-initialization/notes.md` (sekcije 8 i 9),
`2-classes/14-class-basics/notes.md` (sekcije 2 i 4).

---

## Šta MORAŠ da razumeš (suština koraka)

1. **Životni ciklus objekta je deterministički.** Konstrukcija → korišćenje
   → destrukcija, tačno tim redom, u tačno definisanim trenucima.
2. **Kopiranje je podrazumevano, ali može biti pogrešno.** Ako klasa drži
   resurs preko golog pokazivača/handle-a, podrazumevana kopija je bug.
3. **`operator=` ≠ copy ctor.** Prvi radi nad postojećim objektom (ima
   staro stanje), drugi pravi novi.
4. **Virtualan destruktor je obavezan** ako klasa ima virtualne funkcije i
   briše se preko pokazivača na bazu.
5. **Redosled inicijalizacije članova = redosled deklaracije.** Uvek.
6. **Rule of 0 je cilj.** Resurs drži jedna mala RAII klasa; sve ostalo je
   sastavljeno od takvih i ne piše ništa.
7. **`explicit` je podrazumevani izbor** za konstruktore sa jednim argumentom.

---

## Zadatak: "Klasa koja drži buffer" — Rule of 5 od nule

Kostur: `task/buffer.cpp` (tačan format ispisa je u komentaru na vrhu).

Klasa `Buffer`:
- konstruktor alocira `size` bajtova (`new std::uint8_t[size]`), destruktor
  oslobađa (`delete[]`);
- `size()` i `data()` (dve verzije: `const` vraća `const std::uint8_t*`);
- ispravni:
  - **copy ctor** — deep copy (sopstvena memorija, isti sadržaj),
  - **copy assign** — deep copy, bezbedan za self-assignment,
  - **move ctor** — preuzme pokazivač, izvor ostavi u **validnom** stanju
    (`nullptr`, `0`) — destruktor i dodela nad njim moraju da rade,
  - **move assign** — oslobodi svoje, preuzmi tuđe, izvor isprazni;
- `printf` u svakom konstruktoru, dodeli i destruktoru, da vidiš tačan
  redosled.

Testovi (već napisani u `main()`, otkomentarišeš ih jedan po jedan):

1. `Buffer a(10);`
2. `Buffer b = a;` → copy ctor; `b.data() != a.data()`, sadržaj isti
3. `Buffer c(5); c = a;` → copy assign
4. `Buffer d = std::move(a);` → move ctor; posle toga `a.data() == nullptr`
5. `Buffer e(3); e = std::move(b);` → move assign
6. `e = e;` → self-assignment, ne sme da pukne ni da izgubi sadržaj
   (u kodu preko reference `alias`, jer clang za doslovno `e = e;` sa
   `-Wall -Werror` odbije da kompajlira: `-Wself-assign-overloaded`)
7. dodatno: dodela **u** moved-from objekat (`a = d;`) mora da radi

**Postupak:** prvo napiši naivnu copy dodelu (delete[] → new[] → kopiraj) i
vidi da test 6 ispiše `FAIL`. Objasni sebi zašto, pa tek onda ispravi.

Pre nego što uporediš sa rešenjem, odgovori:
- Zašto je move ctor `noexcept`, i šta bi `std::vector<Buffer>` radio da
  nije? (Odgovor stiže u Koraku 3, ali pokušaj.)
- Zašto u destruktoru **ne** treba `if (data_ != nullptr)`?
- Zašto `id_` ne kopiraš iz `other`?

**Zašto ovaj zadatak:** klasik na C++ intervjuima. Ako ovo napišeš bez
greške i objasniš zašto svaki red postoji, prešao si Rule of 5.

### Bonus 1 — Rule of 0 (`solutions/bonus1_rule_of_zero.cpp`)

Napiši `Buffer` sa `std::unique_ptr<std::uint8_t[]>` umesto `new[]`/`delete[]`.

⚠️ Preciznije nego što se obično kaže: `unique_ptr` **nije odmah Rule of 0**
za tip koji treba da se kopira. Dobijaš besplatno destruktor i move, ali
**copy je obrisan** (`unique_ptr` se ne kopira). Ako ti treba deep copy,
pišeš copy ctor i copy assign — a onda move više nije generisan, pa ga
vraćaš (i moraš da paziš da `size_` ostane u skladu sa pokazivačem).
**Pravi Rule of 0** je `std::vector<std::uint8_t>` kao član: ne pišeš
nijednu specijalnu funkciju, a sve radi. U fajlu su obe verzije i
`static_assert`-ovi koji proveravaju šta je kompajler generisao.

### Bonus 2 — virtual destruktor (`solutions/bonus2_virtual_dtor.cpp`)

`Buffer` sa virtuelnim destruktorom i `CrcBuffer : public Buffer` koji ima
sopstveni resurs. Brisanje preko `Buffer*` poziva oba destruktora.
Sa `-DNO_VIRTUAL`: kompajler upozori, a ASan prijavi `new-delete-type-mismatch`.

### Bonus 3 — embedded `RingBuffer` (`solutions/bonus3_ring_buffer.cpp`)

`RingBuffer<T, N>` nad `std::array<T, N>` (bez heap-a), `push()` vraća
`bool` (pun → `false`, bez izuzetaka), `pop()` vraća `std::optional<T>`,
`N` mora da bude stepen dvojke (indeks preko maske umesto `%`).
`static_assert(std::is_trivially_copyable_v<...>)` dokazuje da sme
`memcpy` (DMA, snimak u flash, slanje preko UART-a). U komentaru: šta sve
kvari trivially copyable (prazan korisnički destruktor je dovoljan!).

---

## Provera pre Koraka 3

Pitanja su u `../questions.md` (odgovaraš u tom fajlu), referentni
odgovori u `../answers-reference.md` — otvori ih tek kad napišeš svoje.

## Zapažanja posle vežbe

<!-- tvoje beleške -->
