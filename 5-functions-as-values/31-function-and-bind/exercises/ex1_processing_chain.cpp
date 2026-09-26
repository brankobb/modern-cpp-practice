// KIND: usage
//
// Zadatak 1 -- std::function kao "bilo šta što se poziva", std::bind za
// delimičnu primenu i metode (sekcije 1, 3, 4)
//   ./build.sh 5-functions-as-values/31-function-and-bind/exercises/ex1_processing_chain.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_processing_chain.cpp
//
// Očitavanje senzora prolazi kroz lanac koraka obrade. Svaki korak je
// std::function<double(double)>, sa imenom.
// Korak 1: using Step = std::pair<std::string, std::function<double(double)>>;
//   i void process(double x, const std::vector<Step>& chain) -- ispiše
//   "x -> ime vrednost -> ime vrednost ...". PRAZAN korak (bez funkcije)
//   preskoči i ispiše "-> ime preskočen" (poziv bi bacio bad_function_call).
// Korak 2: napravi lanac od četiri različite vrste:
//   a) kalibracija: METODA Calibration::apply na objektu cal, preko
//      std::bind(&Calibration::apply, &cal, _1);
//   b) skala: funkcijski objekat Scale{2};
//   c) square: obična funkcija double square(double);
//   d) clampTo: std::bind(clampTo, _1, 0.0, 100.0) -- fiksirane granice;
//   i jedan prazan korak {"rezerva", {}}.
// Korak 3: napiši a) i d) i kao lambde (lambdaChain) i proveri da je
//   izlaz isti. Koja verzija se lakše čita?

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Calibration {
    double offset;
    double apply(double x) const { return x + offset; }
};

struct Scale {
    double k;
    double operator()(double x) const { return x * k; }
};

double square(double x) { return x * x; }
double clampTo(double x, double lo, double hi) { return x < lo ? lo : (x > hi ? hi : x); }

// TODO korak 1

int main() {
    Calibration cal{1.5};
    (void)cal;
    // Korak 2 -- otkomentariši:
    // using namespace std::placeholders;
    // std::vector<Step> chain{
    //     {"calibration", std::bind(&Calibration::apply, &cal, _1)},
    //     {"scale", Scale{2}},
    //     {"square", square},
    //     {"spare", {}},
    //     {"clamp", std::bind(clampTo, _1, 0.0, 100.0)}};
    // process(3, chain);
    // process(6, chain);

    // Korak 3 -- otkomentariši (i napiši lambdaChain):
    // process(6, lambdaChain);
}

/* EXPECTED OUTPUT
3 -> calibration 4.5 -> scale 9 -> square 81 -> spare skipped -> clamp 81
6 -> calibration 7.5 -> scale 15 -> square 225 -> spare skipped -> clamp 100
6 -> calibration 7.5 -> scale 15 -> square 225 -> spare skipped -> clamp 100
*/
