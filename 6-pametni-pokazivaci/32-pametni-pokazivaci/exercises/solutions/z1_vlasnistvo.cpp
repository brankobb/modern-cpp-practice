// Rešenje zadatka z1_vlasnistvo.

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Senzor {
    explicit Senzor(int senzorId) : id(senzorId) { std::cout << "Senzor(" << id << ")\n"; }
    ~Senzor() { std::cout << "~Senzor(" << id << ")\n"; }
    int id;
};

struct Konfig {
    explicit Konfig(std::string i) : ime(std::move(i)) {}
    std::string ime;
    ~Konfig() { std::cout << "~Konfig(" << ime << ")\n"; }
};

// Korak 1: fabrika vraća unique_ptr -- pozivalac postaje jedini vlasnik
// (R.20). Ako mu treba deljeno vlasništvo, unique_ptr se konvertuje u
// shared_ptr; obrnuto ne može.
std::unique_ptr<Senzor> napraviSenzor(int id) { return std::make_unique<Senzor>(id); }

class Sistem {
public:
    // Sink po vrednosti (R.32): potpis kaže "preuzimam vlasništvo".
    void dodaj(std::unique_ptr<Senzor> s) { senzori_.push_back(std::move(s)); }
    std::size_t broj() const { return senzori_.size(); }

private:
    std::vector<std::unique_ptr<Senzor>> senzori_;
};   // uništavanje: vektor uništi elemente od prvog ka poslednjem

// Korak 2: deljeno vlasništvo -- objekat živi dok ga drži bar jedan.
struct Modul {
    std::shared_ptr<const Konfig> cfg;
};

// Korak 3: weak_ptr ne produžava život; lock() je jedini bezbedan pristup.
struct Posmatrac {
    std::weak_ptr<const Konfig> cfg;
    void proveri() const {
        if (auto p = cfg.lock())
            std::cout << "posmatrač vidi: " << p->ime << '\n';
        else
            std::cout << "posmatrač vidi: ništa (objekat uništen)\n";
    }
};

int main() {
    {
        Sistem sis;
        auto s = napraviSenzor(1);
        sis.dodaj(std::move(s));
        sis.dodaj(napraviSenzor(2));
        std::cout << "s posle predaje: " << (s ? "pun" : "prazan") << ", u sistemu: " << sis.broj() << '\n';
    }

    Posmatrac pos;
    {
        // make_shared prosledi "v1" konstruktoru Konfig-a: objekat se pravi
        // direktno u bloku koji deli sa brojačem referenci (jedna alokacija).
        auto cfg = std::make_shared<const Konfig>("v1");
        Modul a{cfg}, b{cfg};
        pos.cfg = cfg;
        std::cout << "vlasnika: " << cfg.use_count() << '\n';
        pos.proveri();
    }
    pos.proveri();
}
