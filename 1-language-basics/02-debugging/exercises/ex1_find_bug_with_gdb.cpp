// KIND: usage
//
// Zadatak 1 -- nađi logički bag gdb-om (sekcije 1, 2, 3)
// Rešenje: exercises/solutions/ex1_find_bug_with_gdb.cpp
//
// averageOfLast(v, k) treba da vrati prosek poslednjih k elemenata.
// Program se kompajlira bez upozorenja i radi bez ASan/UBSan prijava --
// ali je rezultat pogrešan. Sanitizeri ovde ne pomažu: nema UB-a, samo
// pogrešne logike. Zato debugger.
//
// Korak 1: build za gdb i pokretanje:
//     g++ -std=c++17 -g -O0 1-language-basics/02-debugging/exercises/ex1_find_bug_with_gdb.cpp -o ex1
//     gdb ./ex1
//   (gdb) break averageOfLast
//   (gdb) run
//   (gdb) print v
//   (gdb) print v.size() - k + 1        <- odakle petlja kreće?
//   (gdb) watch s
//   (gdb) continue                      <- koje vrednosti ulaze u zbir?
//   Zapiši šta je pogrešno, PRE nego što menjaš kod.
// Korak 2: popravi averageOfLast. Proveri u gdb-u da petlja sada
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

double averageOfLast(const std::vector<int>& v, std::size_t k) {
    int s = 0;
    for (std::size_t i = v.size() - k + 1; i < v.size(); ++i) {
        s += v[i];
    }
    return static_cast<double>(s) / static_cast<double>(k);
}

int main() {
    std::vector<int> v{10, 20, 30, 40, 50};
    std::cout << "average of last 3: " << averageOfLast(v, 3) << '\n';
    std::cout << "average of last 5: " << averageOfLast(v, 5) << '\n';
    // Korak 3 -- otkomentariši:
    // std::cout << "average of last 7: " << averageOfLast(v, 7) << '\n';
    // try {
    //     averageOfLast(v, 0);
    // } catch (const std::invalid_argument& e) {
    //     std::cout << "k = 0: " << e.what() << '\n';
    // }
}

/* EXPECTED OUTPUT
average of last 3: 40
average of last 5: 30
average of last 7: 30
k = 0: k must be > 0
*/
