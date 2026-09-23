# Sesija 10 — Vežba: sopstveni `UniquePtr`, embedded RAII, objekti bez heap-a

Završna vežba: sve iz week1 i week2 u tri mala dela. Kod je napisan tako
da radi i na sistemu **bez heap-a** (mikrokontroler), gde su RAII i
placement new osnovni alati.

**Izvori:** standard, delovi `[unique.ptr]` (kao model), `[new.delete.placement]`
i `[ptr.launder]`; *Effective Modern C++* **Item 18** i **Item 21**;
*Effective C++* **Item 14** (kopija RAII objekta); C++ Core Guidelines
**R.1**, **CP.20** (RAII za lock) i **C.66**.

**Kako vežbati:**

```
./build.sh week2-modern-layer/s10-uniqueptr-embedded/exercise.cpp   # TVOJA verzija (kostur sa TODO)
./build.sh week2-modern-layer/s10-uniqueptr-embedded/main.cpp       # rešenje
./check_cases.sh week2-modern-layer/s10-uniqueptr-embedded
```

- `exercise.cpp`: kostur. Kompajlira se i ovakav, a zadaci su u
  komentarima, sa testovima koji se otključavaju u `main()`.
- `main.cpp`: rešenje. Kao u s08, zamenjuje globalni `operator new` samo
  da bi dokazao da deo 3 ne koristi heap.
- `errors/` (e01–e03), `ub/` (u01–u03).
- Deo "Buffer u rule of 0" iz starog plana je urađen u week1 s05
  (korak 4), pa ga ovde nema.

---

# Deo 1 — `UniquePtr<T>`

```cpp
UniquePtr(UniquePtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)) {}
UniquePtr& operator=(UniquePtr&& other) noexcept { reset(other.release()); return *this; }
T* release() noexcept { return std::exchange(ptr_, nullptr); }
void reset(T* p = nullptr) noexcept {
    T* old = std::exchange(ptr_, p);   // prvo postavi novi
    if (old != p) delete old;          // pa obriši stari
}
```

Šta se proverava (`main.cpp`):

| Zahtev | Kako | Test |
|---|---|---|
| jedan vlasnik | kopija `= delete` (`errors/e01`) | `kopija=false` |
| prenos vlasništva | move, izvor → `nullptr` | `a=prazan` |
| move dodela samom sebi | `reset(other.release())`: release vrati isti pokazivač, reset ga ne briše | `b->id=2` posle `b = std::move(b)` |
| radi u `std::vector` | move je `noexcept` | 3 elementa bez kopija |
| bez dodatne cene | samo jedan pokazivač | `sizeof = 8` |
| `makeUnique<T>(args...)` | savršeno prosleđivanje (s09) | |

Zašto **reset** prvo postavlja novi pokazivač, pa briše stari: destruktor
starog objekta može (preko nekog lanca) da pristupi istom `UniquePtr`-u.
Tada on već mora biti u ispravnom stanju. Tako je propisan i
`std::unique_ptr::reset`.

Šta ova verzija nema u odnosu na `std::unique_ptr`: custom deleter
(drugi parametar šablona, s08), verziju za nizove (`T[]`), konverziju
`UniquePtr<Derived>` → `UniquePtr<Base>`, i poređenja.

---

# Deo 2 — Embedded RAII: prekidi i lock

```cpp
class InterruptGuard {
public:
    InterruptGuard() noexcept : wasEnabled_(interruptsEnabled) { interruptsEnabled = false; }
    ~InterruptGuard() { interruptsEnabled = wasEnabled_; }   // PRETHODNO stanje
    ...
};
```

- Guard na izlazu vraća **prethodno** stanje, a ne "uključi prekide".
  Test sa ugnežđenim guard-ovima: posle unutrašnjeg su prekidi i dalje
  **isključeni**. Naivan guard koji uvek uključuje ih uključi usred
  spoljne kritične sekcije, i ta greška se ne vidi dok se ne desi prekid u
  pogrešnom trenutku.
- Guard se ne kopira (`errors/e02`): dve kopije bi vratile stanje dvaput.
- Za lock ne treba sopstveni guard: `std::lock_guard` radi sa **bilo
  kojim** tipom koji ima `lock()` i `unlock()` (BasicLockable), pa i sa
  sopstvenim `SpinLock`-om (test).

Na stvarnom Cortex-M mikrokontroleru isto izgleda ovako (ilustracija, nije
kompajlirano u ovom repou):

```cpp
InterruptGuard() noexcept : primask_(__get_PRIMASK()) { __disable_irq(); }
~InterruptGuard() { __set_PRIMASK(primask_); }
```

---

# Deo 3 — Objekat bez heap-a

```cpp
template <typename T>
class StaticStorage {
    alignas(T) unsigned char buffer_[sizeof(T)];   // prostor i poravnanje za T
    bool constructed_ = false;
public:
    template <typename... Args> T& emplace(Args&&... args) {
        destroy();
        T* object = new (buffer_) T(std::forward<Args>(args)...);   // placement new
        constructed_ = true;
        return *object;
    }
    void destroy() noexcept { if (constructed_) { get().~T(); constructed_ = false; } }
    T& get() noexcept { return *std::launder(reinterpret_cast<T*>(buffer_)); }
    ~StaticStorage() { destroy(); }
};
```

Test: `heap alokacija: 0`.

Pravila za placement new:

| Pravilo | Ako se prekrši |
|---|---|
| bafer je **dovoljno velik** | objekat preko kraja bafera; `static_assert` ga hvata pri kompajliranju (`errors/e03`) |
| bafer je **poravnat** za `T` (`alignas(T)`) | UB: UBSan `constructor call on misaligned address` (`ub/u03`) |
| objekat se uništava **ručnim destruktorom** `p->~T()` | `delete` pokuša da oslobodi memoriju koja nije sa heap-a (`ub/u01`) |
| **svaki** objekat se uništi pre novog u istom baferu | resursi starog objekta cure (`ub/u02`) |
| pristup kroz bafer ide preko `std::launder` (C++17) | formalno UB: kompajler sme da pretpostavi da se u baferu nije ništa promenilo |

Klasa kao `StaticStorage` sprovodi sva pravila na jednom mestu. Standardna
biblioteka ima isto: `std::optional<T>` čuva `T` u sopstvenom baferu,
bez heap-a, sa ispravnim poravnanjem i uništavanjem.

✅ U kodu bez heap-a: `std::optional`, `std::array`, `std::variant` i
sopstveni "storage" tipovi umesto `new`. Placement new samo iza takvog
tipa, nikad rasut po kodu.

---

# Pravilo za praksu

✅ Pametni pokazivač: move `noexcept`, kopija obrisana, `reset` prvo
postavlja pa briše.

✅ Guard vraća **prethodno** stanje i ne kopira se.

✅ Placement new: `alignas`, dovoljna veličina (`static_assert`), ručni
destruktor, sve umotano u klasu.

⚠️ Na mikrokontroleru neporavnat pristup nije samo sporiji: može da
izazove hardverski izuzetak.

**Rezime:** `std::unique_ptr` nije magija: to je RAII klasa sa pet
specijalnih funkcija napisanih po pravilima iz week1. Isti obrazac
(zauzmi u konstruktoru, vrati u destruktoru, zabrani kopiju) radi i za
prekide i lock-ove, a sa placement new-om i za objekte kojima ne treba
heap.

## Zapažanja posle vežbe

