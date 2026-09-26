// KIND: why
// DEMO-OUT: NAIVE fill\(3, 7\): \[3 7\]
//
// Zadatak 3 -- () i {} kod vector-a nisu isto (sekcije 10, 11)
// Rešenje: exercises/solutions/ex3_vector_parentheses.cpp
//
// Korak 1: PRE pokretanja, za svaki red u testu koraka 1 upiši u komentar
//   koliko elemenata očekuješ. Onda otkomentariši i proveri.
// Korak 2: fill(n, value) treba da vrati n kopija vrednosti. Pokreni
//   naivnu verziju:
//     ./build.sh 1-language-basics/03-initialization/exercises/ex3_vector_parentheses.cpp -DNAIVE
//   i objasni zašto je dobila 2 elementa (sekcija 11: initializer_list
//   konstruktor ima prednost čim su argumenti konvertibilni u element).
//   Popravi fill() u #else grani.
// Korak 3: zašto vector<string>{10} NE pravi string "10"? (sekcija 10)

#include <iostream>
#include <string>
#include <vector>

std::vector<int> fill(int n, int value) {
#ifdef NAIVE
    return std::vector<int>{n, value};
#else
    // TODO korak 2
    (void)n;
    (void)value;
    return {};
#endif
}

void print(const char* label, const std::vector<int>& v) {
    std::cout << label << ": [";
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
    // print("fill(3, 7)", fill(3, 7));
#ifdef NAIVE
    print("fill(3, 7)", fill(3, 7));
#endif
}

/* EXPECTED OUTPUT
v1(10): 10
v2{10}: 1
v3(10, 20): 10
v4{10, 20}: 2
s1{10}: 10
s2{"10"}: 1
fill(3, 7): [7 7 7]
*/
