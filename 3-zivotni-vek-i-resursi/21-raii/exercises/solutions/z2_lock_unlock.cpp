// Rešenje zadatka z2_lock_unlock.

#include <iostream>
#include <mutex>
#include <stdexcept>

struct Mutex {
    bool zakljucan = false;
    void lock() {
        if (zakljucan) throw std::logic_error("deadlock");
        zakljucan = true;
    }
    void unlock() { zakljucan = false; }
};

// Ako pišeš m.lock(); ... m.unlock(); (nije dobro): svaki izuzetak ili
// rani return između ostavi mutex zaključan zauvek.
// Treba ovako: otključavanje je u destruktoru lokalnog objekta, a
// destruktori lokalnih objekata se pozivaju pri SVAKOM izlasku iz bloka.
void obradi(Mutex& m, int x) {
    std::lock_guard<Mutex> guard(m);
    if (x < 0) throw std::invalid_argument("negativna vrednost");
    std::cout << "obrađeno: " << x << '\n';
}

int main() {
    Mutex m;
    try {
        obradi(m, -1);
    } catch (const std::invalid_argument& e) {
        std::cout << "prva obrada: " << e.what() << '\n';
    }
    try {
        obradi(m, 5);
    } catch (const std::logic_error& e) {
        std::cout << "druga obrada: " << e.what() << '\n';
    }
    std::cout << "mutex na kraju: " << (m.zakljucan ? "zaključan" : "otključan") << '\n';
}
