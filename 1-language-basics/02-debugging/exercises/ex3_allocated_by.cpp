// KIND: why
// DEMO-UB: NAIVNO heap-buffer-overflow
//
// Zadatak 3 -- zašto se u ASan izveštaju čita i "allocated by" (sekcija 5)
// Rešenje: exercises/solutions/ex3_allocated_by.cpp
//
// kopija(s) pravi kopiju C stringa na heap-u (kao strdup).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/02-debugging/exercises/ex3_allocated_by.cpp -DNAIVNO
//   i iz izveštaja odgovori (zapiši u komentar):
//   a) vrsta greške i da li je upis ili čitanje, koliko bajtova?
//   b) gde se desila (#0 i #1 prvog steka)? -- to je std::strcpy, a
//      strcpy je ispravan.
//   c) "0 bytes after N-byte region": koliki je blok, a koliko je trebalo?
//   d) "allocated by ... here": u kom redu je blok napravljen? Tamo je bag.
//   Mesto gde je program pao i mesto greške nisu isto.
// Korak 2: u #else grani napiši kopija() ispravno: blok za strlen(s) + 1
//   znakova ('\0' na kraju). Zatim (možeš i ovako) napiši i verziju koja
//   vraća std::string -- nema ručne veličine ni delete[].

#include <cstring>
#include <iostream>
#include <string>

#ifdef NAIVNO
char* kopija(const char* s) {
    char* p = new char[std::strlen(s)];   // koliko bajtova treba?
    std::strcpy(p, s);
    return p;
}

int main() {
    char* k = kopija("senzor-01");
    std::cout << k << '\n';
    delete[] k;
}
#else
// TODO korak 2: char* kopija(const char*) i std::string kopijaS(const char*)

int main() {
    // Korak 2 -- otkomentariši:
    // char* k = kopija("senzor-01");
    // std::cout << k << '\n';
    // delete[] k;
    // std::cout << kopijaS("senzor-02") << '\n';
}
#endif

/* EXPECTED OUTPUT
senzor-01
senzor-02
*/
