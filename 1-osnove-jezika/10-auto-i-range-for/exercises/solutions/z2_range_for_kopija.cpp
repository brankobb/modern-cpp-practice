// Rešenje zadatka z2_range_for_kopija.

#include <iostream>
#include <vector>

int kopija = 0;

struct Senzor {
    double temp;
    Senzor(double t) : temp(t) {}
    Senzor(const Senzor& o) : temp(o.temp) { ++kopija; }
    Senzor& operator=(const Senzor&) = default;
};

// Ako pišeš "for (auto s : v) s.temp = 0;" (nije dobro): s je kopija, pa
// se menja kopija, a original ostaje isti; uz to se plati kopija elementa.
// Treba ovako: auto& kad menjaš, const auto& kad samo čitaš.
void reset(std::vector<Senzor>& v) {
    for (auto& s : v) s.temp = 0;
}

double prosek(const std::vector<Senzor>& v) {
    double sum = 0;
    for (const auto& s : v) sum += s.temp;
    return sum / static_cast<double>(v.size());
}

int main() {
    std::vector<Senzor> senzori{70.0, 80.0, 90.0};
    kopija = 0;
    std::cout << "prosek pre: " << prosek(senzori) << '\n';
    reset(senzori);
    std::cout << "kopija: " << kopija << ", s[0].temp = " << senzori[0].temp << '\n';
    std::cout << "prosek posle: " << prosek(senzori) << '\n';
}
