// KIND: why
// SANITIZER: thread
// DEMO-OUT: NAIVE mutex free after the exception: false
//
// Zadatak 3 -- zašto lock_guard, a ne lock()/unlock() (sekcija 6)
// Rešenje: exercises/solutions/ex3_exception_holds_mutex.cpp
//
// append() dodaje merenje u deljeni bafer; negativno merenje je greška.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 8-concurrency/39-threads/exercises/ex3_exception_holds_mutex.cpp -DNAIVE
//   Posle uhvaćenog izuzetka mutex je i dalje ZAKLJUČAN: throw je
//   preskočio m.unlock(). Sledeća nit koja pozove append() čekala bi
//   zauvek (program bi "zaglavio" -- zato ovde proveravamo sa try_lock iz
//   druge niti, koji ne čeka). Isto se desi sa return-om u sredini.
// Korak 2: u #else grani napiši append() sa std::lock_guard: destruktor
//   otključa i kad se izađe izuzetkom (RAII, lekcija 21).

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

std::mutex m;
std::vector<int> buffer;

#ifdef NAIVE
void append(int v) {
    m.lock();
    if (v < 0) throw std::invalid_argument("negative reading");
    buffer.push_back(v);
    m.unlock();
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne upisuje ništa)
void append(int) {}
#endif

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
#ifdef NAIVE
    if (!isFree) m.unlock();   // main ga drži od append(-1); uništavanje zaključanog mutex-a je UB
#endif
    std::cout << "in the buffer: " << buffer.size() << '\n';
}

/* EXPECTED OUTPUT
caught: negative reading
mutex free after the exception: true
in the buffer: 1
*/
