// EXPECT-UB: terminate called after throwing an instance of 'std::invalid_argument'
// POGREŠNO: funkcija označena noexcept ipak baci izuzetak.
// Zašto: noexcept je obećanje, a ne provera pri kompajliranju. Ako izuzetak
//   ipak stigne do granice funkcije, poziva se std::terminate (nema
//   unwinding-a do catch-a u main-u). Kompajleri upozore samo kad je throw
//   direktno u telu; kroz pozvanu funkciju ne vide ništa.
// Ispravno: noexcept samo na funkcije koje ZAISTA ne mogu da bace (swap,
//   move, destruktori); za parsiranje vrati grešku kroz povratnu vrednost
//   (std::optional) ili ukloni noexcept.
#include <cstdio>
#include <stdexcept>

int parse(const char* s) noexcept {
    if (!s[0]) throw std::invalid_argument("empty");
    return s[0] - '0';
}

int main() {
    try {
        std::printf("%d\n", parse(""));
    } catch (...) {
        std::puts("caught");
    }
}
