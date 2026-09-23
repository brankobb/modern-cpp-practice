# Sesija 22 — `std::optional`, `std::variant`, `std::any` (kurs 224–230)

Tri C++17 "rečnička" tipa (vocabulary types), za situacije koje su se
ranije rešavale dogovorom:

- **`optional<T>`**: vrednost **ili ništa** -- umesto `-1`, `nullptr` ili
  `bool` + izlazni parametar;
- **`variant<A, B, C>`**: **tačno jedan** od poznatih tipova -- bezbedan
  `union` koji zna šta drži;
- **`any`**: **bilo koji** tip koji se kopira -- bezbedan `void*` koji zna
  svoj tip.

Nijedan ne alocira za mali sadržaj: vrednost je unutar objekta (osim
velikog sadržaja u `any`, sekcija 7).

Već obrađeno, ovde samo upućujemo:

- izuzeci (`bad_optional_access` i ostali): s11;
- CTAD vodič i `if constexpr` (za `visit`): s21, sekcije 1 i 5;
- `using` sa paketom i variadic šabloni: s13, sekcija 2.

**Izvori:** standard, `[optional]`, `[variant]` (`[variant.visit]`,
`[variant.status]` za `valueless_by_exception`), `[any]`. Core Guidelines
**C.181** (izbegavaj "gole" `union`-e -- koristi `variant`), **F.20**
(za izlaznu vrednost vrati vrednost, ne izlazni parametar). *C++17 -- The
Complete Guide* (Josuttis), poglavlja o ova tri tipa.

**Kako vežbati:**

```
./build.sh week3-advanced/s22-optional-variant-any/main.cpp        # svi ISPRAVNI slučajevi
./check_cases.sh week3-advanced/s22-optional-variant-any           # svi POGREŠNI slučajevi
./check_exercises.sh week3-advanced/s22-optional-variant-any       # vežbe
```

- `errors/` (e01–e08): kod koji se **ne kompajlira**.
- `ub/` (u01–u02): `*` na praznom `optional`-u (hvata se samo uz
  `-D_GLIBCXX_ASSERTIONS`, u zaglavlju fajla); `get_if` bez provere.
- `runtime/` (r01–r03): `value()`, `get<>`, `any_cast` na pogrešnom
  sadržaju, bez `try`.
- `sizeof` i broj alokacija u `main.cpp` su za libstdc++ na x86-64.

---

# 1. `std::optional`: vrednost ili ništa (kurs 224)

```cpp
std::optional<int> parsiraj(const std::string& s) {
    if (/* nije broj */) return std::nullopt;
    return std::stoi(s);               // int -> optional<int>
}
if (auto r = parsiraj(ulaz)) std::cout << *r;
```

| Pristup | Prazan optional |
|---|---|
| `if (o)`, `o.has_value()` | `false` |
| `*o`, `o->clan` | **UB** -- ne proverava (`ub/u01`) |
| `o.value()` | baca `bad_optional_access` (`runtime/r01`) |
| `o.value_or(x)` | vrati `x` |

- ✅ F.20: rezultat koji može da ne postoji se **vraća** kao optional,
  umesto `bool` + izlazni parametar ili "magične" vrednosti `-1`.
- ⚠️ `*o` na praznom: ASan ne vidi ništa (memorija pripada optional-u), pa
  program tiho ispiše smeće -- ovde 0. `-D_GLIBCXX_ASSERTIONS` uključi
  proveru u libstdc++ (`ub/u01`).
- ❌ Članovi sadržaja idu preko `->`, ne `.` (`errors/e06`).

---

# 2. `optional`: pravljenje, izmena, cena (kurs 225)

- Prazan optional **ne pravi** `T`: `std::optional<Tacka>` radi i kad
  `Tacka` nema podrazumevani konstruktor (test).
- Na mestu: `o.emplace(3, 4)`, `std::optional<Tacka> u(std::in_place, 5, 6)`,
  `std::make_optional<std::string>(3, 'a')` (test: `aaa`).
- Pražnjenje: `o = std::nullopt` ili `o.reset()` -- uništi sadržaj.
- **Cena**: `sizeof(optional<int>)` je 8, `optional<double>` 16 (vrednost
  + `bool`, zaokruženo na poravnanje); 0 alokacija (test).
- ❌ `optional<int&>` ne postoji (do C++26, `errors/e01`): čuva objekat, a
  referenca nije objekat. Umesto toga pokazivač (`nullptr` je "nema").

---

# 3. `optional` u praksi (kurs 226)

- **Lenja inicijalizacija**: član `std::optional<double> kal_;` se računa
  pri prvom čitanju (test: 2 čitanja, 1 računanje).
- **Poređenje**: prazan je jednak `nullopt` i manji od svake vrednosti
  (test); `o == 0` je za prazan `false`, bez izuzetka (provereno van
  `main.cpp`).
- ⚠️ **`optional<bool>`**: `if (o)` pita **da li ima** vrednost, ne da li
  je vrednost `true`. `optional<bool> o = false` daje `if (o)` → `true`
  (test; zadatak z2). Isto za `optional<int>` = 0 i `optional<T*>` =
  `nullptr`. ✅ Piši eksplicitno: `o.has_value()`, `*o`, `o == true`.
- Kad **ne** koristiti: kad "nema" ima razlog koji treba javiti (tada
  izuzetak, ili C++23 `std::expected`); za opcioni parametar koji se samo
  čita obično je dovoljno preopterećenje ili `const T*`.

---

# 4. `std::variant`: jedno od nekoliko tipova (kurs 227)

```cpp
std::variant<int, double, std::string> v;   // drži int{} -- prvu alternativu
v = 2.5;                                     // sada double; index() == 1
std::get<double>(v);                         // baca bad_variant_access ako nije double
std::get_if<double>(&v);                     // pokazivač ili nullptr
std::holds_alternative<double>(v);           // bool
```

- Variant **uvek** drži tačno jednu alternativu; zna koju (`index()`) i
  sam uništava staru pri dodeli (C.181: umesto `union` + ručni "tag").
- `sizeof(variant<int, double>)` je 16: najveća alternativa + indeks
  (test). Bez heap-a.
- ❌ `get<T>` za tip koji nije alternativa, ili se javlja dvaput, se ne
  kompajlira (`errors/e02`, `e05`); za pogrešnu **trenutnu** alternativu
  baca (`runtime/r02`).
- ⚠️ `get_if` vraća `nullptr` -- proveri ga (`ub/u02`, UBSan: "member call
  on null pointer").
- Podrazumevani konstruktor pravi prvu alternativu; ako ona nema
  podrazumevani konstruktor, nema ga ni variant (`errors/e07`). ✅
  `std::monostate` na prvom mestu je "prazno" stanje (test).

---

# 5. `std::visit` (kurs 228)

```cpp
template <typename... F> struct Preopterecen : F... { using F::operator()...; };
template <typename... F> Preopterecen(F...) -> Preopterecen<F...>;   // C++17; C++20 ne treba

std::visit(Preopterecen{
    [](int i)                { ... },
    [](double d)             { ... },
    [](const std::string& s) { ... },
}, v);
```

- `visit` pozove granu za alternativu koju variant trenutno drži.
  `Preopterecen` spoji lambde u jedan objekat sa više `operator()`
  (nasleđivanje od paketa + `using` sa paketom, C++17).
- ✅ Zaboravljena alternativa je **greška kompajliranja** (`errors/e03`) --
  prednost nad `switch (v.index())`, gde se zaboravljen slučaj ne primeti.
- ❌ Sve grane moraju da vrate **isti tip** (`errors/e08`: `"ok"` je
  `const char*`, druga grana `std::string`; nađeno pri pisanju rešenja
  z1). ✅ `-> std::string` na svakoj lambdi.
- Jedna generička lambda + `if constexpr` (s21, sekcija 5) za "iste
  stvari za više tipova" (test).

---

# 6. `variant` u praksi (kurs 229)

- **Mašina stanja**: `using Stanje = std::variant<Ugasen, Radi, Greska>;`
  -- svako stanje nosi SVOJE podatke (brzina postoji samo u `Radi`, opis
  greške samo u `Greska`). Prelaz je `visit` koji vraća novo stanje
  (test: start → brze → brze → kvar → reset).
- Zatvoren skup tipova, poznat unapred -- alternativa nasleđivanju i
  virtuelnim funkcijama kad se tipovi ne dodaju, a operacije da (i bez
  heap-a).
- ⚠️ **`valueless_by_exception()`**: ako pri promeni alternative stara bude
  uništena, a nova baci izuzetak u konstruktoru, variant ne drži ništa;
  `index()` je `variant_npos`, `visit` baca. Test: dodela tipa čiji copy
  konstruktor baca → `true`. libstdc++ to izbegava gde može (za
  `emplace` tipa sa podrazumevanim konstruktorom koji baca, u
  `variant<int, ...>`, `variant<std::string, ...>` ili `variant<std::vector<int>, ...>`,
  ostane stara vrednost -- provereno van `main.cpp`); standard to dozvoljava,
  ali ne garantuje.
- ⚠️ Konverzija pri dodeli: `variant<bool, std::string> v = "tekst"` je u
  prvobitnom C++17 birao `bool` (pokazivač → `bool`). Ispravka P0608
  primenjena je unazad: g++ 13 i clang 18 biraju `std::string` i sa
  `-std=c++17` (provereno); stariji kompajleri mogu da izaberu `bool`.

---

# 7. `std::any` (kurs 230)

```cpp
std::map<std::string, std::any> svojstva;
svojstva["id"] = 7;
std::any_cast<int>(svojstva["id"]);        // tačno int -- inače bad_any_cast
std::any_cast<double>(&svojstva["id"]);    // pokazivač: nullptr ako tip nije tačan
svojstva["id"].type() == typeid(int);
```

- Drži bilo koji tip koji se **kopira** (`errors/e04`: `unique_ptr` ne može).
- `any_cast<T>` traži **tačno** `T`, bez konverzija: `int` nije `long`
  (`runtime/r03`). ⚠️ `"abc"` u `any` je `const char*`, ne `std::string`
  (test; zadatak z3).
- **Cena**: `sizeof(any)` je 16; mali sadržaj (`int`) je unutar objekta, 0
  alokacija; 64 B → 1 alokacija (test). Pristup ide preko provere tipa pri
  izvršavanju.
- ✅ Izbor:

| | Skup tipova | Provera | Heap |
|---|---|---|---|
| `optional<T>` | `T` ili ništa | pri izvršavanju (`has_value`) | ne |
| `variant<A, B>` | zatvoren, poznat pri kompajliranju | `visit` pokriva sve pri kompajliranju | ne |
| `any` | otvoren, bilo koji | samo pri izvršavanju (`any_cast`) | za veliki sadržaj |

  `any` samo kad skup tipova zaista nije poznat (svojstva, dodaci,
  prosleđivanje kroz sloj koji ne zna tipove); inače `variant`.

---

# Mapa na kurs

| Kurs | Tema | Ovde |
|---|---|---|
| 224 | std::optional - I | sekcija 1; `ub/u01`; `runtime/r01`; zadatak z1 |
| 225 | std::optional - II | sekcija 2; `errors/e01`, `e06` |
| 226 | std::optional - III | sekcija 3; zadatak z2 |
| 227 | std::variant - I | sekcija 4; `errors/e02`, `e05`, `e07`; `ub/u02`; `runtime/r02` |
| 228 | std::variant - II | sekcija 5; `errors/e03`, `e08`; zadatak z1 |
| 229 | std::variant - III | sekcija 6 |
| 230 | std::any | sekcija 7; `errors/e04`; `runtime/r03`; zadatak z3 |

---

# Pravilo za praksu

✅ `optional<T>` kao povratna vrednost kad rezultat može da ne postoji
(F.20); `value_or` ili `if (o)` pre `*o`.

⚠️ `optional<bool>` (i `<int>`, `<T*>`): `if (o)` je "ima li vrednost".

✅ `variant` + `visit` umesto `union`-a i umesto `switch` po oznaci tipa
(C.181); isti povratni tip iz svih grana.

✅ `get_if` + provera, ili `visit` -- `get` samo kad je alternativa sigurno
poznata.

⚠️ `any` samo za zaista otvoren skup tipova; `any_cast` traži tačan tip
(pazi na literale).

**Rezime:** `optional` je "možda vrednost", `variant` je "jedan od ovih
tipova", `any` je "nešto, proveri šta". Sva tri čuvaju vrednost u sebi i
znaju šta drže -- razlika je u tome koliko se o tipu zna pri
kompajliranju: kod `variant`-a sve, kod `any`-ja ništa.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_komande_senzora`](exercises/z1_komande_senzora.cpp) | upotreba | optional za "možda uspe", variant za komande, visit za izvršavanje (sekcije 1, 4, 5) | — |
| [`z2_optional_bool`](exercises/z2_optional_bool.cpp) | zašto | zašto if (o) nije isto što i if (*o) za optional<bool> (sekcija 3) | `-DNAIVNO` |
| [`z3_any_literal`](exercises/z3_any_literal.cpp) | zašto | zašto any_cast<std::string> ne uspe na "temp" (sekcija 7) | `-DNAIVNO` |

## Zapažanja posle vežbe
