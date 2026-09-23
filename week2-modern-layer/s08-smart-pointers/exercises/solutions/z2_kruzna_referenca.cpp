// Rešenje zadatka z2_kruzna_referenca.

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Ako i roditelj i dete drže shared_ptr jedan na drugog (nije dobro):
// ciklus -- brojači nikad ne padnu na 0, niko se ne uništi.
// Treba ovako: vlasništvo samo u jednom smeru (roditelj poseduje decu), a
// pokazivač nazad je weak_ptr.
struct Cvor {
    explicit Cvor(std::string i) : ime(std::move(i)) {}
    ~Cvor() { std::cout << "~Cvor(" << ime << ")\n"; }
    std::string ime;
    std::weak_ptr<Cvor> roditelj;
    std::vector<std::shared_ptr<Cvor>> deca;

    std::string imeRoditelja() const {
        if (auto r = roditelj.lock()) return r->ime;
        return "(nema)";
    }
};

int main() {
    {
        auto koren = std::make_shared<Cvor>("koren");
        auto dete = std::make_shared<Cvor>("dete");
        dete->roditelj = koren;
        koren->deca.push_back(dete);
        std::cout << "koren use_count: " << koren.use_count() << '\n';
        std::cout << "roditelj deteta: " << dete->imeRoditelja() << '\n';
    }
    // Redosled: lokalne se uništavaju obrnuto -- prvo "dete" (use_count
    // deteta pada na 1, drži ga koren), pa "koren" (pada na 0): ~Cvor(koren),
    // a u njegovom destruktoru vektor dece pusti dete -> ~Cvor(dete).
    std::cout << "kraj bloka\n";
}
