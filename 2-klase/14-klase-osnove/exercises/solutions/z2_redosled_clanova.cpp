// Rešenje zadatka z2_redosled_clanova.

#include <iostream>
#include <memory>
#include <vector>

struct Kalibracija {
    std::vector<double> koef;
    Kalibracija() : koef{2.0, 0.5} { std::cout << "Kalibracija\n"; }
};

struct Filter {
    double pojacanje;
    explicit Filter(const Kalibracija& k) : pojacanje(k.koef[0]) { std::cout << "Filter\n"; }
};

// Ako je filt_ deklarisan pre kal_ (nije dobro): pravi se prvi, bez obzira
// na init listu, i čita neinicijalizovanu memoriju.
// Treba ovako: član od kog drugi zavisi deklariši PRE njega, a init listu
// piši istim redom (tada -Wreorder ćuti i kod se čita kako se izvršava).
struct Senzor {
    Kalibracija kal_;
    Filter filt_;
    Senzor() : kal_(), filt_(kal_) {}
};

int main() {
    auto s = std::make_unique<Senzor>();
    std::cout << "pojačanje: " << s->filt_.pojacanje << '\n';
}
