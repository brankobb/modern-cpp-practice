// VRSTA: zašto
// DEMO-OUT: NAIVNO napuni\(3, 7\): \[3 7\]
//
// Zadatak 3 -- () i {} kod vector-a nisu isto (sekcije 10, 11)
// Rešenje: exercises/solutions/z3_vector_zagrade.cpp
//
// Korak 1: PRE pokretanja, za svaki red u testu koraka 1 upiši u komentar
//   koliko elemenata očekuješ. Onda otkomentariši i proveri.
// Korak 2: napuni(n, vrednost) treba da vrati n kopija vrednosti. Pokreni
//   naivnu verziju:
//     ./build.sh week0-fundamentals/03-uniform-init/exercises/z3_vector_zagrade.cpp -DNAIVNO
//   i objasni zašto je dobila 2 elementa (sekcija 11: initializer_list
//   konstruktor ima prednost čim su argumenti konvertibilni u element).
//   Popravi napuni() u #else grani.
// Korak 3: zašto vector<string>{10} NE pravi string "10"? (sekcija 10)

#include <iostream>
#include <string>
#include <vector>

std::vector<int> napuni(int n, int vrednost) {
#ifdef NAIVNO
    return std::vector<int>{n, vrednost};
#else
    // TODO korak 2
    (void)n;
    (void)vrednost;
    return {};
#endif
}

void ispisi(const char* opis, const std::vector<int>& v) {
    std::cout << opis << ": [";
    for (std::size_t i = 0; i < v.size(); ++i) std::cout << (i ? " " : "") << v[i];
    std::cout << "]\n";
}

int main() {
    // Korak 1 -- predvidi, pa otkomentariši:
    // std::vector<int> v1(10);           // predviđanje: ?
    // std::vector<int> v2{10};           // predviđanje: ?
    // std::vector<int> v3(10, 20);       // predviđanje: ?
    // std::vector<int> v4{10, 20};       // predviđanje: ?
    // std::vector<std::string> s1{10};   // predviđanje: ?
    // std::vector<std::string> s2{"10"}; // predviđanje: ?
    // std::cout << "v1(10): " << v1.size() << '\n'
    //           << "v2{10}: " << v2.size() << '\n'
    //           << "v3(10, 20): " << v3.size() << '\n'
    //           << "v4{10, 20}: " << v4.size() << '\n'
    //           << "s1{10}: " << s1.size() << '\n'
    //           << "s2{\"10\"}: " << s2.size() << '\n';

    // Korak 2 -- otkomentariši:
    // ispisi("napuni(3, 7)", napuni(3, 7));
#ifdef NAIVNO
    ispisi("napuni(3, 7)", napuni(3, 7));
#endif
}

/* OČEKIVANI IZLAZ
v1(10): 10
v2{10}: 1
v3(10, 20): 10
v4{10, 20}: 2
s1{10}: 10
s2{"10"}: 1
napuni(3, 7): [7 7 7]
*/
