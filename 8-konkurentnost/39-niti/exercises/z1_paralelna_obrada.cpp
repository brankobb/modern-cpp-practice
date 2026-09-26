// VRSTA: upotreba
// SANITIZER: thread
//
// Zadatak 1 -- podela posla na niti, bezbedan deljeni dnevnik, join u
// destruktoru (sekcije 2, 4, 6; runtime/r01)
//   ./build.sh 8-konkurentnost/39-niti/exercises/z1_paralelna_obrada.cpp
//   ./build.sh 8-konkurentnost/39-niti/exercises/z1_paralelna_obrada.cpp --tsan
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla. Pokreni
// i sa --tsan: ThreadSanitizer ne sme ništa da prijavi.
// Rešenje: exercises/solutions/z1_paralelna_obrada.cpp
//
// Korak 1: long paralelniZbir(const std::vector<int>& v, std::size_t n)
//   -- n niti, svaka sabira svoj deo (std::accumulate) u SVOJ element
//   std::vector<long> delovi(n); posle join-a saberi delove. Poslednja
//   nit uzima i ostatak (1001 element na 4 niti nije ravno).
// Korak 2: class BezbedanDnevnik sa void zapisi(std::string) i
//   std::vector<std::string> sortirano() const. Podaci i std::mutex su
//   članovi; svaki pristup pod std::lock_guard-om. (Zašto mutex mora da
//   bude mutable?)
// Korak 3: class CuvarNiti -- drži std::thread&, u destruktoru
//   if (joinable()) join(). U obradiSaGreskom(int& rezultat) pokreni nit
//   koja upiše 7 u rezultat, napravi čuvara, pa baci
//   std::runtime_error("greška posle pokretanja niti").
//   Bez čuvara: runtime/r01 (terminate). (C++20 std::jthread radi ovo sam.)

#include <algorithm>
#include <iostream>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

// TODO korak 1, 2, 3

int main() {
    std::vector<int> v(1001);
    std::iota(v.begin(), v.end(), 0);   // 0..1000

    // Korak 1 -- otkomentariši:
    // std::cout << "zbir, 1 nit: " << paralelniZbir(v, 1) << ", 4 niti: " << paralelniZbir(v, 4)
    //           << ", 7 niti: " << paralelniZbir(v, 7) << '\n';

    // Korak 2 -- otkomentariši:
    // BezbedanDnevnik d;
    // std::vector<std::thread> niti;
    // for (int i = 0; i < 4; ++i)
    //     niti.emplace_back([&d, i] {
    //         for (int j = 0; j < 3; ++j) d.zapisi(std::to_string(i) + "." + std::to_string(j));
    //     });
    // for (auto& t : niti) t.join();
    // auto sve = d.sortirano();
    // std::cout << "dnevnik: " << sve.size() << " zapisa, prvi " << sve.front() << ", poslednji " << sve.back()
    //           << '\n';

    // Korak 3 -- otkomentariši:
    // int rezultat = 0;
    // try {
    //     obradiSaGreskom(rezultat);
    // } catch (const std::exception& e) {
    //     std::cout << "uhvaćeno: " << e.what() << "; nit je završila, rezultat " << rezultat << '\n';
    // }
}

/* OČEKIVANI IZLAZ
zbir, 1 nit: 500500, 4 niti: 500500, 7 niti: 500500
dnevnik: 12 zapisa, prvi 0.0, poslednji 3.2
uhvaćeno: greška posle pokretanja niti; nit je završila, rezultat 7
*/
