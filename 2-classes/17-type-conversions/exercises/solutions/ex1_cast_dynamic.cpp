// Rešenje zadatka ex1_cast_dynamic.

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Korak 1: cast na JEDAN operand pre deljenja, pa je deljenje u double.
// static_cast<double>(zbir / n) bi bilo kasno -- celobrojno deljenje je već
// odseklo ostatak.
double prosek(const std::vector<int>& v) {
    if (v.empty()) return 0;
    int zbir = 0;
    for (int x : v) zbir += x;
    return static_cast<double>(zbir) / static_cast<double>(v.size());
}

// Korak 2: dynamic_cast radi samo za polimorfne tipove (errors/e03), i za
// pokazivač vraća nullptr kad objekat nije tog tipa (za referencu baca
// std::bad_cast).
struct Poruka {
    virtual ~Poruka() = default;
};
struct Tekst : Poruka {
    explicit Tekst(std::string s) : sadrzaj(std::move(s)) {}
    std::string sadrzaj;
};
struct Komanda : Poruka {
    explicit Komanda(int k) : kod(k) {}
    int kod;
};

void obradi(const Poruka& p) {
    if (const auto* k = dynamic_cast<const Komanda*>(&p))
        std::cout << "komanda " << k->kod << '\n';
    else
        std::cout << "nije komanda\n";
}

// Korak 3: konverzija na jednom mestu (konstruktor odredišta), obe
// explicit -- nijedna se ne dešava tiho.
struct Fahrenheit {
    double f;
};

class Celsius {
public:
    explicit Celsius(Fahrenheit t) : c_((t.f - 32.0) * 5.0 / 9.0) {}
    explicit operator double() const { return c_; }

private:
    double c_;
};

int main() {
    std::cout << "prosek {3, 4}: " << prosek({3, 4}) << '\n';
    std::cout << "prosek {}: " << prosek({}) << '\n';

    Tekst t("zdravo");
    Komanda k(7);
    obradi(t);
    obradi(k);

    Celsius c(Fahrenheit{212});
    std::cout << "212 F = " << static_cast<double>(c) << " C\n";
}
