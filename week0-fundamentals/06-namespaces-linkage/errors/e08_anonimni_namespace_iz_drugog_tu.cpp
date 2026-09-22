// STD: c++17
// LINK: support/internal_helper.cpp
// EXPECT-GCC: undefined reference to `helper(int)'
// EXPECT-CLANG: undefined reference to `helper(int)'
// POGREŠNO: poziv funkcije koja je u drugom .cpp u anonimnom namespace-u.
// Zašto: anonimni namespace (i static na nivou namespace-a) daje INTERNAL
//   linkage. helper() postoji samo unutar internal_helper.cpp i linker ga ne
//   vidi iz drugih TU-ova. To je i poenta: pomoćna funkcija .cpp fajla ne
//   može da se sudari sa istim imenom u drugom fajlu.
// Ispravno: ako treba da se koristi spolja, neka ima external linkage (bez
//   static / anonimnog namespace-a) i deklaraciju u header-u.
int helper(int);

int main() {
    return helper(1);
}
