# Završna vežba dela 5 — Sistem događaja

Dispečer događaja kakav ima skoro svaki uređaj ili GUI: delovi programa se
**pretplate** na temu ("temp", "alarm") funkcijom koju treba pozvati, a
drugi delovi **objave** poruku i ne znaju ko je sluša. Pretplate su
`std::function`, pravljene lambdama sa različitim capture-ima. Težište je
na životnom veku: šta lambda hvata, koliko to živi i šta se desi kad se
spisak rukovalaca menja usred objave.

**Kako raditi:**

```
./build.sh 5-funkcije-kao-vrednosti/zavrsna-vezba/zadatak.cpp        # tvoj kod
./check_exercises.sh 5-funkcije-kao-vrednosti/zavrsna-vezba          # provera: zadatak i rešenje
```

Koraci su u komentaru na vrhu `zadatak.cpp`; `main()` je napisan i
zakomentarisan po koracima. Rešenje je u `resenje.cpp`. Očekuj 2 sedenja.

---

## Šta vežba spaja

| Korak | Šta radiš | Lekcija |
|---|---|---|
| 1 | `std::function` kao pretplata; `pretplati`, `odjavi`, `objavi` | lekcija 31, sekcija 1 |
| 1 | lambde sa `[&brojac]` i `[prefiks]`: referenca vidi promene, kopija ne | lekcija 30, sekcija 5 |
| 2 | filteri; lanac obrade u `vector<function<double(double)>>`, jedan korak sa `std::bind`, drugi lambdom | lekcija 31, sekcije 2, 3 i 5 |
| 3 | `Logger` hvata `this` i odjavljuje se u destruktoru (RAII) | lekcija 30, sekcija 7; lekcija 21 |
| 4 | odjava usred objave: obilazak kopije spiska | lekcija 27, sekcija 6 |

## Na šta da paziš

- ⚠️ **`[this]` i životni vek.** Lambda koja hvata `this` živi u
  dispečeru i posle objekta ako se ne odjavi. Kad se odjava iz destruktora
  `Logger`-a ukloni, sledeća objava zove `zapisi` na uništenom objektu --
  ASan: `stack-use-after-scope` (provereno). Destruktor koji se odjavljuje
  je RAII za pretplatu.
- ⚠️ **Kopija `Logger`-a** bi imala isti `id_`, pa bi se ista pretplata
  odjavljivala dvaput, a lambda bi i dalje hvatala `this` originala --
  zato je kopiranje zabranjeno.
- ⚠️ **Menjanje spiska usred obilaska.** Ako `objavi` obilazi sam
  `pretplate_` (ne kopiju), a rukovalac se odjavi, `erase` pomeri
  elemente i petlja preskoči sledećeg rukovaoca: na oba kompajlera "start
  od pumpa" jednostavno izostane, **bez ikakve prijave** -- ASan ništa ne
  vidi. `-D_GLIBCXX_DEBUG` ga uhvati ("attempt to increment a singular
  iterator"). Obe stvari provereno.
- ⚠️ **Redosled ispisa.** Rukovaoci i sami pišu na `cout`; ako se
  `d.objavi(...)` pozove unutar `std::cout << "objava: " << d.objavi(...)`,
  deo reda se ispiše pre rukovaoca, deo posle. Zato `main` prvo sačuva
  rezultat u promenljivu.
- ✅ `std::bind(linearno, _1, 1.8, 32.0)` i `[](double c) { return linearno(c, 1.8, 32.0); }`
  rade isto; lambda je čitljivija i nema zamke bind-a (lekcija 31, sekcija 5).

## Posle rešenja

1. `objavi` kopira ceo spisak pri svakoj objavi. Kako bi izgledala
   jeftinija varijanta (oznaka "odjavljen" + čišćenje posle objave)?
2. Šta bi trebalo promeniti da `Logger` može da se **premešta**?
3. U delu 6: kako bi `std::weak_ptr` rešio problem viseće `this` bez
   odjave u destruktoru?

## Zapažanja posle vežbe
