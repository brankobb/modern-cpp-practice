// KIND: why
// DEMO-OUT: NAIVE second run: deadlock
//
// Zadatak 2 -- zašto lock_guard, a ne lock() ... unlock() (sekcije 1, 2)
// Rešenje: exercises/solutions/ex2_lock_unlock.cpp
//
// Mutex je ovde simuliran (jedna nit): lock() na već zaključanom baci
// izuzetak "deadlock" -- pravi std::mutex bi u tom slučaju zauvek čekao.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/21-raii/exercises/ex2_lock_unlock.cpp -DNAIVE
//   Prva obrada baci izuzetak između lock() i unlock(). Izuzetak je
//   uhvaćen, program ide dalje -- ali unlock() se nikad nije izvršio.
//   Sledeća obrada zaglavi. Svaki "return" ili izuzetak između lock i
//   unlock je ista greška.
// Korak 2: u #else grani napiši process() sa std::lock_guard<Mutex>
//   (<mutex>). lock_guard radi sa SVAKIM tipom koji ima lock() i
//   unlock() -- ne mora std::mutex. Destruktor lock_guard-a otključa
//   i kad blok napusti izuzetak (stack unwinding).

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

#ifdef NAIVE
void process(Mutex& m, int x) {
    m.lock();
    if (x < 0) throw std::invalid_argument("negative value");
    std::cout << "processed: " << x << '\n';
    m.unlock();
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija samo baca, da bi se fajl kompajlirao)
void process(Mutex&, int) { throw std::invalid_argument("not written yet"); }
#endif

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

/* EXPECTED OUTPUT
first run: negative value
processed: 5
mutex at the end: unlocked
*/
