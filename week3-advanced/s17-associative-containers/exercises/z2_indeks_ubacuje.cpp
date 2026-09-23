// VRSTA: zašto
// DEMO-OUT: NAIVNO konfigurisanih kanala posle provera: 5
//
// Zadatak 2 -- zašto se u mapi ne proverava sa [] (sekcija 3)
// Rešenje: exercises/solutions/z2_indeks_ubacuje.cpp
//
// Konfiguracija ima dva kanala. Program proverava da li su konfigurisani
// kanali koje traži korisnik.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week3-advanced/s17-associative-containers/exercises/z2_indeks_ubacuje.cpp -DNAIVNO
//   Posle provere tri nepostojeća kanala, konfiguracija ima 5 kanala.
//   m[k] kad k ne postoji UBACI par (k, 0) i vrati referencu na novu
//   nulu -- "provera" je izmenila mapu. (Zato [] ne postoji za const mapu,
//   lekcija 07, errors/e08.)
// Korak 2: u #else grani napiši konfigurisan() koja samo čita: find (ili
//   count; C++20 contains). Neka prima const std::map&.

#include <iostream>
#include <map>
#include <string>

#ifdef NAIVNO
bool konfigurisan(std::map<std::string, int>& m, const std::string& k) { return m[k] != 0; }
#else
// TODO korak 2 (dok ne napišeš, ova verzija uvek vraća false)
bool konfigurisan(const std::map<std::string, int>&, const std::string&) { return false; }
#endif

int main() {
    std::map<std::string, int> kanali{{"temp", 1}, {"pritisak", 2}};
    std::cout << std::boolalpha;
    for (const char* k : {"temp", "vlaga", "struja", "napon"})
        std::cout << k << ": " << konfigurisan(kanali, k) << '\n';
    std::cout << "konfigurisanih kanala posle provera: " << kanali.size() << '\n';
}

/* OČEKIVANI IZLAZ
temp: true
vlaga: false
struja: false
napon: false
konfigurisanih kanala posle provera: 2
*/
