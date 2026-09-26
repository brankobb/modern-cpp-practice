// KIND: why
// SANITIZER: thread
// DEMO-OUT: NAIVNO mutex slobodan posle izuzetka: false
//
// Zadatak 3 -- zašto lock_guard, a ne lock()/unlock() (sekcija 6)
// Rešenje: exercises/solutions/ex3_izuzetak_drzi_mutex.cpp
//
// upisi() dodaje merenje u deljeni bafer; negativno merenje je greška.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 8-concurrency/39-threads/exercises/ex3_izuzetak_drzi_mutex.cpp -DNAIVNO
//   Posle uhvaćenog izuzetka mutex je i dalje ZAKLJUČAN: throw je
//   preskočio m.unlock(). Sledeća nit koja pozove upisi() čekala bi
//   zauvek (program bi "zaglavio" -- zato ovde proveravamo sa try_lock iz
//   druge niti, koji ne čeka). Isto se desi sa return-om u sredini.
// Korak 2: u #else grani napiši upisi() sa std::lock_guard: destruktor
//   otključa i kad se izađe izuzetkom (RAII, lekcija 21).

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

std::mutex m;
std::vector<int> bafer;

#ifdef NAIVNO
void upisi(int v) {
    m.lock();
    if (v < 0) throw std::invalid_argument("negativno merenje");
    bafer.push_back(v);
    m.unlock();
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne upisuje ništa)
void upisi(int) {}
#endif

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
#ifdef NAIVNO
    if (!slobodan) m.unlock();   // main ga drži od upisi(-1); uništavanje zaključanog mutex-a je UB
#endif
    std::cout << "u baferu: " << bafer.size() << '\n';
}

/* EXPECTED OUTPUT
uhvaćeno: negativno merenje
mutex slobodan posle izuzetka: true
u baferu: 1
*/
