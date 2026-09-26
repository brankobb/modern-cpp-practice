// VRSTA: upotreba
//
// Zadatak 1 -- pokazivači, reference i opsezi (sekcije 3, 6, 10, 11)
//   ./build.sh 1-osnove-jezika/04-pokazivaci-i-reference/exercises/z1_parametri.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_parametri.cpp
//
// Korak 1: void zameni(int& a, int& b) -- zameni vrednosti.
//   Zatim bool zameniPtr(int* a, int* b) -- isto preko pokazivača, ali
//   pokazivač SME da bude nullptr: tada ne radi ništa i vrati false.
//   (Referenca "ne može" da bude null, pa zameni() nema tu proveru.)
// Korak 2: int zbir(const int* first, const int* last) -- zbir elemenata
//   u poluotvorenom opsegu [first, last), samo pokazivačkom aritmetikom
//   (bez indeksa). zbir(p, p) je 0. Zašto const int*?
// Korak 3: const Senzor* najtopliji(const Senzor* niz, std::size_t n) --
//   vrati pokazivač na senzor sa najvećom temperaturom, ili nullptr ako je
//   n == 0. Zašto ovde pokazivač, a ne referenca? (EC++ Item 21: "možda
//   nema rezultata")

#include <cstddef>
#include <iostream>

struct Senzor {
    const char* ime;
    double temp;
};

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // int x = 1, y = 2;
    // zameni(x, y);
    // std::cout << "zameni: x=" << x << " y=" << y << '\n';
    // bool ok = zameniPtr(&x, &y);
    // std::cout << "zameniPtr: " << ok << " x=" << x << " y=" << y << '\n';
    // std::cout << "zameniPtr(nullptr): " << zameniPtr(&x, nullptr) << '\n';

    // Korak 2 -- otkomentariši:
    // int niz[] = {1, 2, 3, 4, 5};
    // std::cout << "zbir svih: " << zbir(niz, niz + 5) << '\n';
    // std::cout << "zbir [1, 3): " << zbir(niz + 1, niz + 3) << '\n';
    // std::cout << "zbir praznog: " << zbir(niz, niz) << '\n';

    // Korak 3 -- otkomentariši:
    // Senzor s[] = {{"motor", 71.5}, {"baterija", 38.0}, {"cpu", 84.25}};
    // if (const Senzor* p = najtopliji(s, 3))
    //     std::cout << "najtopliji: " << p->ime << ' ' << p->temp << '\n';
    // if (najtopliji(s, 0) == nullptr)
    //     std::cout << "prazan niz: nema rezultata\n";
}

/* OČEKIVANI IZLAZ
zameni: x=2 y=1
zameniPtr: 1 x=1 y=2
zameniPtr(nullptr): 0
zbir svih: 15
zbir [1, 3): 5
zbir praznog: 0
najtopliji: cpu 84.25
prazan niz: nema rezultata
*/
