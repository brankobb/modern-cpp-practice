// KIND: why
// SANITIZER: thread
// DEMO-OUT: NAIVE A met B: false, B met A: true
//
// Zadatak 2 -- zašto "pokreni i zaboravi" sa std::async ne radi
// paralelno (sekcija 4, EMC Item 38)
// Rešenje: exercises/solutions/ex2_discarded_future.cpp
//
// Dva zadatka treba da rade ISTOVREMENO. Svaki javi "stigao sam" (svoj
// promise) i čeka do 300 ms da stigne i drugi.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 8-concurrency/40-async-and-future/exercises/ex2_discarded_future.cpp -DNAIVE
//   Prvo: kompajler upozori "ignoring return value ... nodiscard" --
//   libstdc++ označava std::async sa [[nodiscard]], i to baš zbog ovoga.
//   Zatim: A čeka B 300 ms uzalud, i tek onda počne B.
//   Future koji vrati std::async je privremen objekat; uništi se na kraju
//   iskaza, a destruktor future-a iz async-a ČEKA kraj zadatka. Zato se
//   drugi async ne pozove dok se prvi zadatak ne završi -- zadaci rade
//   jedan za drugim.
// Korak 2: u #else grani sačuvaj oba future-a u promenljive i pozovi
//   get() (ili wait()) tek pošto su oba pokrenuta.

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

#ifdef NAIVE
void launchBoth() {
    std::async(std::launch::async, taskA);
    std::async(std::launch::async, taskB);
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne pokreće ništa)
void launchBoth() {}
#endif

int main() {
    launchBoth();
    std::cout << std::boolalpha << "A met B: " << aMetB << ", B met A: " << bMetA << '\n';
}

/* EXPECTED OUTPUT
A met B: true, B met A: true
*/
