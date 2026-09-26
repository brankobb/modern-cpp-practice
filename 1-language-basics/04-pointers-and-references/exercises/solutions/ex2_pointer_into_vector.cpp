// Rešenje zadatka ex2_pointer_into_vector.

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

struct Sensor {
    std::string name;
    double temp;
};

// Ako add() vraća Sensor* (nije dobro): kad size() pređe capacity(),
// push_back alocira novi veći blok, premesti elemente i OSLOBODI stari --
// svi ranije vraćeni pokazivači pokazuju u oslobođenu memoriju.
// Treba ovako: vrati indeks. Indeks ostaje tačan i posle realokacije, jer
// elementi zadržavaju redosled. Referencu uzmi tek kad je koristiš, i ne
// čuvaj je preko sledećeg add().
//
// Korak 3: reserve(n) pomaže samo ako unapred znaš gornju granicu; čim je
// pređeš, stari pokazivači ponovo vise -- greška se samo odloži. (Ako baš
// trebaju stabilne adrese: std::deque pri dodavanju na kraj, ili
// vector<unique_ptr<Sensor>> -- lekcija 32.)
class Registry {
public:
    std::size_t add(std::string name, double temp) {
        sensors_.push_back({std::move(name), temp});
        return sensors_.size() - 1;
    }
    Sensor& get(std::size_t i) { return sensors_.at(i); }
    std::size_t count() const { return sensors_.size(); }

private:
    std::vector<Sensor> sensors_;
};

int main() {
    Registry r;
    std::size_t engine = r.add("engine", 70.0);
    for (int i = 0; i < 100; ++i) r.add("s" + std::to_string(i), i);
    r.get(engine).temp += 1.0;
    std::cout << r.get(engine).name << ' ' << r.get(engine).temp << '\n';
    std::cout << "total: " << r.count() << '\n';
}
