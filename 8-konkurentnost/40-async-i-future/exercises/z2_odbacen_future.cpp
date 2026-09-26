// VRSTA: zašto
// SANITIZER: thread
// DEMO-OUT: NAIVNO A je sreo B: false, B je sreo A: true
//
// Zadatak 2 -- zašto "pokreni i zaboravi" sa std::async ne radi
// paralelno (sekcija 4, EMC Item 38)
// Rešenje: exercises/solutions/z2_odbacen_future.cpp
//
// Dva zadatka treba da rade ISTOVREMENO. Svaki javi "stigao sam" (svoj
// promise) i čeka do 300 ms da stigne i drugi.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 8-konkurentnost/40-async-i-future/exercises/z2_odbacen_future.cpp -DNAIVNO
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

#ifdef NAIVNO
void pokreniOba() {
    std::async(std::launch::async, zadatakA);
    std::async(std::launch::async, zadatakB);
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne pokreće ništa)
void pokreniOba() {}
#endif

int main() {
    pokreniOba();
    std::cout << std::boolalpha << "A je sreo B: " << aSreoB << ", B je sreo A: " << bSreoA << '\n';
}

/* OČEKIVANI IZLAZ
A je sreo B: true, B je sreo A: true
*/
