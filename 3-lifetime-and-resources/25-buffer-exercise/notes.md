# Lekcija 25 — Vežba: `Buffer` (rule of 3 → 5 → 0)

Sve iz lekcija 19–24 u jednoj klasi. `Buffer` poseduje dinamički niz `int`-ova
preko sirovog pokazivača, pa mora sam da napiše destruktor, kopiju i move.
Na kraju se ista klasa napiše bez ijedne specijalne funkcije (rule of 0),
i to je lekcija: ručno pisanje specijalnih funkcija je retko potrebno.

**Izvori:** C++ Core Guidelines **C.20** (rule of 0), **C.21** (rule of
5), **C.66** (`noexcept` move) i **C.83** (`swap`); *Effective C++*
**Item 11** i **Item 25** (swap); *Effective Modern C++* **Item 14**
(`noexcept`). Nastavak kursa 56 (Rule of 5 & 0).

**Kako vežbati:**

```
./build.sh 3-lifetime-and-resources/25-buffer-exercise/exercise.cpp   # TVOJA verzija (kostur sa TODO)
./build.sh 3-lifetime-and-resources/25-buffer-exercise/main.cpp       # rešenje, za poređenje
./check_cases.sh 3-lifetime-and-resources/25-buffer-exercise          # pogrešne varijante
```

- `exercise.cpp`: kostur. Kompajlira se i ovakav, a koraci su opisani u
  komentarima. Posle svakog koraka pokreni pod ASan-om.
- `main.cpp`: rešenje i merenja.
- `errors/` (e01–e02), `ub/` (u01–u02): najčešće greške u rule of 5.

---

# Korak 1 — rule of 3

Destruktor, copy konstruktor, copy dodela (lekcije 20, 21):

```cpp
~Buffer() { delete[] data_; }
Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) {
    std::copy(other.data_, other.data_ + size_, data_);
}
Buffer& operator=(const Buffer& other) {   // copy-and-swap: strong garancija, radi za a = a
    Buffer copy(other);
    swap(*this, copy);
    return *this;
}
friend void swap(Buffer& a, Buffer& b) noexcept {   // menja ČLANOVE
    std::swap(a.data_, b.data_);
    std::swap(a.size_, b.size_);
}
```

⚠️ `swap` mora da menja **članove**. `std::swap(*this, other)` zove move
dodelu, pa ako move dodela zove `swap`, to je beskonačna rekurzija
(`ub/u02`, stack overflow, bez upozorenja).

---

# Korak 2 — rule of 5

Dodaj move konstruktor i move dodelu (lekcija 22):

```cpp
Buffer(Buffer&& other) noexcept
    : data_(std::exchange(other.data_, nullptr)), size_(std::exchange(other.size_, 0)) {}
Buffer& operator=(Buffer&& other) noexcept {
    if (this != &other) {
        delete[] data_;                              // bez ovoga: curenje (ub/u01)
        data_ = std::exchange(other.data_, nullptr);
        size_ = std::exchange(other.size_, 0);
    }
    return *this;
}
```

**Rule of 5** (C.21): ako klasa napiše ili obriše bilo koju od pet funkcija
(destruktor, copy ctor, copy dodela, move ctor, move dodela), treba da
se odluči za **svih pet**. Razlog iz lekcije 22: deklarisan destruktor ili kopija
sprečava da kompajler napiše move (lekcija 23), pa bi se tiho kopiralo.

**Varijanta: jedan `operator=` po vrednosti** (`main.cpp`, sekcija 5):

```cpp
Buffer& operator=(Buffer other) noexcept { swap(*this, other); return *this; }
```

Pokriva i kopiju i move (parametar se pravi copy ili move konstruktorom),
uvek daje strong garanciju. Cena: move dodela uradi jedan move
konstruktor više. ❌ Ne sme uz njega i `operator=(Buffer&&)`: poziv
`a = std::move(b)` je tada dvosmislen (`errors/e01`).

---

# Korak 3 — merenja (`main.cpp`)

| Test | Rezultat | Zašto |
|---|---|---|
| kopija, move, dodele, dodela samom sebi | kopija=3, move=2 | move dodela samom sebi se preskače |
| `Buffer filled = makeFilled(5, 3);` | kopija=0, move=0 | NRVO (nije garantovano, lekcija 24) |
| 5× `push_back`, `noexcept` move | kopija=0, move=12 | 5 u vektor + 1+2+4 pri realokacijama |
| 5× `push_back`, move **bez** `noexcept` | **kopija=7**, move=5 | realokacija kopira, da bi sačuvala strong garanciju |

Poslednji red je najvažniji: move konstruktor bez `noexcept` je za
`std::vector` **kao da ne postoji**. Vektor pri realokaciji mora da
garantuje da, ako premeštanje elementa baci, stari niz ostane netaknut.
Kopija to omogućava, a move koji može da baci ne. Detaljno: lekcija 23.

---

# Korak 4 — rule of 0

```cpp
struct Buffer0 {
    explicit Buffer0(std::size_t size) : data(size, 0) {}
    std::vector<int> data;       // vector zna da se kopira, pomera i uništi
};
struct UniqueBuffer {           // isto, ali samo move (unique_ptr zabrani kopiju)
    std::unique_ptr<int[]> data;
    std::size_t size;
};
```

- **Nula** specijalnih funkcija, a ponašanje je isto: duboka kopija,
  `noexcept` move, ispravna dodela samom sebi.
- Nema šta da se zaboravi: novi član se kopira i pomera sam.
- ⚠️ Move-only član čini i klasu move-only. Kopija
  `std::vector<UniqueBuffer>` se ne kompajlira, a poruka dolazi iz dubine
  biblioteke (`errors/e02`).

| Pristup | Linija koda za specijalne funkcije | Rizik |
|---|---|---|
| rule of 5 | ~30 | zaboravljen `noexcept`, `delete`, `nullptr`, provera `this != &other` |
| rule of 0 | 0 | nema |

✅ **C.20:** rule of 0 je podrazumevani izbor. Rule of 5 samo u klasi čiji
je jedini posao upravljanje resursom (kao `std::vector` ili
`std::unique_ptr`), a ostale klase koriste takve klase kao članove.

---

# Mapa na kurs

| Nastavak kursa | Gde |
|---|---|
| 56 Rule of 5 & 0 | ova lekcija |
| 57 Copy Elision | lekcija 24 |

---

# Pravilo za praksu

✅ Rule of 0: članovi `std::vector`, `std::string`, `std::unique_ptr`.

✅ Ako ipak pišeš: svih pet, move `noexcept`, `swap` po članovima.

✅ Proveri `std::is_nothrow_move_constructible_v<T>` za tipove koji idu u
`std::vector`.

⚠️ Move dodela: obriši staro, preuzmi novo, isprazni izvor, i sve to samo
ako `this != &other`.

**Rezime:** rule of 3/5 je način da se ručno upravlja resursom kako treba,
ali svaka od pet funkcija ima svoju zamku, a jedan zaboravljen `noexcept`
tiho vrati kopiranje u `std::vector`. Kad resursom upravlja član
(`vector`, `unique_ptr`), kompajler napiše svih pet tačno. Zato je rule
of 0 cilj, a rule of 5 izuzetak.

## Zapažanja posle vežbe

