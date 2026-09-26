// Rešenje zadatka z3_izuzetak_drzi_mutex.

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

std::mutex m;
std::vector<int> bafer;

// Ako je m.lock() ... m.unlock() ručno (nije dobro): throw (ili return)
// između njih preskoči unlock, i mutex ostane zaključan zauvek.
// Treba ovako: lock_guard otključa u destruktoru, na svakom izlazu iz funkcije.
void upisi(int v) {
    std::lock_guard<std::mutex> g(m);
    if (v < 0) throw std::invalid_argument("negativno merenje");
    bafer.push_back(v);
}

int main() {
    upisi(5);
    try {
        upisi(-1);
    } catch (const std::exception& e) {
        std::cout << "uhvaćeno: " << e.what() << '\n';
    }
    bool slobodan = false;
    std::thread proba([&slobodan] {
        slobodan = m.try_lock();
        if (slobodan) m.unlock();
    });
    proba.join();
    std::cout << std::boolalpha << "mutex slobodan posle izuzetka: " << slobodan << '\n';
    std::cout << "u baferu: " << bafer.size() << '\n';
}
