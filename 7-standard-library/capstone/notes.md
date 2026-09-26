# Završna vežba dela 7 — Indeks log fajlova

Program pregleda direktorijum sa log fajlovima uređaja (i poddirektorijume),
parsira svaki red, gradi indeks po kanalu i po fajlu, i ispisuje
statistiku: min, max, medijanu, prekoračenja praga, najviše vrednosti i
kanale koji postoje u svim fajlovima. Skoro bez ručnih petlji za
računanje -- to rade kontejneri i algoritmi. Spaja lekcije 34–38.

**Kako raditi:**

```
./build.sh 7-standard-library/capstone/task.cpp        # tvoj kod
./check_exercises.sh 7-standard-library/capstone          # provera: zadatak i rešenje
```

Koraci su u komentaru na vrhu `task.cpp`; `main()` je napisan i
zakomentarisan po koracima. `makeLogs()` je dato: pravi privremeni
direktorijum (`temp_directory_path()`), a `main` ga na kraju briše, i u
nerešenom zadatku. Rešenje je u `solution.cpp`. Očekuj 2–3 sedenja.

---

## Ulaz

```
hall.log              08:00 temp 21.5 / 08:30 temp abc / # komentar ...
boiler.log            ... / 08:25                (bad format: jedna reč)
archive/2024-01.log   ... / 07:30 humidity 55 %     (bad format: četiri reči)
napomena.txt          nije log, preskače se
```

## Šta vežba spaja

| Korak | Šta radiš | Lekcija |
|---|---|---|
| 1 | rekurzivni obilazak, filtriranje po ekstenziji, sortiranje | lekcija 38, sekcije 5 i 6 |
| 2 | sečenje reda preko `string_view` bez kopija; `from_chars` u `optional<double>` | lekcija 38, sekcije 1 i 2; lekcija 37, sekcija 1 |
| 2 | red je `variant<Record, Error>`, obrada preko `visit` | lekcija 37, sekcije 4 i 5 |
| 3 | indeks: `map<string, vector<double>>`, `map<string, set<string>>`, `unordered_map` za brojanje | lekcija 35, sekcije 1, 3 i 4 |
| 4 | `minmax_element`, `nth_element`, `count_if`, `partial_sort`, `set_intersection` | lekcija 36, sekcije 2, 3 i 4 |

## Na šta da paziš

- ⚠️ **`string_view` samo dok red živi.** `nextWord` vraća poglede u
  `line` iz `std::getline`; u `Record` idu kao `std::string(...)`, jer
  `line` se u sledećem krugu petlje prepiše (lekcija 38, sekcija 3).
- ⚠️ **Redosled.** `recursive_directory_iterator` i `unordered_map`
  nemaju određen redosled -- oba se pre ispisa sortiraju. `map` i `set`
  su već sortirani.
- ⚠️ **`nth_element` menja niz**, zato `median` prima vektor **po
  vrednosti**; za paran broj elemenata vraća gornji od dva srednja
  (humidity 40 i 42 → 42).
- ✅ Presek skupova: `std::set_intersection` traži sortirane opsege --
  `std::set` to već jeste; izlaz ide preko `std::inserter`, jer ciljni
  skup nema `push_back`.
- ✅ `threshold("humidity")` vraća `std::nullopt` i ta kolona se ne ispisuje --
  bez "magične" vrednosti -1.

## Posle rešenja

1. Šta bi se promenilo da logovi imaju milione redova? Gde bi
   `reserve`, `unordered_map` umesto `map` ili paralelni algoritmi
   (lekcija 41) pomogli?
2. Kako bi `Error` nosila i ime fajla i broj reda, da izveštaj pokaže
   gde je greška?
3. Zašto `std::visit` sa `Overloaded` ovde ne može da zaboravi
   slučaj, a `if (std::holds_alternative<Record>(r))` može?

## Zapažanja posle vežbe
