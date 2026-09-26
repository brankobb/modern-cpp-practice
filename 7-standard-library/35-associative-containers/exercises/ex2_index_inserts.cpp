// KIND: why
// DEMO-OUT: NAIVE configured channels after the checks: 5
//
// Zadatak 2 -- zašto se u mapi ne proverava sa [] (sekcija 3)
// Rešenje: exercises/solutions/ex2_index_inserts.cpp
//
// Konfiguracija ima dva kanala. Program proverava da li su konfigurisani
// kanali koje traži korisnik.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/35-associative-containers/exercises/ex2_index_inserts.cpp -DNAIVE
//   Posle provere tri nepostojeća kanala, konfiguracija ima 5 kanala.
//   m[k] kad k ne postoji UBACI par (k, 0) i vrati referencu na novu
//   nulu -- "provera" je izmenila mapu. (Zato [] ne postoji za const mapu,
//   lekcija 09, errors/e08.)
// Korak 2: u #else grani napiši isConfigured() koja samo čita: find (ili
//   count; C++20 contains). Neka prima const std::map&.

#include <iostream>
#include <map>
#include <string>

#ifdef NAIVE
bool isConfigured(std::map<std::string, int>& m, const std::string& k) { return m[k] != 0; }
#else
// TODO korak 2 (dok ne napišeš, ova verzija uvek vraća false)
bool isConfigured(const std::map<std::string, int>&, const std::string&) { return false; }
#endif

int main() {
    std::map<std::string, int> channels{{"temp", 1}, {"pressure", 2}};
    std::cout << std::boolalpha;
    for (const char* k : {"temp", "humidity", "current", "voltage"})
        std::cout << k << ": " << isConfigured(channels, k) << '\n';
    std::cout << "configured channels after the checks: " << channels.size() << '\n';
}

/* EXPECTED OUTPUT
temp: true
humidity: false
current: false
voltage: false
configured channels after the checks: 2
*/
