// Rešenje zadatka ex2_lock_unlock.

#include <iostream>
#include <mutex>
#include <stdexcept>

struct Mutex {
    bool locked = false;
    void lock() {
        if (locked) throw std::logic_error("deadlock");
        locked = true;
    }
    void unlock() { locked = false; }
};

// Ako pišeš m.lock(); ... m.unlock(); (nije dobro): svaki izuzetak ili
// rani return između ostavi mutex zaključan zauvek.
// Treba ovako: otključavanje je u destruktoru lokalnog objekta, a
// destruktori lokalnih objekata se pozivaju pri SVAKOM izlasku iz bloka.
void process(Mutex& m, int x) {
    std::lock_guard<Mutex> guard(m);
    if (x < 0) throw std::invalid_argument("negative value");
    std::cout << "processed: " << x << '\n';
}

int main() {
    Mutex m;
    try {
        process(m, -1);
    } catch (const std::invalid_argument& e) {
        std::cout << "first run: " << e.what() << '\n';
    }
    try {
        process(m, 5);
    } catch (const std::logic_error& e) {
        std::cout << "second run: " << e.what() << '\n';
    }
    std::cout << "mutex at the end: " << (m.locked ? "locked" : "unlocked") << '\n';
}
