// KIND: usage
//
// Zadatak 1 -- pokazivači, reference i opsezi (sekcije 3, 6, 10, 11)
//   ./build.sh 1-language-basics/04-pointers-and-references/exercises/ex1_parameters.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_parameters.cpp
//
// Korak 1: void swapValues(int& a, int& b) -- zameni vrednosti.
//   Zatim bool swapPtr(int* a, int* b) -- isto preko pokazivača, ali
//   pokazivač SME da bude nullptr: tada ne radi ništa i vrati false.
//   (Referenca "ne može" da bude null, pa swapValues() nema tu proveru.)
// Korak 2: int sum(const int* first, const int* last) -- zbir elemenata
//   u poluotvorenom opsegu [first, last), samo pokazivačkom aritmetikom
//   (bez indeksa). sum(p, p) je 0. Zašto const int*?
// Korak 3: const Sensor* hottest(const Sensor* arr, std::size_t n) --
//   vrati pokazivač na senzor sa najvećom temperaturom, ili nullptr ako je
//   n == 0. Zašto ovde pokazivač, a ne referenca? (EC++ Item 21: "možda
//   nema rezultata")

#include <cstddef>
#include <iostream>

struct Sensor {
    const char* name;
    double temp;
};

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // int x = 1, y = 2;
    // swapValues(x, y);
    // std::cout << "swapValues: x=" << x << " y=" << y << '\n';
    // bool ok = swapPtr(&x, &y);
    // std::cout << "swapPtr: " << ok << " x=" << x << " y=" << y << '\n';
    // std::cout << "swapPtr(nullptr): " << swapPtr(&x, nullptr) << '\n';

    // Korak 2 -- otkomentariši:
    // int arr[] = {1, 2, 3, 4, 5};
    // std::cout << "sum of all: " << sum(arr, arr + 5) << '\n';
    // std::cout << "sum [1, 3): " << sum(arr + 1, arr + 3) << '\n';
    // std::cout << "sum of empty: " << sum(arr, arr) << '\n';

    // Korak 3 -- otkomentariši:
    // Sensor s[] = {{"engine", 71.5}, {"battery", 38.0}, {"cpu", 84.25}};
    // if (const Sensor* p = hottest(s, 3))
    //     std::cout << "hottest: " << p->name << ' ' << p->temp << '\n';
    // if (hottest(s, 0) == nullptr)
    //     std::cout << "empty array: no result\n";
}

/* EXPECTED OUTPUT
swapValues: x=2 y=1
swapPtr: 1 x=1 y=2
swapPtr(nullptr): 0
sum of all: 15
sum [1, 3): 5
sum of empty: 0
hottest: cpu 84.25
empty array: no result
*/
