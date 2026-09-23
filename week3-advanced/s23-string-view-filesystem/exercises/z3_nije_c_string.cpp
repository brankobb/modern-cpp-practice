// VRSTA: zašto
// DEMO-OUT: NAIVNO poslato: \[temp=21;vlaga=40\]
//
// Zadatak 3 -- zašto data() od string_view-a nije C string (sekcija 3; errors/e03)
// Rešenje: exercises/solutions/z3_nije_c_string.cpp
//
// Konfiguracija se seče na delove (string_view), a svaki deo se šalje
// C funkciji posaljiNaPort(const char*) -- kao drajver serijskog porta.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week3-advanced/s23-string-view-filesystem/exercises/z3_nije_c_string.cpp -DNAIVNO
//   Prvi deo je "temp=21", a poslato je "temp=21;vlaga=40". data() je
//   samo pokazivač na PRVI znak pogleda; C funkcija čita do '\0', a '\0'
//   postoji tek na kraju celog originala. Nije UB samo zato što je
//   original ceo string sa '\0'; da je pogled na bafer bez '\0',
//   čitalo bi se van njega.
// Korak 2: u #else grani napiši posalji(std::string_view) tako da C
//   funkcija dobije tačno deo: kopija std::string(deo), pa .c_str().

#include <cstdio>
#include <string>
#include <string_view>

void posaljiNaPort(const char* poruka) { std::printf("poslato: [%s]\n", poruka); }   // "C API"

#ifdef NAIVNO
void posalji(std::string_view deo) { posaljiNaPort(deo.data()); }
#else
// TODO korak 2 (dok ne napišeš, šalje se "?")
void posalji(std::string_view) { posaljiNaPort("?"); }
#endif

int main() {
    std::string konfig = "temp=21;vlaga=40";
    std::string_view sve = konfig;
    auto sep = sve.find(';');
    posalji(sve.substr(0, sep));
    posalji(sve.substr(sep + 1));
}

/* OČEKIVANI IZLAZ
poslato: [temp=21]
poslato: [vlaga=40]
*/
