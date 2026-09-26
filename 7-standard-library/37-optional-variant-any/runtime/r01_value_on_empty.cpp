// EXPECT-RUN: terminate called after throwing an instance of 'std::bad_optional_access'
// POGREŠNO: value() proverava i, za prazan optional, baca
// std::bad_optional_access -- bez try: terminate. (Za razliku od *o,
// ub/u01, ovo bar nije UB.)
// Ispravno: if (o) ...; o.value_or(x); ili try/catch ako je prazan
// optional zaista izuzetna situacija.
#include <optional>
int main() {
    std::optional<int> o;
    return o.value();
}
