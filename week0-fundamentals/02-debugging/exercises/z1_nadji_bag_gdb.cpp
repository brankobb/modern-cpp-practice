// VRSTA: upotreba
//
// Zadatak 1 -- nađi logički bag gdb-om (sekcije 1, 2, 3)
// Rešenje: exercises/solutions/z1_nadji_bag_gdb.cpp
//
// prosekPoslednjih(v, k) treba da vrati prosek poslednjih k elemenata.
// Program se kompajlira bez upozorenja i radi bez ASan/UBSan prijava --
// ali je rezultat pogrešan. Sanitizeri ovde ne pomažu: nema UB-a, samo
// pogrešne logike. Zato debugger.
//
// Korak 1: build za gdb i pokretanje:
//     g++ -std=c++17 -g -O0 week0-fundamentals/02-debugging/exercises/z1_nadji_bag_gdb.cpp -o z1
//     gdb ./z1
//   (gdb) break prosekPoslednjih
//   (gdb) run
//   (gdb) print v
//   (gdb) print v.size() - k + 1        <- odakle petlja kreće?
//   (gdb) watch s
//   (gdb) continue                      <- koje vrednosti ulaze u zbir?
//   Zapiši šta je pogrešno, PRE nego što menjaš kod.
// Korak 2: popravi prosekPoslednjih. Proveri u gdb-u da petlja sada
//   prolazi kroz tačno k elemenata.
// Korak 3: granični slučajevi. Šta se dešava za k = 0? (deljenje nulom u
//   double-u: rezultat je inf ili nan, bez greške) A za k veće od
//   v.size()? (v.size() - k je size_t -- prelazi preko nule, lekcija 01)
//   Neka k == 0 baca std::invalid_argument, a k > v.size() računa prosek
//   svih elemenata. Otkomentariši test.

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

double prosekPoslednjih(const std::vector<int>& v, std::size_t k) {
    int s = 0;
    for (std::size_t i = v.size() - k + 1; i < v.size(); ++i) {
        s += v[i];
    }
    return static_cast<double>(s) / static_cast<double>(k);
}

int main() {
    std::vector<int> v{10, 20, 30, 40, 50};
    std::cout << "prosek poslednja 3: " << prosekPoslednjih(v, 3) << '\n';
    std::cout << "prosek poslednjih 5: " << prosekPoslednjih(v, 5) << '\n';
    // Korak 3 -- otkomentariši:
    // std::cout << "prosek poslednjih 7: " << prosekPoslednjih(v, 7) << '\n';
    // try {
    //     prosekPoslednjih(v, 0);
    // } catch (const std::invalid_argument& e) {
    //     std::cout << "k = 0: " << e.what() << '\n';
    // }
}

/* OČEKIVANI IZLAZ
prosek poslednja 3: 40
prosek poslednjih 5: 30
prosek poslednjih 7: 30
k = 0: k mora biti > 0
*/
