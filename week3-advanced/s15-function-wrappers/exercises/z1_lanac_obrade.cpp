// VRSTA: upotreba
//
// Zadatak 1 -- std::function kao "bilo šta što se poziva", std::bind za
// delimičnu primenu i metode (sekcije 1, 3, 4)
//   ./build.sh week3-advanced/s15-function-wrappers/exercises/z1_lanac_obrade.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_lanac_obrade.cpp
//
// Očitavanje senzora prolazi kroz lanac koraka obrade. Svaki korak je
// std::function<double(double)>, sa imenom.
// Korak 1: using Korak = std::pair<std::string, std::function<double(double)>>;
//   i void obradi(double x, const std::vector<Korak>& lanac) -- ispiše
//   "x -> ime vrednost -> ime vrednost ...". PRAZAN korak (bez funkcije)
//   preskoči i ispiše "-> ime preskočen" (poziv bi bacio bad_function_call).
// Korak 2: napravi lanac od četiri različite vrste:
//   a) kalibracija: METODA Kalibracija::primeni na objektu kal, preko
//      std::bind(&Kalibracija::primeni, &kal, _1);
//   b) skala: funkcijski objekat Skaliraj{2};
//   c) kvadrat: obična funkcija double kvadrat(double);
//   d) ogranici: std::bind(ogranici, _1, 0.0, 100.0) -- fiksirane granice;
//   i jedan prazan korak {"rezerva", {}}.
// Korak 3: napiši a) i d) i kao lambde (lambdaLanac) i proveri da je
//   izlaz isti. Koja verzija se lakše čita?

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Kalibracija {
    double pomak;
    double primeni(double x) const { return x + pomak; }
};

struct Skaliraj {
    double k;
    double operator()(double x) const { return x * k; }
};

double kvadrat(double x) { return x * x; }
double ogranici(double x, double lo, double hi) { return x < lo ? lo : (x > hi ? hi : x); }

// TODO korak 1

int main() {
    Kalibracija kal{1.5};
    (void)kal;
    // Korak 2 -- otkomentariši:
    // using namespace std::placeholders;
    // std::vector<Korak> lanac{
    //     {"kalibracija", std::bind(&Kalibracija::primeni, &kal, _1)},
    //     {"skala", Skaliraj{2}},
    //     {"kvadrat", kvadrat},
    //     {"rezerva", {}},
    //     {"ogranici", std::bind(ogranici, _1, 0.0, 100.0)}};
    // obradi(3, lanac);
    // obradi(6, lanac);

    // Korak 3 -- otkomentariši (i napiši lambdaLanac):
    // obradi(6, lambdaLanac);
}

/* OČEKIVANI IZLAZ
3 -> kalibracija 4.5 -> skala 9 -> kvadrat 81 -> rezerva preskočen -> ogranici 81
6 -> kalibracija 7.5 -> skala 15 -> kvadrat 225 -> rezerva preskočen -> ogranici 100
6 -> kalibracija 7.5 -> skala 15 -> kvadrat 225 -> rezerva preskočen -> ogranici 100
*/
