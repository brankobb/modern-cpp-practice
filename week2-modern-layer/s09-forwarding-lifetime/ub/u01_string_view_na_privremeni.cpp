// EXPECT-UB: heap-use-after-free
// POGREŠNO: string_view napravljen od std::string koji funkcija vraća po vrednosti.
// Zašto: makeGreeting() vraća privremeni std::string, a view pokazuje na
//   njegove znakove. Na kraju te linije privremeni nestane, i view pokazuje
//   na oslobođen bafer. Pravilo o produženju života (s01) važi samo za
//   reference, a string_view je objekat, ne referenca.
//   clang -Wall upozori (-Wdangling-gsl), g++ ne.
//   (Napomena: printf("%.*s", view) ASan u testu nije prijavio -- čitanje
//   preko libc ne ide kroz instrumentisan kod. view[0] jeste.)
// Ispravno: std::string greeting = makeGreeting("Ana"); pa view na njega;
//   ili odmah std::string umesto string_view kad izvor ne živi dovoljno dugo.
#include <cstdio>
#include <string>
#include <string_view>

std::string makeGreeting(const char* name) { return std::string("Dobrodošao nazad, ") + name + "!"; }

int main() {
    std::string_view view = makeGreeting("Ana");
    char first = view[0];
    std::printf("%c %zu\n", first, view.size());
}
