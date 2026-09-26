// KIND: why
// DEMO-OUT: NAIVE temp -5: heater off
//
// Zadatak 2 -- zašto se signed i unsigned ne mešaju u poređenju (sekcija 3)
// Rešenje: exercises/solutions/ex2_signed_unsigned.cpp
//
// Termostat uključuje grejač kad je temperatura ispod praga. Prag je
// unsigned (npr. stiže iz konfiguracije kao "nenegativan broj").
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/01-types-io-functions/exercises/ex2_signed_unsigned.cpp -DNAIVE
//   Na 25 stepeni grejač je isključen (tačno), na 5 uključen (tačno), a na
//   -5 -- ISKLJUČEN. temp < threshold poredi int sa unsigned: temp se
//   konvertuje u unsigned, i -5 postane 4294967291. g++ -Wall i clang
//   -Wextra upozore (-Wsign-compare); pročitaj upozorenje.
// Korak 2: u #else grani napiši needsHeating ispravno. Prag ionako mora
//   biti mali pozitivan broj, pa: ako je temp < 0 -> true; inače poredi
//   static_cast<unsigned>(temp) < threshold (sada su oba unsigned, i temp
//   je sigurno nenegativan). Ili: prag kao int u potpisu funkcije.
// Korak 3: (C++20) isto bez ručne provere: std::cmp_less(temp, threshold)
//   iz <utility>. Probaj sa -std=c++20.

#include <iostream>

#ifdef NAIVE
bool needsHeating(int temp, unsigned threshold) { return temp < threshold; }
#else
// TODO korak 2 (dok ne napišeš, ova verzija uvek vraća false)
bool needsHeating(int, unsigned) { return false; }
#endif

int main() {
    const unsigned threshold = 18;
    for (int temp : {25, 5, -5})
        std::cout << "temp " << temp << ": heater " << (needsHeating(temp, threshold) ? "on" : "off") << '\n';
}

/* EXPECTED OUTPUT
temp 25: heater off
temp 5: heater on
temp -5: heater on
*/
