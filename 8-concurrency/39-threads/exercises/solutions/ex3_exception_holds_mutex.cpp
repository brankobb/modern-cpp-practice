// Rešenje zadatka ex3_exception_holds_mutex.

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

std::mutex m;
std::vector<int> buffer;

// Ako je m.lock() ... m.unlock() ručno (nije dobro): throw (ili return)
// između njih preskoči unlock, i mutex ostane zaključan zauvek.
// Treba ovako: lock_guard otključa u destruktoru, na svakom izlazu iz funkcije.
void append(int v) {
    std::lock_guard<std::mutex> g(m);
    if (v < 0) throw std::invalid_argument("negative reading");
    buffer.push_back(v);
}

int main() {
    append(5);
    try {
        append(-1);
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << '\n';
    }
    bool isFree = false;
    std::thread probe([&isFree] {
        isFree = m.try_lock();
        if (isFree) m.unlock();
    });
    probe.join();
    std::cout << std::boolalpha << "mutex free after the exception: " << isFree << '\n';
    std::cout << "in the buffer: " << buffer.size() << '\n';
}
