// Rešenje zadatka z2_izuzetak_u_konstruktoru.

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

void kalibrisi(bool uspeh) {
    if (!uspeh) throw std::runtime_error("kalibracija nije uspela");
}

class Trag {
public:
    explicit Trag(const char* ime) : ime_(ime) { std::cout << ' ' << ime_ << "()"; }
    ~Trag() { std::cout << " ~" << ime_ << "()"; }
    Trag(const Trag&) = delete;
    Trag& operator=(const Trag&) = delete;

private:
    const char* ime_;
};

// Ako resurse drže sirovi pokazivači, a oslobađa ih destruktor Senzor-a
// (nije dobro): kad konstruktor baci, taj destruktor se ne pozove, i sve
// što je zauzeto u konstruktoru curi.
// Treba ovako: svaki resurs drži član koji je već "ceo objekat" sa svojim
// destruktorom. Pri izuzetku se napravljeni članovi uništavaju obrnutim
// redom -- trag_ se vidi u izlazu, a vektori oslobode memoriju.
class Senzor {
public:
    explicit Senzor(bool uspeh) : trag_("trag"), sirovi_(64), filtrirani_(64) {
        kalibrisi(uspeh);
    }
    // Nema destruktora, kopija se generiše ispravno (rule of 0) -- ali
    // Trag nema kopiju, pa je nema ni Senzor.

    std::size_t velicina() const { return sirovi_.size(); }

private:
    Trag trag_;
    std::vector<int> sirovi_;
    std::vector<int> filtrirani_;
};

int main() {
    try {
        Senzor s(false);
    } catch (const std::exception& e) {
        std::cout << "\nuhvaćeno: " << e.what() << '\n';
    }
    {
        Senzor ok(true);
        std::cout << "\nok: " << ok.velicina() << " elemenata\n";
    }
    std::cout << '\n';
}
