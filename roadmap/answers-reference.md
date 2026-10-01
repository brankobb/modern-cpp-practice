# Referentni odgovori — Korak 1 i 2

Otvori tek kad napišeš svoje u `questions.md`. Poenta nije da se poklopi
tekst, nego da tvoj odgovor sadrži isti **razlog** (ono posle "jer").

## 1. Copy ctor vs `operator=`

Copy ctor se poziva kad se **pravi novi** objekat iz postojećeg:
`T b = a;`, `T b{a};`, prosleđivanje po vrednosti, vraćanje po vrednosti
(kad nema elizije ni move-a), hvatanje izuzetka po vrednosti.
`operator=` se poziva nad objektom koji **već postoji**: `b = a;`.

Razlika nije sintaksna nego suštinska: konstruktor nema staro stanje,
dodela ga ima — mora da oslobodi stari resurs i da preživi `a = a`.
`T b = a;` je inicijalizacija, ne dodela, uprkos znaku `=`.

## 2. Virtualan destruktor

Ako se objekat `Derived` briše preko `Base*` (`delete base_ptr;`), a
`~Base` nije virtualan, to je **undefined behavior**. U praksi: poziva se
samo `~Base`, pa resursi koje drži `Derived` procure, a operator `delete`
dobije pogrešnu veličinu objekta (ASan: `new-delete-type-mismatch`).
Virtualan destruktor ide preko vtable-a i poziva `~Derived` pa `~Base`.

Pravilo (C.35): destruktor bazne klase je ili `public virtual`, ili
`protected` i nevirtualan (onda brisanje preko baze ne može ni da se napiše).

## 3. `T x;` / `T x{};` / `T x();`

- `T x;` — default-initialization. Za klasu: poziva se default ctor. Za
  `int`, pokazivač, POD `struct` u funkciji: vrednost je **neodređena**, a
  čitanje je UB.
- `T x{};` — value-initialization. Za skalar: nula. Za klasu sa
  korisničkim default ctor-om: taj ctor. Za klasu bez njega: prvo sve
  nule, pa ctor. **Podrazumevani izbor.**
- `T x();` — **nije promenljiva**: deklaracija funkcije `x` bez
  parametara koja vraća `T` (most vexing parse). Greška se vidi tek kad
  pokušaš da koristiš `x` kao objekat.

## 4. Copy elision

Elision = kompajler konstruiše objekat **direktno na odredištu**, bez
privremenog i bez copy/move poziva — čak i ako copy/move ctor ima
vidljive sporedne efekte (printf).

- `Buffer b = a;` (`a` je `Buffer` lvalue) — uvek **tačno jedan** copy ctor,
  u svakom standardu. Tu nema šta da se eliminiše.
- `Buffer b = Buffer(10);`, `Buffer b = make();` (`return Buffer{10};`),
  `Buffer b = 10;` (ne-`explicit` ctor) — pre C++17 jezik je formalno
  zahtevao privremeni objekat pa copy/move u `b` (dva poziva; kroz
  `return` čak tri). Elizija je bila **dozvoljena, ne obavezna** — pa je
  copy/move ctor morao da postoji i bude dostupan, čak i kad ga kompajler
  ne pozove. Sa `-fno-elide-constructors` pod C++11 vidiš: `ctor move`,
  a za `make()`: `ctor move move`.
- Od C++17 to je **garantovano**: prvalue (`Buffer(10)`, rezultat
  `make()`) se više ne "kopira", nego inicijalizuje odredište. Jedan
  `ctor`, i radi čak i za tip koji nema copy ni move.
- `return local;` (NRVO, imenovan objekat) i dalje **nije** garantovan ni u
  C++17: ako kompajler ne eliminiše, radi se **move** (ne copy).

Izlazi iznad su provereni sa g++ 13. Detalji i razlika g++/clang:
`3-lifetime-and-resources/24-copy-elision/notes.md` (sekcije 1 i 2).

## 5. Self-assignment

`a = a;` — objekat dodeljen samom sebi. Retko se piše doslovno, ali se
dešava preko aliasa: `v[i] = v[j]` kad je `i == j`, `*p = *q` kad pokazuju
na isto, `std::swap` / algoritmi koji dodeljuju elemente.

Opasan je jer naivna dodela prvo oslobodi svoj resurs, a to je **i resurs
izvora**. Posle toga kopira iz oslobođene (ili upravo dobijene,
neinicijalizovane) memorije. Rešenja: kopiraj u novu memoriju **pre**
brisanja stare, ili copy-and-swap; provera `if (this == &other)` radi, ali
sama po sebi ne daje exception safety.

## 6. `explicit`

Konstruktor sa jednim argumentom je i **implicitna konverzija**. Bez
`explicit`, `void f(Buffer); f(10);` se kompajlira i tiho napravi bafer od
10 bajtova; `Celsius c = 36.6;` prođe bez da se vidi jedinica. `explicit`
traži da namera bude napisana: `f(Buffer{10})`. Pravilo (C.46): svaki
konstruktor sa jednim argumentom je `explicit`, osim kad je konverzija
prirodna i bez gubitka značenja.

## 7. `const T&` vs `T&&`

- `const T&` vezuje se za **sve**: lvalue, rvalue, const, ne-const.
  Obećava: "samo čitam". Pošto je izvor možda nečiji živ objekat, sme
  samo da ga kopira.
- `T&&` vezuje se **samo za rvalue** (privremeni ili `std::move(x)`).
  Poruka: "izvor ti više ne treba — smeš da mu uzmeš resurse". Zato move
  ctor uzima `T&&` i ostavlja izvor prazan.

Overload `f(const T&)` + `f(T&&)` bira kopiranje ili pomeranje prema
kategoriji argumenta, pri kompajliranju. (U šablonu `template<class U>
void f(U&&)` je to nešto drugo — forwarding referenca, Korak 3/4.)

## 8. Redosled inicijalizacije članova

Po **redosledu deklaracije** u klasi. Redosled u inicijalizacionoj listi
konstruktora se ignoriše (kompajler upozori sa `-Wreorder`). Baze se
inicijalizuju pre članova, redom kojim su navedene u listi nasleđivanja.
Destrukcija ide tačno obrnuto. Posledica: član sme da se inicijalizuje
samo iz članova deklarisanih **iznad** njega.

## 9. Rule of 0 vs Rule of 5

Rule of 5 znači pet ručno pisanih funkcija koje moraju međusobno da se
slažu (self-assignment, moved-from stanje, `noexcept`, exception safety) —
pet mesta za bag, i svaki novi član znači pet izmena (zaboravljen član u
copy ctor-u = tihi bag). Rule of 0: resurs drži jedna mala, testirana
klasa (`unique_ptr`, `vector`, `File`), a kompajler za sve ostale generiše
ispravne operacije, sa ispravnim `noexcept`. Manje koda, nema
zaboravljenih članova, i klasa automatski postaje move-only ili
kopirajuća prema svojim članovima.

## 10. Slicing

Kad se `Derived` kopira u objekat tipa `Base` **po vrednosti**
(`Base b = derived;`, `void f(Base b)`, `std::vector<Base>`), kopira se
samo `Base` podobjekat — izvedeni članovi se odbace, a vptr kopije
pokazuje na vtable od `Base`. Virtualni pozivi nad kopijom idu na `Base`
verziju. Opasno jer se kompajlira bez greške i bez upozorenja, a ponašanje
je tiho pogrešno (`demos.cpp`, Korak 2: "by value: ..."). Zaštita:
polimorfne tipove prosleđuj po referenci/pokazivaču, a u bazi zabrani
javnu kopiju (C.67); kad treba kopija, virtuelni `clone()`.
