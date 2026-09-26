// KIND: why
// DEMO-OUT: NAIVNO druga obrada: deadlock
//
// Zadatak 2 -- zašto lock_guard, a ne lock() ... unlock() (sekcije 1, 2)
// Rešenje: exercises/solutions/ex2_lock_unlock.cpp
//
// Mutex je ovde simuliran (jedna nit): lock() na već zaključanom baci
// izuzetak "deadlock" -- pravi std::mutex bi u tom slučaju zauvek čekao.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/21-raii/exercises/ex2_lock_unlock.cpp -DNAIVNO
//   Prva obrada baci izuzetak između lock() i unlock(). Izuzetak je
//   uhvaćen, program ide dalje -- ali unlock() se nikad nije izvršio.
//   Sledeća obrada zaglavi. Svaki "return" ili izuzetak između lock i
//   unlock je ista greška.
// Korak 2: u #else grani napiši obradi() sa std::lock_guard<Mutex>
//   (<mutex>). lock_guard radi sa SVAKIM tipom koji ima lock() i
//   unlock() -- ne mora std::mutex. Destruktor lock_guard-a otključa
//   i kad blok napusti izuzetak (stack unwinding).

#include <iostream>
#include <mutex>
#include <stdexcept>

struct Mutex {
    bool zakljucan = false;
    void lock() {
        if (zakljucan) throw std::logic_error("deadlock");
        zakljucan = true;
    }
    void unlock() { zakljucan = false; }
};

#ifdef NAIVNO
void obradi(Mutex& m, int x) {
    m.lock();
    if (x < 0) throw std::invalid_argument("negativna vrednost");
    std::cout << "obrađeno: " << x << '\n';
    m.unlock();
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija samo baca, da bi se fajl kompajlirao)
void obradi(Mutex&, int) { throw std::invalid_argument("nije napisano"); }
#endif

int main() {
    Mutex m;
    try {
        obradi(m, -1);
    } catch (const std::invalid_argument& e) {
        std::cout << "prva obrada: " << e.what() << '\n';
    }
    try {
        obradi(m, 5);
    } catch (const std::logic_error& e) {
        std::cout << "druga obrada: " << e.what() << '\n';
    }
    std::cout << "mutex na kraju: " << (m.zakljucan ? "zaključan" : "otključan") << '\n';
}

/* EXPECTED OUTPUT
prva obrada: negativna vrednost
obrađeno: 5
mutex na kraju: otključan
*/
