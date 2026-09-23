// VRSTA: zašto
// DEMO-OUT: NAIVNO temp -5: grejač isključen
//
// Zadatak 2 -- zašto se signed i unsigned ne mešaju u poređenju (sekcija 3)
// Rešenje: exercises/solutions/z2_signed_unsigned.cpp
//
// Termostat uključuje grejač kad je temperatura ispod praga. Prag je
// unsigned (npr. stiže iz konfiguracije kao "nenegativan broj").
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/01-types-io-functions/exercises/z2_signed_unsigned.cpp -DNAIVNO
//   Na 25 stepeni grejač je isključen (tačno), na 5 uključen (tačno), a na
//   -5 -- ISKLJUČEN. temp < prag poredi int sa unsigned: temp se
//   konvertuje u unsigned, i -5 postane 4294967291. g++ -Wall i clang
//   -Wextra upozore (-Wsign-compare); pročitaj upozorenje.
// Korak 2: u #else grani napiši trebaGrejanje ispravno. Prag ionako mora
//   biti mali pozitivan broj, pa: ako je temp < 0 -> true; inače poredi
//   static_cast<unsigned>(temp) < prag (sada su oba unsigned, i temp je
//   sigurno nenegativan). Ili: prag kao int u potpisu funkcije.
// Korak 3: (C++20) isto bez ručne provere: std::cmp_less(temp, prag) iz
//   <utility>. Probaj sa -std=c++20.

#include <iostream>

#ifdef NAIVNO
bool trebaGrejanje(int temp, unsigned prag) { return temp < prag; }
#else
// TODO korak 2 (dok ne napišeš, ova verzija uvek vraća false)
bool trebaGrejanje(int, unsigned) { return false; }
#endif

int main() {
    const unsigned prag = 18;
    for (int temp : {25, 5, -5})
        std::cout << "temp " << temp << ": grejač " << (trebaGrejanje(temp, prag) ? "uključen" : "isključen")
                  << '\n';
}

/* OČEKIVANI IZLAZ
temp 25: grejač isključen
temp 5: grejač uključen
temp -5: grejač uključen
*/
