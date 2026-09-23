# Sesija 7 — Copy elision, vraćanje i parametri

Koliko objekata zaista nastane kad funkcija vrati objekat po vrednosti,
ili kad se objekat prosledi po vrednosti? Često **nijedan višak**:
kompajler sme (a od C++17 u nekim slučajevima i mora) da izostavi kopiju i
move. Ova sesija pokazuje kada, kada ne, i šta iz toga sledi za pisanje
funkcija.

**Izvori:** standard, delovi `[class.copy.elision]`, `[basic.lval]`
(materijalizacija privremenih) i `[stmt.return]`. P0135 (garantovana
elizija, C++17) i P1825 (šire automatsko pomeranje, C++20). Uz to
*Effective Modern C++* **Item 25** (ne `std::move` u return lokalne),
**Item 41** (parametar po vrednosti) i **Item 42** (emplace), i C++ Core
Guidelines **F.15–F.18**, **F.48**. Nastavak kursa 57.

**Kako vežbati:**

```
./build.sh week2-modern-layer/s07-copy-elision/main.cpp
./check_cases.sh week2-modern-layer/s07-copy-elision
```

- `errors/` (e01–e03): kod koji se **ne kompajlira**. Ova sesija nema
  `ub/`: greške ovde su suvišne kopije, a ne UB.

---

# 1. Vraćanje po vrednosti

| Kod | Test (g++ i clang, `-O0`) | Pravilo |
|---|---|---|
| `return Logged();` | `ctor` | **RVO, garantovan od C++17**: prvalue se pravi direktno u odredištu |
| `Logged x; ... return x;` | `ctor` | **NRVO**: dozvoljen, nije garantovan. g++ i clang ga rade i na `-O0` |
| `return std::move(x);` | `ctor move` | ❌ NRVO onemogućen: vraća se referenca, pa mora move |
| `Logged a = Logged(Logged(Logged()));` | `ctor` | prvalue se ne materijalizuje dok ne mora |

- ⚠️ **Ne piši `return std::move(local);`** (EMC Item 25). U najboljem
  slučaju ne pomaže, a obično pokvari NRVO. g++ i clang sa `-Wall`
  upozore (`-Wpessimizing-move`). U `main.cpp` je upozorenje namerno
  isključeno oko jedne funkcije, da bi se video efekat.
- Sa `-fno-elide-constructors` (g++) NRVO nestaje (`ctor move`), a RVO
  ostaje, jer je u C++17 obavezan (test).

**Garantovana elizija (C++17)** nije samo optimizacija: tip koji nema ni
kopiju ni move (npr. sa `std::mutex` članom) sme da se vrati po vrednosti
iz `return Guarded{};`. U C++14 to je greška, jer je konstruktor morao da
postoji i kad se ne pozove (`errors/e01`).

---

# 2. Kad elizije nema: automatski move

| `return` čega | Test | Zašto |
|---|---|---|
| jedne od dve lokalne (zavisno od uslova) | `ctor ctor move` | NRVO nemoguć, ali lokalna se **automatski pomera** |
| parametra po vrednosti | `ctor move` | parametar nije u okviru funkcije, pa elizija ne može, ali se pomera |
| člana objekta | `ctor copy` | član živi i posle funkcije: **kopija** (za move-only greška, `errors/e02`) |
| parametra `T&& r` | g++ C++17: `copy`; clang C++17 i oba u C++20: `move` | C++20 (P1825) proširio automatski move; clang to primenjuje i u C++17 |

Pravilo: **lokalna promenljiva ili parametar po vrednosti** se na
`return` sami pomeraju, bez `std::move`. Za sve ostalo (član, globalna,
referenca) ide kopija, osim ako eksplicitno napišeš `std::move`, i tada
je to svesna odluka da isprazniš izvor.

---

# 3. Parametri: koji oblik izabrati (F.15–F.18)

| Šta funkcija radi sa argumentom | Parametar |
|---|---|
| samo čita, jeftino za kopiju (`int`, `double`, mali struct) | `T` |
| samo čita, skupo za kopiju | `const T&` |
| menja argument pozivaoca | `T&` |
| **čuva kopiju** (sink): konstruktor, setter, `push_back` | `T` po vrednosti + `std::move`, ili par `const T&` / `T&&` |
| preuzima vlasništvo move-only tipa | `T` po vrednosti (`std::unique_ptr<X> p`) |
| prosleđuje dalje bez promene (šablon) | `T&&` + `std::forward` (s09) |

**Sink parametar (EMC Item 41)**, test iz `main.cpp`:

| | lvalue argument | rvalue argument |
|---|---|---|
| `setName(Logged name)` + `std::move` | `copy move=` | `ctor move=` |
| `setNameRef(const Logged&)` / `(Logged&&)` | `copy=` | `ctor move=` |

Po vrednosti je **jedan move skuplje** za lvalue, a isto za rvalue. Zauzvrat
je jedna funkcija umesto dve. Kod više parametara razlika raste: dva
overload-a po parametru daju 2ⁿ funkcija.

✅ Po vrednosti kad je move jeftin i kad funkcija **uvek** čuva argument.
Ne kad ga čuva samo ponekad (tada bi se kopija platila i kad ne treba),
i ne za tipove gde je move skup (`std::array`, EMC Item 29).

Move-only tip po vrednosti znači "funkcija preuzima vlasništvo", i
pozivalac mora da napiše `std::move` (`errors/e03`). To je dobro: iz
poziva se vidi da `p` više nije vlasnik.

---

# 4. `emplace_back` vs `push_back` (EMC Item 42)

```
push_back(Logged("x"))  -> ctor(str) move
emplace_back("x")        -> ctor(str)
```

`emplace_back` prosleđuje argumente konstruktoru i pravi objekat
**direktno u vektoru**. `push_back` prima gotov objekat, pa se privremeni
napravi i onda pomeri.

⚠️ `emplace_back` poziva i **explicit** konstruktore (`v.emplace_back(5)`
za `std::vector<std::vector<int>>` napravi vektor od 5 elemenata), pa je
`push_back` bolji kad već imaš objekat ili kad hoćeš proveru tipova.

---

# Mapa na kurs

| Nastavak kursa | Sekcija |
|---|---|
| 57 Copy Elision | 1, 2 |

---

# Pravilo za praksu

✅ Vraćaj po vrednosti. `return local;` bez `std::move`.

✅ `std::move` u `return` samo za član ili nešto što nije lokalno, i samo
kad ga namerno prazniš.

✅ Sink parametri po vrednosti + `std::move` kad je move jeftin.

✅ Move-only tipovi po vrednosti znače predaju vlasništva.

⚠️ `-Wpessimizing-move` i `-Wredundant-move` su upozorenja koja treba
poslušati.

**Rezime:** C++17 garantuje da `return T(...)` ne pravi ni kopiju ni move,
a kompajleri u praksi izostave i kopiju imenovane lokalne promenljive.
Kad elizija nije moguća, lokalne promenljive i parametri se na `return`
pomeraju sami. Zato je vraćanje po vrednosti jeftino, a ručni
`std::move` u `return`-u može samo da šteti.

## Vežbe

Zadaci su u `exercises/`, rešenja u `exercises/solutions/`. Svaki zadatak
se kompajlira i nerešen; koraci su u komentaru na vrhu, testovi su
zakomentarisani u `main()`, a na dnu je blok OČEKIVANI IZLAZ. Zadaci
"zašto" prvo pokažu problem: build sa navedenim `-D` makroom (npr.
`./build.sh <zadatak>.cpp -DNAIVNO`). Sve zadatke i rešenja proverava
`./check_exercises.sh <lekcija>`.

| Zadatak | Vrsta | Tema | Demonstracija problema |
|---|---|---|---|
| [`z1_parametri_i_povratak`](exercises/z1_parametri_i_povratak.cpp) | upotreba | vraćanje po vrednosti, sink parametar, emplace_back (sekcije 1, 3, 4) | — |
| [`z2_return_std_move`](exercises/z2_return_std_move.cpp) | zašto | zašto NE pisati return std::move(lokalna) (sekcije 1, 2) | `-DNAIVNO` |
| [`z3_const_lokalna`](exercises/z3_const_lokalna.cpp) | zašto | zašto lokalna koju vraćaš ne treba da bude const (sekcija 2) | `-DNAIVNO` |

## Zapažanja posle vežbe

