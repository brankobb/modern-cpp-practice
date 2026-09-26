// Rešenje zadatka ex2_odbacen_future.

#include <chrono>
#include <future>
#include <iostream>

std::promise<void> stigaoA, stigaoB;
std::shared_future<void> signalA = stigaoA.get_future().share();
std::shared_future<void> signalB = stigaoB.get_future().share();
bool aSreoB = false, bSreoA = false;

void zadatakA() {
    stigaoA.set_value();
    aSreoB = signalB.wait_for(std::chrono::milliseconds(300)) == std::future_status::ready;
}
void zadatakB() {
    stigaoB.set_value();
    bSreoA = signalA.wait_for(std::chrono::milliseconds(300)) == std::future_status::ready;
}

// Ako se rezultat std::async-a odbaci (nije dobro): privremeni future se
// uništi odmah, a njegov destruktor čeka kraj zadatka -- zadaci idu
// jedan za drugim.
// Treba ovako: oba future-a žive dok oba zadatka ne krenu; čeka se posle.
void pokreniOba() {
    auto a = std::async(std::launch::async, zadatakA);
    auto b = std::async(std::launch::async, zadatakB);
    a.get();
    b.get();
}

int main() {
    pokreniOba();
    std::cout << std::boolalpha << "A je sreo B: " << aSreoB << ", B je sreo A: " << bSreoA << '\n';
}
