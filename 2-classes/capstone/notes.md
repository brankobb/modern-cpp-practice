# Završna vežba dela 2 — Temperature i senzori

Mala biblioteka za rad sa temperaturama i senzorima: tipovi koji ne
dozvoljavaju besmislene operacije, hijerarhija senzora sa virtuelnim
funkcijama i nadzor koji ih sve čita preko istog interfejsa. Samo ono
što je bilo u lekcijama 14–17 (i delu 1); izuzeci dolaze tek u delu 3,
pa invarijantu čuva `assert`.

**Kako raditi:**

```
./build.sh 2-classes/capstone/task.cpp          # tvoj kod
./check_exercises.sh 2-classes/capstone            # provera: zadatak i rešenje
```

Klase pišeš sam (koraci su u komentaru na vrhu `task.cpp`), a `main()`
je već napisan, zakomentarisan po koracima. Rešenje je u `solution.cpp`.
Očekuj 2–3 sedenja.

---

## Šta vežba spaja

| Korak | Šta radiš | Lekcija |
|---|---|---|
| 1 | `Delta` i `Temperature`: dva tipa, pa `Temperature + Temperature` ne može ni da se napiše | lekcija 15, sekcije 2, 3 i 5 |
| 1 | invarijanta preko privatnog konstruktora i fabričkih funkcija; `explicit` konstruktor | lekcija 14, sekcije 1 i 2; lekcija 17, sekcija 5 |
| 1 | `operator<<` koji ne menja podešavanja tuđeg stream-a | lekcija 15, sekcija 6 |
| 2 | apstraktna bazna klasa, `virtual`/`override`/`final`, virtual destruktor, zabranjeno kopiranje | lekcija 16, sekcije 4, 5, 8 i 12 |
| 2 | brojač živih senzora kao `static inline` član | lekcija 14, sekcija 6 |
| 3 | kalibracija samo senzora koji to umeju: `dynamic_cast` | lekcija 17, sekcija 4 |
| 4 | prag alarma kao funkcijski objekat | lekcija 15, sekcija 9 |

## Na šta da paziš

- ✅ **Tip čuva značenje.** Temperatura i razlika temperatura su različite
  stvari (kao tačka i vektor): `20 °C + 5 K` ima smisla, `20 °C + 5 °C`
  nema. Sa dva tipa to kompajler proverava umesto tebe. Probaj u `main`:
  `Temperature::fromCelsius(1) + Temperature::fromCelsius(2)` se ne
  kompajlira (g++: `no match for 'operator+'`), `Delta r = 2.0;` isto
  ne (`explicit`), a ni `Temperature t{20.0};` (privatan konstruktor) --
  provereno na g++ i clang-u.
- ⚠️ **`operator<<` i stanje stream-a.** `out << std::fixed << std::setprecision(1)`
  unutar operatora ostaje na stream-u i posle njega, pa sledeći broj koji
  pozivalac ispiše ima jednu decimalu, a da on to nije tražio. U prvoj
  verziji rešenja se baš to desilo: kelvin je ispisan kao 296.6 umesto
  296.65. Zato formatiranje ide u lokalni `std::ostringstream`.
- ⚠️ `std::vector<Sensor*>` ovde **ne poseduje** senzore; oni su na steku
  u `main`-u i žive duže od vektora. Vlasništvo preko pokazivača dolazi u
  delu 6 (`std::unique_ptr<Sensor>`).
- ⚠️ Poređenje `double` sa `==` je tačno samo za vrednosti koje su
  izračunate istim putem (ovde: ista vrednost iz iste fabrike). Za
  izmerene vrednosti poređenje ide sa tolerancijom.

## Posle rešenja

1. Šta bi se promenilo da `Temperature` čuva kelvine umesto °C? Koje bi
   vrednosti tada ispale "ružne" pri ispisu, i zašto?
2. Zašto je `CalibratedSensor` `final`? Šta bi značilo nasleđivanje od
   njega?
3. Kako bi `fromCelsius` javila grešku za -300 °C kad bi mogla da koristi
   izuzetke (lekcija 18)?

## Zapažanja posle vežbe
