# 12 — Operator overloading (OOP bridge)

- member vs free function: piši operator kao MEMBER kad je levi operand
  tvoje klase (`a + b` gde je `a` tipa `MyClass`); piši kao FREE function
  (često `friend`) kad levi operand NIJE tvoje klase (npr. `2 * vec` gde je
  `2` int) ili za `operator<<`/`operator>>` sa `std::ostream`/`std::istream`
  (levi operand je stream, ne tvoja klasa)
- `operator==` — piši kao free function koja poredi sve relevantne članove;
  ako pišeš `operator==`, obično ti treba i `operator!=` (ili u C++20,
  kompajler ga izvodi automatski iz `==`)
- `operator[]` — vrati referencu da dozvoli i čitanje i pisanje
  (`v[0] = 5;`); dodaj bounds check u debug verziji ili dokumentuj da NE
  proverava (kao `std::vector::operator[]`, za razliku od `.at()`)
- `operator=` (assignment) je već pokriven u week1 (copy-and-swap, self-assignment)
  — ovde je fokus na ARITMETIČKIM i poređenje operatorima
- pravilo: operator treba da radi ono što IZGLEDA da radi (principle of
  least surprise) — ne preopterećuj `+` da radi oduzimanje

## API korišćen u vežbi

- `std::ostream& operator<<(std::ostream&, const T&)` — standardni obrazac
  za "printable" tip; vraća REFERENCU na stream da omogući lančano
  `std::cout << a << b;` (svaki `<<` vraća stream, sledeći `<<` se
  primenjuje na tu povratnu vrednost)
- slobodna funkcija (free function) `operator*(double, const Vector2D&)` —
  MORA biti van klase jer LEVI operand (`double`) nije tip tvoje klase;
  da si napisao `operator*` kao member funkciju Vector2D-a, radio bi
  samo `vec * 2.0`, NE `2.0 * vec`

## Zapažanja posle vežbe
