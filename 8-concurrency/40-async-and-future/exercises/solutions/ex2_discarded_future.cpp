// Rešenje zadatka ex2_discarded_future.

#include <chrono>
#include <future>
#include <iostream>

std::promise<void> arrivedA, arrivedB;
std::shared_future<void> signalA = arrivedA.get_future().share();
std::shared_future<void> signalB = arrivedB.get_future().share();
bool aMetB = false, bMetA = false;

void taskA() {
    arrivedA.set_value();
    aMetB = signalB.wait_for(std::chrono::milliseconds(300)) == std::future_status::ready;
}
void taskB() {
    arrivedB.set_value();
    bMetA = signalA.wait_for(std::chrono::milliseconds(300)) == std::future_status::ready;
}

// Ako se rezultat std::async-a odbaci (nije dobro): privremeni future se
// uništi odmah, a njegov destruktor čeka kraj zadatka -- zadaci idu
// jedan za drugim.
// Treba ovako: oba future-a žive dok oba zadatka ne krenu; čeka se posle.
void launchBoth() {
    auto a = std::async(std::launch::async, taskA);
    auto b = std::async(std::launch::async, taskB);
    a.get();
    b.get();
}

int main() {
    launchBoth();
    std::cout << std::boolalpha << "A met B: " << aMetB << ", B met A: " << bMetA << '\n';
}
