// Rešenje zadatka ex1_processing_chain.

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

// Korak 1: std::function briše razliku između vrsta callback-a -- u
// vektoru su metoda, objekat, funkcija i lambda, a za process() su isti tip.
using Step = std::pair<std::string, std::function<double(double)>>;

void process(double x, const std::vector<Step>& chain) {
    std::cout << x;
    for (const auto& [name, f] : chain) {
        if (!f) {                        // prazan: poziv bi bacio bad_function_call
            std::cout << " -> " << name << " skipped";
            continue;
        }
        x = f(x);
        std::cout << " -> " << name << ' ' << x;
    }
    std::cout << '\n';
}

int main() {
    Calibration cal{1.5};
    using namespace std::placeholders;
    // Korak 2: &cal -- bind čuva POKAZIVAČ, pa cal mora da živi dok se
    // lanac koristi (sa cal umesto &cal bio bi kopiran).
    std::vector<Step> chain{
        {"calibration", std::bind(&Calibration::apply, &cal, _1)},
        {"scale", Scale{2}},
        {"square", square},
        {"spare", {}},
        {"clamp", std::bind(clampTo, _1, 0.0, 100.0)}};
    process(3, chain);
    process(6, chain);

    // Korak 3: iste stvari lambdama -- poziv se čita kao običan poziv
    // funkcije, bez _1 i bez pravila o kopiranju argumenata.
    std::vector<Step> lambdaChain{
        {"calibration", [&cal](double x) { return cal.apply(x); }},
        {"scale", Scale{2}},
        {"square", square},
        {"spare", {}},
        {"clamp", [](double x) { return clampTo(x, 0.0, 100.0); }}};
    process(6, lambdaChain);
}
