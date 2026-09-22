# 04 — Pokazivači i reference (kompletan pregled)

Izvori: Effective Modern C++ Item 8 ("Prefer nullptr to 0 and NULL");
Effective C++ Item 20 ("Prefer pass-by-reference-to-const to
pass-by-value"), Item 21 ("Don't try to return a reference when you must
return an object"). main.cpp prati ovu strukturu, svaka tvrdnja testirana
kompajliranjem/pokretanjem pre nego što je ušla ovde.

## 1. Pokazivač — osnove

```cpp
int x = 5;
int* p = &x;   // & -- uzmi adresu
int y = *p;    // * -- dereferenciraj (pristupi vrednosti na adresi)
void* vp = p;  // generički pokazivač -- MORA se cast-ovati pre dereferenciranja
```

Pokazivač je TIPIZIRAN — `int*` i `char*` su različiti tipovi, iako oba
"samo" čuvaju adresu; tip određuje KOLIKO bajtova se čita/piše pri
dereferenciranju i za koliko se pomera pri aritmetici.

## 2. `nullptr` vs `NULL` vs `0` (EMC Item 8)

```cpp
void f(int);
void f(char*);

f(0);        // poziva f(int) -- 0 je int literal
f(NULL);     // TESTIRANO: AMBIGUOUS COMPILE ERROR na g++ -- NULL nije
             // garantovano pokazivačkog tipa, kompajler ne zna koji f()
f(nullptr);  // UVEK poziva f(char*) -- nullptr ima sopstveni tip (std::nullptr_t)
             // koji se implicitno konvertuje u BILO KOJI pokazivački tip,
             // ali NE u int
```

`nullptr` postoji BAŠ zbog ovog problema — `NULL` je istorijski makro
(obično `0` ili `0L`), pa se u overload resolution-u ponaša kao broj, ne
kao pokazivač, i to je izvor tihih bugova ili (kao gore) compile errora.
**Pravilo:** uvek koristi `nullptr` za pokazivače u novom kodu.

## 3. Pointer arithmetic

```cpp
int arr[5] = {1,2,3,4,5};
int* p = arr;
p + 1;        // pomera se za sizeof(int) bajtova, NE za 1 bajt
p - arr;      // ptrdiff_t -- razlika u ELEMENTIMA, ne bajtovima
```

- `p + n` je validno SAMO unutar niza ili tačno JEDAN element PROŠAO kraj
  (za poređenje, npr. `p == arr + 5`); dereferenciranje `arr + 5` je UB
- poređenje pokazivača iz RAZLIČITIH nizova (`p1 < p2` gde p1, p2 nisu iz
  istog niza) je UB, čak i ako "slučajno" radi na tvom kompajleru

## 4. Pokazivač na pokazivač, referenca na pokazivač

```cpp
int x = 5;
int* p = &x;
int** pp = &p;    // pokazivač na pokazivač -- validno, česta upotreba: out-parametar koji menja sam pokazivač
int*& rp = p;     // REFERENCA na pokazivač -- takođe validno
```

Referenca NA REFERENCU ne postoji kao tip (`int& &` se "kolabira" u `int&`
u template kontekstu — videćeš u week2 s09), niti postoji niz referenci
(`int& arr[3];` je compile error) — referenca MORA imati identitet kroz
JEDNU konkretnu promenljivu/slot, ne kroz niz slotova.

## 5. Referenca — osnove

- MORA se inicijalizovati pri deklaraciji, ne može se kasnije "rebindovati"
  na drugi objekat (`ref = y;` je ASSIGNMENT, ne rebind)
- ne postoji "null referenca" u ispravnom kodu — ali referenca MOŽE
  danglovati (referisati na uništen objekat), što se ponaša slično null-u
  kad ga dereferenciraš (UB)
- `sizeof(ref)` daje veličinu REFERISANOG tipa, ne veličinu "referencе
  same po sebi" — testirano: `sizeof(Big&)` unutar funkcije daje 100 (za
  `struct Big { char buf[100]; }`), ne veličinu pokazivača
- adresa reference (`&ref`) je ISTA kao adresa objekta na koji referiše —
  testirano, `&x == &rx` gde je `int& rx = x;`

## 6. Prosleđivanje parametara: vrednost vs pokazivač vs referenca (EC++ Item 20)

| Način | Kopira? | Može biti "prazno"/opciono? | Kad koristiti |
|---|---|---|---|
| `T` (po vrednosti) | DA | ne | mali/jeftini tipovi (`int`, iteratori, `shared_ptr` kad MORAŠ da deliš vlasništvo) |
| `const T&` | NE | ne | DEFAULT izbor za veće objekte koje samo čitaš |
| `T&` (non-const) | NE | ne | "out" parametar koji funkcija menja (ređe idiomatično od `return`) |
| `T*` / `const T*` | NE | DA (`nullptr`) | kad parametar STVARNO može izostati, ili kad menjaš na šta pokazivač pokazuje |

Prosleđivanje velikog objekta PO VREDNOSTI kopira ceo objekat na svaki
poziv — `const T&` izbegava tu kopiju dok i dalje garantuje da funkcija ne
menja original.

## 7. Vraćanje reference/pokazivača (EC++ Item 21)

```cpp
int& danglingRef() {
    int local = 42;
    return local; // NIKAD -- local nestaje kad funkcija vrati, referenca dangluje
}
```

- **NIKAD** ne vraćaj referencu/pokazivač na LOKALNU promenljivu funkcije
  (testirano: UBSan prijavljuje "reference binding to null pointer",
  ASan puca na sledećem čitanju)
- **OK** je vratiti referencu na ČLAN objekta (npr. `operator[]`) — objekat
  živi DUŽE od poziva funkcije, referenca ostaje validna dok objekat živi
- **OK** je vratiti `*this` (referenca na sam objekat) za method chaining
  (`operator=`, `operator<<`) — videćeš ovo u week1 s03 (copy-and-swap)

## Zapažanja posle vežbe
