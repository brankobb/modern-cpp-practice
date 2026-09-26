// EXPECT-UB: heap-use-after-free
// POGREŠNO: pokazivač iz c_str() koristi se posle izmene stringa.
// Zašto: c_str() pokazuje na trenutni bafer stringa. += koje prekorači
//   kapacitet alocira NOVI bafer i oslobodi stari (kao std::vector), pa
//   view pokazuje na oslobođenu memoriju.
//   Napomena: za kratak string (SSO, lekcija 22) bafer je u samom objektu,
//   pa ASan ne vidi ništa, iako je po standardu isto UB -- zato je ovde
//   početni string dugačak.
// Ispravno: pozovi c_str() tek kad ti treba, posle svih izmena.
#include <cstdio>
#include <string>

int main() {
    std::string log(20, 's');
    const char* view = log.c_str();
    log += std::string(100, '.');
    std::printf("%c\n", view[0]);
}
