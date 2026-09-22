# 07 — const Qualifier & Compound Types (26)

- `const int* p` — pokazivač na CONST int; ne možeš menjati `*p`, možeš
  menjati `p` (na šta pokazuje)
- `int* const p` — CONST pokazivač na int; možeš menjati `*p`, NE možeš
  menjati `p`
- `const int* const p` — oboje const
- čitaj sa desna na levo oko `*` da ne pogrešiš
- `const T&` parametar — ne možeš menjati referisani objekat, i može se
  vezati i za lvalue i za rvalue (temporary) — zato je default izbor za
  "read-only" parametre
- `const` member funkcija (`void f() const`) — ne sme menjati non-mutable
  članove; `mutable` član je izuzetak (npr. keš, mutex za lock u const
  getteru)
- `const_cast` skida const — legitimno JEDINO ako je originalni objekat
  zaista non-const (npr. legacy C API koji ne prima const parametar iako
  ne menja podatke); ako skineš const sa STVARNO const objekta i pokušaš
  da ga izmeniš, to je UB

## Zapažanja posle vežbe
