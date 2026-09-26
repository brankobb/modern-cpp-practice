// Rešenje zadatka ex1_ownership.

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Sensor {
    explicit Sensor(int sensorId) : id(sensorId) { std::cout << "Sensor(" << id << ")\n"; }
    ~Sensor() { std::cout << "~Sensor(" << id << ")\n"; }
    int id;
};

struct Config {
    explicit Config(std::string i) : name(std::move(i)) {}
    std::string name;
    ~Config() { std::cout << "~Config(" << name << ")\n"; }
};

// Korak 1: fabrika vraća unique_ptr -- pozivalac postaje jedini vlasnik
// (R.20). Ako mu treba deljeno vlasništvo, unique_ptr se konvertuje u
// shared_ptr; obrnuto ne može.
std::unique_ptr<Sensor> makeSensor(int id) { return std::make_unique<Sensor>(id); }

class System {
public:
    // Sink po vrednosti (R.32): potpis kaže "preuzimam vlasništvo".
    void add(std::unique_ptr<Sensor> s) { sensors_.push_back(std::move(s)); }
    std::size_t count() const { return sensors_.size(); }

private:
    std::vector<std::unique_ptr<Sensor>> sensors_;
};   // uništavanje: vektor uništi elemente od prvog ka poslednjem

// Korak 2: deljeno vlasništvo -- objekat živi dok ga drži bar jedan.
struct Module {
    std::shared_ptr<const Config> cfg;
};

// Korak 3: weak_ptr ne produžava život; lock() je jedini bezbedan pristup.
struct Observer {
    std::weak_ptr<const Config> cfg;
    void check() const {
        if (auto p = cfg.lock())
            std::cout << "observer sees: " << p->name << '\n';
        else
            std::cout << "observer sees: nothing (object destroyed)\n";
    }
};

int main() {
    {
        System sys;
        auto s = makeSensor(1);
        sys.add(std::move(s));
        sys.add(makeSensor(2));
        std::cout << "s after handing over: " << (s ? "full" : "empty") << ", in the system: " << sys.count() << '\n';
    }

    Observer obs;
    {
        // make_shared prosledi "v1" konstruktoru Config-a: objekat se pravi
        // direktno u bloku koji deli sa brojačem referenci (jedna alokacija).
        auto cfg = std::make_shared<const Config>("v1");
        Module a{cfg}, b{cfg};
        obs.cfg = cfg;
        std::cout << "owners: " << cfg.use_count() << '\n';
        obs.check();
    }
    obs.check();
}
