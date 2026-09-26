// KIND: why
// DEMO-OUT: NAIVNO kalibracija = -31072
//
// Zadatak 3 -- zašto static_cast nije provera (sekcija 8)
// Rešenje: exercises/solutions/ex3_provereni_narrow.cpp
//
// Vrednost kalibracije stiže spolja (konfiguracija) kao long, a registar
// uređaja je short.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/17-type-conversions/exercises/ex3_provereni_narrow.cpp -DNAIVNO
//   100000 postane -31072. static_cast samo kaže kompajleru "znam šta
//   radim" -- ne proverava opseg. (100000 - 2 * 65536 = -31072.)
// Korak 2: u #else grani napiši
//       template <typename To, typename From> To narrow(From v)
//   koji baca std::range_error ako se vrednost promeni pri konverziji:
//   vrati rezultat nazad u From i uporedi, i proveri da se znak nije
//   promenio (bez toga narrow<unsigned>(-1) prođe: unsigned(-1) vraćen u
//   int je opet -1).
// Korak 3: otkomentariši test. Probaj i std::numeric_limits<short>::max()
//   kao granični slučaj.

#include <iostream>
#include <limits>
#include <stdexcept>

long izKonfiguracije() { return 100000; }

#ifdef NAIVNO
int main() {
    short kal = static_cast<short>(izKonfiguracije());
    std::cout << "kalibracija = " << kal << '\n';
}
#else
// TODO korak 2

// template <typename To, typename From>
// void probaj(From v) {
//     try {
//         To r = narrow<To>(v);   // prvo konverzija: ako baci, ništa nije ispisano
//         std::cout << v << " -> " << r << '\n';
//     } catch (const std::range_error&) {
//         std::cout << v << " -> range_error\n";
//     }
// }

int main() {
    // Korak 3 -- otkomentariši (i probaj() iznad):
    // probaj<short>(1234L);
    // probaj<short>(izKonfiguracije());
    // probaj<short>(static_cast<long>(std::numeric_limits<short>::max()));
    // probaj<short>(static_cast<long>(std::numeric_limits<short>::max()) + 1);
    // probaj<unsigned>(-1);
    // probaj<int>(3.0);
    // probaj<int>(3.5);
}
#endif

/* EXPECTED OUTPUT
1234 -> 1234
100000 -> range_error
32767 -> 32767
32768 -> range_error
-1 -> range_error
3 -> 3
3.5 -> range_error
*/
