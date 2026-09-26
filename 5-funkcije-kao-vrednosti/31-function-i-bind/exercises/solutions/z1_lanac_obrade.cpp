// Rešenje zadatka z1_lanac_obrade.

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

// Korak 1: std::function briše razliku između vrsta callback-a -- u
// vektoru su metoda, objekat, funkcija i lambda, a za obradi() su isti tip.
using Korak = std::pair<std::string, std::function<double(double)>>;

void obradi(double x, const std::vector<Korak>& lanac) {
    std::cout << x;
    for (const auto& [ime, f] : lanac) {
        if (!f) {                        // prazan: poziv bi bacio bad_function_call
            std::cout << " -> " << ime << " preskočen";
            continue;
        }
        x = f(x);
        std::cout << " -> " << ime << ' ' << x;
    }
    std::cout << '\n';
}

int main() {
    Kalibracija kal{1.5};
    using namespace std::placeholders;
    // Korak 2: &kal -- bind čuva POKAZIVAČ, pa kal mora da živi dok se
    // lanac koristi (sa kal umesto &kal bio bi kopiran).
    std::vector<Korak> lanac{
        {"kalibracija", std::bind(&Kalibracija::primeni, &kal, _1)},
        {"skala", Skaliraj{2}},
        {"kvadrat", kvadrat},
        {"rezerva", {}},
        {"ogranici", std::bind(ogranici, _1, 0.0, 100.0)}};
    obradi(3, lanac);
    obradi(6, lanac);

    // Korak 3: iste stvari lambdama -- poziv se čita kao običan poziv
    // funkcije, bez _1 i bez pravila o kopiranju argumenata.
    std::vector<Korak> lambdaLanac{
        {"kalibracija", [&kal](double x) { return kal.primeni(x); }},
        {"skala", Skaliraj{2}},
        {"kvadrat", kvadrat},
        {"rezerva", {}},
        {"ogranici", [](double x) { return ogranici(x, 0.0, 100.0); }}};
    obradi(6, lambdaLanac);
}
