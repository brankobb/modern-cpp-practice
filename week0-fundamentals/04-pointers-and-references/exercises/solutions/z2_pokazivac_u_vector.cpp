// Rešenje zadatka z2_pokazivac_u_vector.

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

struct Senzor {
    std::string ime;
    double temp;
};

// Ako dodaj() vraća Senzor* (nije dobro): kad size() pređe capacity(),
// push_back alocira novi veći blok, premesti elemente i OSLOBODI stari --
// svi ranije vraćeni pokazivači pokazuju u oslobođenu memoriju.
// Treba ovako: vrati indeks. Indeks ostaje tačan i posle realokacije, jer
// elementi zadržavaju redosled. Referencu uzmi tek kad je koristiš, i ne
// čuvaj je preko sledećeg dodaj().
//
// Korak 3: reserve(n) pomaže samo ako unapred znaš gornju granicu; čim je
// pređeš, stari pokazivači ponovo vise -- greška se samo odloži. (Ako baš
// trebaju stabilne adrese: std::deque pri dodavanju na kraj, ili
// vector<unique_ptr<Senzor>> -- week2 s08.)
class Registar {
public:
    std::size_t dodaj(std::string ime, double temp) {
        senzori_.push_back({std::move(ime), temp});
        return senzori_.size() - 1;
    }
    Senzor& uzmi(std::size_t i) { return senzori_.at(i); }
    std::size_t broj() const { return senzori_.size(); }

private:
    std::vector<Senzor> senzori_;
};

int main() {
    Registar r;
    std::size_t motor = r.dodaj("motor", 70.0);
    for (int i = 0; i < 100; ++i) r.dodaj("s" + std::to_string(i), i);
    r.uzmi(motor).temp += 1.0;
    std::cout << r.uzmi(motor).ime << ' ' << r.uzmi(motor).temp << '\n';
    std::cout << "ukupno: " << r.broj() << '\n';
}
