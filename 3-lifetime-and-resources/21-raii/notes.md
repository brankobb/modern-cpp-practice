# Lekcija 21 — RAII i bezbednost pri izuzecima

**RAII** (Resource Acquisition Is Initialization): resurs (memorija, fajl,
lock, konekcija) pripada objektu. Konstruktor ga zauzme, destruktor ga
oslobodi. Pošto jezik **uvek** pozove destruktor lokalnog objekta, pa i
kad izuzetak prekine funkciju (lekcija 19), resurs ne može da procuri. Ova lekcija
pokazuje kako to radi, i šta znači da je kod "bezbedan pri izuzecima".

**Izvori:** standard, delovi `[except.ctor]` (stack unwinding),
`[except.spec]` (`noexcept`) i `[except.terminate]`. Uz to *Effective
C++* **Item 8** (destruktor ne baca), **Item 13** (objekti upravljaju
resursima), **Item 14** (kopija RAII objekta) i **Item 29** (kod bezbedan
pri izuzecima), i C++ Core Guidelines **R.1**, **E.6**, **E.16** i **C.36**.

**Kako vežbati:**

```
./build.sh 3-lifetime-and-resources/21-raii/main.cpp
./check_cases.sh 3-lifetime-and-resources/21-raii
```

- `errors/` (e01–e02): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): kod koji se kompajlira, a pri pokretanju **curi**
  (LeakSanitizer) ili **pukne** (`std::terminate`). u02 i u03 nisu UB
  nego zagarantovan prekid programa, ali greška je ista vrsta: vidi se tek
  pri izvršavanju.

---

# 1. RAII

```cpp
class File {
public:
    File() : handle_(std::tmpfile()) { if (!handle_) throw std::runtime_error("..."); }
    ~File() { if (handle_) std::fclose(handle_); }
    File(const File&) = delete;              // jedinstven resurs (errors/e01, EC++ Item 14)
    File& operator=(const File&) = delete;
private:
    std::FILE* handle_;
};
```

Test: `[otvoren] [zatvoren] | uhvaćen` -- fajl se zatvori **pre** nego
što `catch` počne, iako je funkcija prekinuta izuzetkom.

Pravila:

- ✅ Resurs se zauzima u konstruktoru i odmah predaje objektu (R.1). Nema
  koraka "zauzmi sada, predaj kasnije".
- ✅ Konstruktor koji ne uspe da zauzme resurs **baca** izuzetak. Objekat
  tada ne postoji, pa nema polu-napravljenog stanja.
- ✅ Odluči šta znači **kopija** (EC++ Item 14): zabranjena (fajl, lock),
  duboka kopija (lekcija 20), deljeno vlasništvo (`shared_ptr`) ili prenos
  vlasništva (move, lekcija 22).

**Ne piši RAII klasu kad postoji:**

| Resurs | Gotov RAII tip |
|---|---|
| memorija | `std::vector`, `std::string`, `std::unique_ptr`, `std::shared_ptr` |
| mutex | `std::lock_guard`, `std::scoped_lock`, `std::unique_lock` |
| fajl | `std::fstream`, ili `std::unique_ptr<FILE, FileCloser>` |
| bilo šta | `std::unique_ptr<T, Deleter>`, ili scope guard (sekcija 6) |

---

# 2. Stack unwinding

```
outer() middle() inner() ~inner() ~middle() ~outer() | catch
```

Kad izuzetak izađe iz funkcije, uništavaju se svi njeni potpuno napravljeni
lokalni objekti, pa isto u funkciji koja ju je pozvala, sve do `catch`
bloka. Redosled je obrnut od pravljenja, kao i inače. Kod posle mesta
bacanja (`(ovo se ne izvrši)`) se preskače.

---

# 3. Izuzetak u konstruktoru

Iz lekcije 19: kad konstruktor baci, destruktor objekta se **ne poziva**, ali se
**već napravljeni članovi** unište.

```cpp
Service() : buffer_(new int[256]), config_(new Config) {}   // ❌ Config() baci -> buffer_ curi (ub/u01)
std::unique_ptr<int[]> buffer_;                             // ✅ član sa destruktorom -> oslobodi se
```

Zato klasa sa **više** resursa drži svaki u zasebnom RAII članu, a ne
sirove pokazivače sa `delete` u destruktoru. Tada destruktor klase često
ni ne treba (rule of 0).

---

# 4. Garancije pri izuzecima (EC++ Item 29)

| Garancija | Posle izuzetka | Primer |
|---|---|---|
| **nothrow** | izuzetak se ne dešava | `swap` pokazivača, destruktori, move (obično) |
| **strong** | stanje kao pre poziva (sve ili ništa) | `vector::push_back`, copy-and-swap |
| **basic** | objekat ispravan (invarijante važe, nema curenja), ali stanje nepoznato | većina operacija |
| nijedna | objekat pokvaren ili curenje | ❌ |

Test iz `main.cpp` (kopija elementa "pokvaren" baca):

```
basic posle izuzetka:  jabuka           <- ispravan, ali ni staro ni novo
strong posle izuzetka: staro1 staro2    <- netaknut
```

**Kako se dobija strong garancija: copy-and-swap.** Sva posla koja mogu da
bace urade se **sa strane** (na kopiji), a zatim se rezultat zameni
operacijom koja **ne baca**:

```cpp
void assignStrong(const Inventory& other) {
    std::vector<Item> copy = other.items_;   // može da baci -- items_ netaknut
    items_.swap(copy);                       // ne baca
}
Name& operator=(Name other) {                // kopija se pravi u parametru
    swap(*this, other);                      // noexcept
    return *this;
}
```

Isti oblik radi i za dodelu i za kopiju i (u lekciji 22) za move. Cena je jedna
privremena kopija.

✅ Svaka funkcija treba bar **basic** garanciju. Strong kad nije preskupa.
Nothrow za `swap`, move i destruktore.

---

# 5. Destruktor ne baca (EC++ Item 8)

- Od C++11 su destruktori **implicitno `noexcept`**. Izuzetak iz
  destruktora poziva `std::terminate` odmah, i `catch` u pozivaocu ga
  nikad ne vidi (`ub/u02`).
- Ista stvar za svaku `noexcept` funkciju (`ub/u03`). Kompajleri upozore
  kad je `throw` direktno u telu (`-Wterminate`, `-Wexceptions`), a kad
  baci pozvana funkcija, ne upozori nijedan (test).

Kad zatvaranje može da ne uspe (flush na disk, mrežna konekcija):

```cpp
void close();                        // može da baci: pozivalac zove i reaguje
~Connection() noexcept {
    try { close(); }                 // rezervna opcija ako niko nije pozvao close()
    catch (const std::exception& e) { /* zabeleži, ne bacaj dalje */ }
}
```

---

# 6. RAII bez pisanja klase

```cpp
struct FileCloser { void operator()(std::FILE* f) const noexcept { std::fclose(f); } };
std::unique_ptr<std::FILE, FileCloser> file(std::tmpfile());   // fclose u destruktoru
```

- Funkcijski objekat kao deleter ne mora da se prosleđuje, a
  `unique_ptr` ostaje veličine jednog pokazivača. Test: 8 bajtova, a sa
  pokazivačem na funkciju (`decltype(&std::fclose)`) 16. Pokazivač na
  funkciju **mora** da se prosledi konstruktoru (`errors/e02`), a g++ uz
  `decltype(&std::fclose)` upozori i `-Wignored-attributes`. Detaljno:
  lekcija 32.
- **Scope guard**: objekat čiji destruktor pozove zadatu lambdu. Za
  "vrati stanje na izlazu, kako god se izašlo" kad nema odgovarajućeg
  tipa.

---

# Pravilo za praksu

✅ Svaki resurs pripada objektu čiji ga destruktor oslobađa. `delete`,
`fclose`, `unlock` se ne pišu u običnom kodu.

✅ Klasa sa više resursa: svaki resurs u svom RAII članu.

✅ Destruktori, `swap` i move ne bacaju.

✅ Strong garancija preko copy-and-swap: posao koji može da baci ide na
kopiju, zamena ne baca.

⚠️ `noexcept` je obećanje, ne provera: izuzetak kroz njega je
`std::terminate`.

**Rezime:** jezik garantuje jednu stvar, a to je da se destruktori
lokalnih objekata pozovu kako god se iz bloka izašlo. RAII koristi tu
garanciju za **sve** resurse, pa kod bezbedan pri izuzecima nastaje sam od
sebe, bez `try`/`catch` na svakom koraku. Preostalo je samo da operacije
koje menjaju stanje rade "sa strane" pa zamene (strong garancija), i da
destruktori nikad ne bacaju.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_datoteka_raii`](exercises/ex1_datoteka_raii.cpp) | usage | RAII omotač, unique_ptr sa deleter-om i scope guard (sekcije 1, 6) | — |
| [`ex2_lock_unlock`](exercises/ex2_lock_unlock.cpp) | why | zašto lock_guard, a ne lock() ... unlock() (sekcije 1, 2) | `-DNAIVNO` |
| [`ex3_jaka_garancija`](exercises/ex3_jaka_garancija.cpp) | why | zašto "sve ili ništa" (strong guarantee, sekcija 4, EC++ Item 29) | `-DNAIVNO` |

## Zapažanja posle vežbe

