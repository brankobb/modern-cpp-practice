# Završna vežba dela 4 — Generički kružni bafer

`KruzniBafer<T, N>` čuva poslednjih `N` vrednosti bilo kog tipa: kad je
pun, nova vrednost prepisuje najstariju (tipično za istoriju merenja na
uređaju sa fiksnom memorijom). Oko njega: statistika koja radi za brojeve
i za sopstveni tip, variadic pomoćne funkcije, specijalizacije za
ispis i CTAD. Sve iz lekcija 26–29, uz move semantiku i izuzetke iz dela 3.

**Kako raditi:**

```
./build.sh 4-templates/capstone/task.cpp          # tvoj kod
./check_exercises.sh 4-templates/capstone            # provera: zadatak i rešenje
```

Koraci su u komentaru na vrhu `task.cpp`; `main()` je napisan i
zakomentarisan po koracima. Rešenje je u `solution.cpp`. Očekuj 2–3 sedenja.

---

## Šta vežba spaja

| Korak | Šta radiš | Lekcija |
|---|---|---|
| 1 | klasni šablon sa ne-tipskim parametrom, `static_assert` | lekcija 26, sekcija 6; lekcija 28, sekcije 3 i 8 |
| 1 | `dodaj(const T&)` / `dodaj(T&&)` i `emplace(Args&&...)` sa `std::forward`; brojanje kopija i premeštanja | lekcija 22; lekcija 27, sekcija 3; lekcija 28, sekcija 1 |
| 1 | `at()` koji baca `std::out_of_range` | lekcija 18 |
| 2 | sopstveni trait `JeMerenje<T>` i `jeMerenje_v`; jedna funkcija za brojeve i merenja preko `if constexpr` | lekcija 28, sekcija 7; lekcija 29, sekcije 4 i 5 |
| 3 | `dodajSve` i `sviUOpsegu`: variadic šabloni i fold | lekcija 29, sekcije 2 i 3 |
| 4 | agregat sa CTAD-om (deduction guide), delimična i potpuna specijalizacija, alias šablon | lekcija 29, sekcija 1; lekcija 28, sekcije 4, 5 i 6 |

## Na šta da paziš

- ✅ **Koliko kopija?** `Pracen` broji: `dodaj(x)` je 1 kopija,
  `dodaj(Pracen("b"))` 1 premeštanje, `emplace("c")` još 1 premeštanje
  (pravi privremeni `Pracen`, pa ga premesti u niz). Ako ti izađe više
  kopija, negde nedostaje `std::move` ili `std::forward`.
- ⚠️ **`emplace` ovde nije pravi emplace.** `std::array<T, N>` već drži
  `N` napravljenih objekata (zato `T` mora da ima podrazumevani
  konstruktor), pa `emplace` može samo da dodeli. Pravi emplace, koji
  pravi objekat direktno u memoriji bez privremenog, traži sirovu
  memoriju i placement new -- to je lekcija 33.
- ⚠️ **Prazan paket.** `sviUOpsegu(0, 50)` je `true` (fold po `&&` za
  prazan paket), ali tada se `min` i `max` ne koriste, i g++ sa
  `-Wextra` upozori "set but not used" (desilo se pri pisanju rešenja).
  `[[maybe_unused]]` na parametrima to rešava.
- ⚠️ **CTAD za agregat.** Bez deduction guide-a `Opseg o{0.0, 50.0}` se u
  C++17 ne kompajlira (g++: "class template argument deduction failed"),
  a u C++20 radi -- provereno.
- ✅ `static_assert` u `vrednostOd` daje jasnu poruku ("tip nije broj ni
  merenje") umesto stranice grešaka iz dubine šablona; probaj
  `vrednostOd(std::string("x"))`.

## Posle rešenja

1. Kako bi izgledao `KruzniBafer` sa pravim `emplace`-om, bez zahteva da
   `T` ima podrazumevani konstruktor? (Pogledaj `Buffer` bez heap-a u
   lekciji 33.)
2. Zašto `Formater<T*>` mora da bude delimična, a `Formater<std::string>`
   potpuna specijalizacija?
3. Šta bi trebalo dodati da bi `for (auto& x : bafer)` radilo (iteratori)?

## Zapažanja posle vežbe
