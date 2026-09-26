# Lekcija 15 — Operator overloading

Operator za sopstveni tip je obična funkcija sa posebnim imenom
(`operator+`). Lekcija pokazuje kako se pišu najčešći operatori, kada su
član klase a kada slobodna funkcija, i koja pravila jezik nameće.

**Izvori:** standard, delovi `[over.oper]` (šta se sme preopteretiti),
`[over.binary]`, `[over.ass]`, `[over.sub]`, `[over.call]`, `[over.ref]`,
`[over.inc]` i `[class.compare]` (C++20 `<=>` i `= default`). Uz to
*Effective C++* **Item 10** (`operator=` vraća `*this`), **Item 11**
(dodela samom sebi), **Item 23** (slobodne funkcije umesto članova) i
**Item 24** (slobodna funkcija kad konverzija treba za oba operanda), i
C++ Core Guidelines **C.160–C.170**.

**Kako vežbati:**

```
./build.sh 2-classes/15-operator-overloading/main.cpp                   # ISPRAVNI slučajevi
./build.sh 2-classes/15-operator-overloading/main_cpp20.cpp -std=c++20  # C++20 poređenje
./check_cases.sh 2-classes/15-operator-overloading                      # POGREŠNI slučajevi
```

- `errors/` (e01–e11): kod koji se **ne kompajlira**.
- `ub/` (u01–u03): kod koji se kompajlira, a ASan/UBSan ga hvata.

---

# 1. Operator je funkcija

```cpp
Money sum = a + b;            // kompajler to prevede u jedno od:
Money sum = operator+(a, b);  //   slobodna funkcija
Money sum = a.operator+(b);   //   član klase
```

Pravila (`[over.oper]`):

| Pravilo | Primer greške |
|---|---|
| Bar jedan operand je klasa ili enum | `int operator+(int, int)` (`errors/e02`) |
| Broj operanada je isti kao kod ugrađenog | `operator+` sa 3 parametra (`errors/e04`) |
| Prioritet i asocijativnost se ne menjaju | `a + b * c` je uvek `a + (b * c)` |
| Ne mogu se izmisliti novi operatori | `operator**` |
| Neki se ne mogu preopteretiti | `?:`, `.`, `.*`, `::`, `sizeof`, `typeid` (`errors/e01`) |
| `=`, `()`, `[]`, `->` moraju biti članovi | `operator=` kao slobodna funkcija (`errors/e05`) |

---

# 2. Član ili slobodna funkcija

| Operator | Gde | Zašto |
|---|---|---|
| `=`, `[]`, `()`, `->` | **član** | jezik to traži |
| `+=`, `-=`, `*=`, `++`, `--` | **član** | menjaju levi operand, vraćaju `*this&` |
| `+`, `-`, `*`, `==`, `<` | **slobodna** (često `friend`) | simetrični: konverzija radi za oba operanda |
| `<<`, `>>` za stream | **slobodna** | levi operand je `std::ostream`, a ne tvoja klasa |

**Kanonski oblik** (C.161): `+=` je član, a `+` slobodna funkcija
napisana preko `+=`. Logika je na jednom mestu:

```cpp
Money& Money::operator+=(const Money& rhs) { cents_ += rhs.cents_; return *this; }
Money operator+(Money lhs, const Money& rhs) { lhs += rhs; return lhs; }   // lhs je već kopija
```

**EC++ Item 24:** član se poziva na **levom** operandu, a konverzije se
primenjuju samo na argumente. Sa `Rational(int)` konstruktorom:

| | `third * 2` | `2 * third` |
|---|---|---|
| `operator*` kao član | ✅ `2` → `Rational(2)` | ❌ `int` nema članove (`errors/e03`) |
| `operator*` kao slobodna funkcija | ✅ | ✅ |

---

# 3. `friend`

Kad slobodni operator treba pristup `private` delu, klasa ga proglasi
prijateljem. `friend` funkcija **nije član**: nema `this` i poziva se kao
obična funkcija.

```cpp
class Temperature {
    friend std::ostream& operator<<(std::ostream& os, const Temperature& t) {
        return os << t.celsius_ << " C";
    }
private:
    double celsius_;
};
```

✅ **Hidden friend:** kad je `friend` operator **definisan u klasi**,
pronalazi ga samo ADL (lekcija 08), tj. samo kad je bar jedan argument
`Temperature`. Ne pojavljuje se kao kandidat u tuđim izrazima. To je
preporučen oblik za operatore.

⚠️ `friend` narušava enkapsulaciju isto kao član. Ako operator može da se
napiše preko javnog interfejsa (`r.num()`, `r.den()`), ne treba mu
`friend` (EC++ Item 23). Prijateljstvo se ne nasleđuje i nije
tranzitivno.

---

# 4. `operator=` (dodela)

```cpp
Name& operator=(const Name& other) {
    Name copy(other);              // 1) napravi kopiju (ako baci, *this je netaknut)
    std::swap(data_, copy.data_);  // 2) zameni sadržaj
    return *this;                  // 3) vrati sebe po referenci
}
```

- **Vraća `*this` po referenci** (EC++ Item 10), da bi radilo `a = b = c`
  (dodela je desno asocijativna: `a = (b = c)`).
- **Mora raditi i za `a = a`** (EC++ Item 11). Naivna verzija "prvo
  `delete[] data_`, pa kopiraj `other.data_`" čita oslobođenu memoriju kad
  je `other` isti objekat (`ub/u01`). Dodela samom sebi retko izgleda kao
  `a = a` (tada clang `-Wall` upozori, g++ ne), češće je `v[i] = v[j]` sa
  `i == j`.
- **Copy-and-swap** rešava oba problema odjednom. Detaljno, uz rule of 3 i
  move dodelu: lekcije 20–22.
- Kompajler sam pravi `operator=` koji dodeljuje član po član. Za klase sa
  `std::string`, `std::vector` i sl. to je dovoljno (rule of 0).

---

# 5. Poređenje

**C++17:** šest operatora, svi preko `==` i `<` (C.162: operatori koji
idu zajedno se pišu zajedno):

```cpp
bool operator==(const Version& a, const Version& b) { return a.major == b.major && a.minor == b.minor; }
bool operator<(const Version& a, const Version& b) {
    return std::tie(a.major, a.minor) < std::tie(b.major, b.minor);   // redom, kao rečnik
}
bool operator!=(...) { return !(a == b); }
bool operator> (...) { return b < a; }
bool operator<=(...) { return !(b < a); }
bool operator>=(...) { return !(a < b); }
```

**C++20** (`main_cpp20.cpp`):

```cpp
struct Version {
    int major, minor;
    auto operator<=>(const Version&) const = default;   // svih šest, član po član
};
```

| C++20 pravilo | Posledica |
|---|---|
| `<=>` vraća `strong_ordering`, `weak_ordering` ili `partial_ordering` | `a <=> b` je `< 0`, `== 0` ili `> 0` |
| `<`, `>`, `<=`, `>=` se prepisuju u `(a <=> b) < 0` itd. | ne pišu se ručno |
| `!=` se prepisuje u `!(a == b)` | ne piše se ručno |
| obrnut redosled: `5 == m` → `m == 5` | jedan operator pokriva obe strane (u C++17 greška, `errors/e11`) |
| `= default` za `<=>` daje i `==` | jedna linija za sve |
| **ručno** napisan `<=>` **ne daje** `==` | `a == b` se ne kompajlira (`errors/e08`) |
| `double` član → `partial_ordering` | `1.0 <=> NaN` je `unordered` |
| `= default` za poređenje u C++17 | ❌ ne postoji (`errors/e07`) |

`weak_ordering` se koristi kad su vrednosti **ekvivalentne** a nisu iste,
npr. `Username("Ana")` i `Username("ANA")` kod poređenja bez obzira na
velika i mala slova.

---

# 6. `operator<<` i `operator>>` za stream

```cpp
std::ostream& operator<<(std::ostream& os, const Rational& r) { return os << r.num() << "/" << r.den(); }
std::istream& operator>>(std::istream& is, Rational& r);
```

- Uvek **slobodna** funkcija. Kao član bi se pisalo `v << std::cout`
  (`errors/e06`).
- Vraća **stream po referenci**, da bi radilo `std::cout << a << b`.
- `>>` na lošem ulazu postavi `failbit` (`is.setstate(std::ios::failbit)`)
  i **ne menja** objekat. Test: `"6x8"` ostavlja `5/7` i `fail()` je
  `true`.

---

# 7. `operator[]`

```cpp
int& operator[](int i);               // za pisanje i čitanje
const int& operator[](int i) const;   // za const objekte
```

- Vraća **referencu**, da bi `g[0] = 4` menjao element.
- Treba mu i **`const` verzija**. Bez nje se `const Grid&` ne može čitati
  (`errors/e10`).
- Konvencija standardne biblioteke: `[]` **ne proverava** granice (brzo),
  a `at()` proverava i baca `std::out_of_range`. `[]` van granica je UB kao
  kod običnog niza (`ub/u02`).
- C++23 dozvoljava više argumenata (`m[r, c]`). Do tada: `m(r, c)` preko
  `operator()`, ili `at(r, c)`.

---

# 8. `++` i `--`: prefiks i postfiks

```cpp
Counter& operator++();      // ++c: uveća, vrati sebe
Counter  operator++(int);   // c++: int je samo oznaka (errors/e09); vraća STARU vrednost
```

Postfiks se piše preko prefiksa i pravi kopiju. Zato se u petljama sa
iteratorima piše `++it`: za klase je prefiks jeftiniji, a za `int` je
isto.

---

# 9. `operator()`: funkcijski objekti

Klasa sa `operator()` se poziva kao funkcija (`byLength(a, b)`), a za
razliku od funkcije može da ima **stanje**. Algoritmi (`std::sort`,
`std::find_if`) primaju takve objekte. **Lambda je upravo to:** kompajler
napravi klasu čiji je `operator()` telo lambde (lekcija 09, sekcija 11;
lekcija 11).

`operator()` je jedini operator sa proizvoljnim brojem parametara.

---

# 10. `operator*` i `operator->`: pametni pokazivač

```cpp
T& operator*() const  { return *p_; }
T* operator->() const { return p_; }   // ptr->f() postaje (ptr.operator->())->f()
```

- `operator->` vraća sirov pokazivač (ili objekat koji i sam ima `->`), a
  jezik onda ponovo primeni `->` na rezultat.
- Uz `~ScopedPtr() { delete p_; }` i zabranjeno kopiranje, to je
  najjednostavniji vlasnički pametni pokazivač. Pravi `std::unique_ptr`,
  move i sopstveni `UniquePtr`: lekcije 32 i 33.
- ⚠️ `->` na praznom pokazivaču je UB (`ub/u03`). Ni `std::unique_ptr` to
  ne proverava.
- `explicit operator bool` za `if (ptr)`: lekcija 17 (konverzije).

---

# 11. Pravila i šta ne preopterećivati

✅ **Operator radi ono što izgleda da radi** (C.167). `+` sabira, `==`
poredi. Ako nije očigledno, bolja je funkcija sa imenom.

✅ Operatori koji idu zajedno se pišu zajedno i dosledno: `+` i `+=`, `==`
i `!=`, `<` i ostali (C.162), `[]` i `const []`.

❌ **Ne preopterećuj `&&`, `||` i `,`.** Preopterećen operator je obična
funkcija, pa se **oba** operanda izračunaju pre poziva. Test iz
`main.cpp`: ugrađeni `false && x` ne izračuna `x` (0 poziva), a
preopterećen `Flag && Flag` izračuna oba (2 poziva).

❌ **Ne preopterećuj unarni `&`** (adresa). Tada `&obj` ne vraća adresu, a
generički kod mora da koristi `std::addressof`.

---

# Mapa na kurs

| Nastavak kursa | Sekcija |
|---|---|
| 60 Basics | 1, 2 |
| 61 Assignment Operator | 4 (i lekcije 20–22) |
| 62 Global Overloads | 2, 5, 6 |
| 63 Friend Keyword | 3 |
| 64–65 Smart Pointer Basics / C++11 | 10 (i lekcije 32, 33) |
| 66 Rules | 1, 11 |

---

# Pravilo za praksu

✅ `+=` kao član, `+` kao slobodna funkcija preko `+=`.

✅ Simetrični operatori (`+`, `==`, `<`) i `<<`/`>>` su slobodne funkcije,
najbolje hidden friend.

✅ `operator=` vraća `*this&` i radi za `a = a` (copy-and-swap).

✅ C++20: `auto operator<=>(const T&) const = default;` umesto šest
operatora.

✅ `operator[]` u dve verzije: `const` i ne-`const`.

⚠️ Ručni `<=>` ne daje `==`.

⚠️ `&&`, `||`, `,` i unarni `&` se ne preopterećuju.

**Rezime:** operator je funkcija koju kompajler zove kad vidi simbol.
Jezik fiksira broj operanada, prioritet i koje operatore smeš da
preopteretiš, a ti biraš gde je funkcija (član ako menja levi operand ili
jezik to traži, inače slobodna) i da li ponašanje odgovara onome što
simbol obećava.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok EXPECTED OUTPUT. Zadaci
"why" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVE`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`ex1_razlomak`](exercises/ex1_razlomak.cpp) | usage | aritmetički operatori, poređenje i ispis (sekcije 2, 5, 6, 11) | — |
| [`ex2_opseg_iterator`](exercises/ex2_opseg_iterator.cpp) | usage | sopstveni iterator: *, ++, != i range-for; operator[] i operator() (sekcije 7, 8, 9) | — |
| [`ex3_strogo_manje`](exercises/ex3_strogo_manje.cpp) | why | zašto operator< mora biti STROG (sekcija 5) | `-DNAIVE`, `-DNAIVE_SORT` |

## Zapažanja posle vežbe

