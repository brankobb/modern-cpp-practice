// Rešenje zadatka ex3_slicing.

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Korak 3: bazna klasa ne može da se kopira, pa slicing ne može da se
// desi slučajno (ni push_back u vector<Senzor>, ni parametar po vrednosti).
struct Senzor {
    Senzor() = default;
    Senzor(const Senzor&) = delete;
    Senzor& operator=(const Senzor&) = delete;
    virtual ~Senzor() = default;
    virtual std::string opis() const { return "senzor opšti, vrednost 0"; }
};

struct TermoSenzor : Senzor {
    explicit TermoSenzor(double t) : temp(t) {}
    std::string opis() const override {
        std::ostringstream os;
        os << "termo, " << std::fixed << std::setprecision(1) << temp;
        return os.str();
    }
    double temp;
};

int main() {
    // Ako je vector<Senzor> (nije dobro): čuva samo Senzor delove kopija.
    // Treba ovako: kontejner pokazivača-vlasnika. Objekti ostaju celi, a
    // virtualni poziv ide u pravu klasu.
    std::vector<std::unique_ptr<Senzor>> senzori;
    senzori.push_back(std::make_unique<TermoSenzor>(21.5));
    senzori.push_back(std::make_unique<TermoSenzor>(30.0));
    for (const auto& s : senzori) std::cout << s->opis() << '\n';
}
