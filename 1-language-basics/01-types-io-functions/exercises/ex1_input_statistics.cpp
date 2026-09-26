// KIND: usage
//
// Zadatak 1 -- čitanje brojeva sa proverom, struct kao rezultat, formatiran
// ispis (sekcije 7, 8, 9)
//   ./build.sh 1-language-basics/01-types-io-functions/exercises/ex1_input_statistics.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_input_statistics.cpp
//
// Ulaz dolazi iz std::istringstream (isto bi radilo sa std::cin).
// Korak 1: struct Statistics { int valid; int errors; long long sum;
//   int min; int max; };  (sve na 0)
// Korak 2: Statistics accumulate(std::istream& in) -- čita cele brojeve
//   dok ne dođe do kraja ulaza. Neispravan token (slova, broj van opsega
//   int-a) uveća errors, pa clear() i ignore() do sledećeg razmaka.
//   Ispravan broj uđe u sum, min i max. sum je long long -- zašto?
//   (Hint: sekcija 4.)
// Korak 3: void print(const Statistics& s) -- tabela sa std::setw:
//   naziv poravnat levo na 12 mesta (std::left), vrednost desno na 12
//   (std::right), a prosek sa 2 decimale (std::fixed, std::setprecision).
//   setw broji BAJTOVE, ne slova: naziv sa "š" (u UTF-8 dva bajta)
//   pomerio bi poravnanje za jedno mesto. Posle ispisa vrati
//   podrazumevani format (sekcija 8: lepljivi manipulatori).

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

// TODO korak 1, 2, 3

int main() {
    // Korak 1-3 -- otkomentariši:
    // std::istringstream input("12 7 x -3 99999999999 2000000000 2000000000 abc 5");
    // Statistics s = accumulate(input);
    // print(s);
    // std::cout << 42 << " (format restored after printing)\n";
}

/* EXPECTED OUTPUT
valid                  6
invalid                3
sum           4000000021
min                   -3
max           2000000000
average     666666670.17
42 (format restored after printing)
*/
