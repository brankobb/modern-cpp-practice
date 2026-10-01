# Provera posle Koraka 1 i 2

Ako na ova pitanja odgovaraš bez razmišljanja, spreman si za Korak 3.
Piši odgovore ovde (2–5 rečenica, po mogućstvu sa malim primerom koda),
pa ih tek onda uporedi sa `answers-reference.md`. Ako ne znaš neki,
vrati se na tu temu pre Koraka 3.

## 1. Kada se poziva copy ctor, a kada `operator=`?

## 2. Zašto je virtualan destruktor obavezan?

## 3. Koja je razlika između `T x;`, `T x{};` i `T x();`?

## 4. Šta je copy elision? Kada je `T b = ...;` pre C++17 moglo da napravi dva poziva umesto jednog?

> Napomena uz originalno pitanje ("zašto `Buffer b = a;` može da pozove
> copy ctor dva puta pre C++17"): za `Buffer b = a;`, gde je `a` već
> `Buffer`, copy ctor se poziva **tačno jednom** u svim standardima. Dva
> poziva (konstruktor privremenog + copy/move) postoje kod
> `Buffer b = Buffer(10);`, `Buffer b = make();` i kod `Buffer b = 10;`
> sa ne-`explicit` konstruktorom. Odgovori na tu, preciznu verziju.

## 5. Šta je self-assignment i zašto je opasan?

## 6. Zašto `explicit` na konstruktoru sa jednim argumentom?

## 7. Koja je razlika između `const T&` i `T&&` parametra?

## 8. Redosled inicijalizacije članova — po čemu se određuje?

## 9. Zašto je Rule of 0 bolji od Rule of 5?

## 10. Šta je slicing i zašto je opasan?
