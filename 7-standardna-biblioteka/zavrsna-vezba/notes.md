# Završna vežba dela 7 — Indeks log fajlova

Program pregleda direktorijum sa log fajlovima uređaja (i poddirektorijume),
parsira svaki red, gradi indeks po kanalu i po fajlu, i ispisuje
statistiku: min, max, medijanu, prekoračenja praga, najviše vrednosti i
kanale koji postoje u svim fajlovima. Skoro bez ručnih petlji za
računanje -- to rade kontejneri i algoritmi. Spaja lekcije 34–38.

**Kako raditi:**

```
./build.sh 7-standardna-biblioteka/zavrsna-vezba/zadatak.cpp        # tvoj kod
./check_exercises.sh 7-standardna-biblioteka/zavrsna-vezba          # provera: zadatak i rešenje
```

Koraci su u komentaru na vrhu `zadatak.cpp`; `main()` je napisan i
zakomentarisan po koracima. `napraviLogove()` je dato: pravi privremeni
direktorijum (`temp_directory_path()`), a `main` ga na kraju briše, i u
nerešenom zadatku. Rešenje je u `resenje.cpp`. Očekuj 2–3 sedenja.

---

## Ulaz

```
hala.log              08:00 temp 21.5 / 08:30 temp abc / # komentar ...
kotao.log             ... / 08:25                (loš format: jedna reč)
arhiva/2024-01.log    ... / 07:30 vlaga 55 %     (loš format: četiri reči)
napomena.txt          nije log, preskače se
```

## Šta vežba spaja

| Korak | Šta radiš | Lekcija |
|---|---|---|
| 1 | rekurzivni obilazak, filtriranje po ekstenziji, sortiranje | lekcija 38, sekcije 5 i 6 |
| 2 | sečenje reda preko `string_view` bez kopija; `from_chars` u `optional<double>` | lekcija 38, sekcije 1 i 2; lekcija 37, sekcija 1 |
| 2 | red je `variant<Zapis, Greska>`, obrada preko `visit` | lekcija 37, sekcije 4 i 5 |
| 3 | indeks: `map<string, vector<double>>`, `map<string, set<string>>`, `unordered_map` za brojanje | lekcija 35, sekcije 1, 3 i 4 |
| 4 | `minmax_element`, `nth_element`, `count_if`, `partial_sort`, `set_intersection` | lekcija 36, sekcije 2, 3 i 4 |

## Na šta da paziš

- ⚠️ **`string_view` samo dok red živi.** `sledecaRec` vraća poglede u
  `red` iz `std::getline`; u `Zapis` idu kao `std::string(...)`, jer
  `red` se u sledećem krugu petlje prepiše (lekcija 38, sekcija 3).
- ⚠️ **Redosled.** `recursive_directory_iterator` i `unordered_map`
  nemaju određen redosled -- oba se pre ispisa sortiraju. `map` i `set`
  su već sortirani.
- ⚠️ **`nth_element` menja niz**, zato `medijana` prima vektor **po
  vrednosti**; za paran broj elemenata vraća gornji od dva srednja
  (vlaga 40 i 42 → 42).
- ✅ Presek skupova: `std::set_intersection` traži sortirane opsege --
  `std::set` to već jeste; izlaz ide preko `std::inserter`, jer ciljni
  skup nema `push_back`.
- ✅ `prag("vlaga")` vraća `std::nullopt` i ta kolona se ne ispisuje --
  bez "magične" vrednosti -1.

## Posle rešenja

1. Šta bi se promenilo da logovi imaju milione redova? Gde bi
   `reserve`, `unordered_map` umesto `map` ili paralelni algoritmi
   (lekcija 41) pomogli?
2. Kako bi `Greska` nosila i ime fajla i broj reda, da izveštaj pokaže
   gde je greška?
3. Zašto `std::visit` sa `Preopterecen` ovde ne može da zaboravi
   slučaj, a `if (std::holds_alternative<Zapis>(r))` može?

## Zapažanja posle vežbe
