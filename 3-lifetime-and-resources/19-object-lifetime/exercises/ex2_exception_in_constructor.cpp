// KIND: why
// DEMO-UB: NAIVE detected memory leaks
//
// Zadatak 2 -- zašto destruktor ne čisti za konstruktorom koji je bacio
// (sekcija 4)
// Rešenje: exercises/solutions/ex2_exception_in_constructor.cpp
//
// Sensor u konstruktoru zauzme dva bafera. Drugi korak (kalibracija) baci
// izuzetak.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/19-object-lifetime/exercises/ex2_exception_in_constructor.cpp -DNAIVE
//   Na izlazu nema "~Sensor", a LeakSanitizer prijavi curenje oba bafera.
//   Objekat počinje da živi tek kad se konstruktor ZAVRŠI; ovaj se nije
//   završio, pa destruktor koji bi uradio delete[] ne postoji za njega.
//   (Red "caught" može da se ne vidi ako izlaz preusmeriš u fajl ili
//   pipe -- LeakSanitizer završi program pre pražnjenja bafera.)
// Korak 2: u #else grani napiši Sensor tako da bafere drže ČLANOVI sa
//   sopstvenim destruktorom: std::vector<int> ili std::unique_ptr<int[]>.
//   Već napravljeni članovi SE uništavaju i kad konstruktor baci -- pa
//   nema curenja, a Sensor više ne treba ni svoj destruktor.
// Korak 3: dodaj u Sensor član Trace (kao u ex1) pre bafera i proveri u
//   izlazu da se njegov destruktor pozove iako konstruktor Sensor-a baci.

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

void calibrate(bool success) {
    if (!success) throw std::runtime_error("calibration failed");
}

#ifdef NAIVE
class Sensor {
public:
    explicit Sensor(bool success) {
        raw_ = new int[64];
        filtered_ = new int[64];
        calibrate(success);          // baca: raw_ i filtered_ ostaju zauzeti
    }
    ~Sensor() {
        std::cout << "~Sensor\n";
        delete[] filtered_;
        delete[] raw_;
    }
    Sensor(const Sensor&) = delete;
    Sensor& operator=(const Sensor&) = delete;

private:
    int* raw_;
    int* filtered_;
};

int main() {
    try {
        Sensor s(false);
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << '\n';
    }
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // try {
    //     Sensor s(false);
    // } catch (const std::exception& e) {
    //     std::cout << "\ncaught: " << e.what() << '\n';
    // }
    // {
    //     Sensor ok(true);
    //     std::cout << "\nok: " << ok.size() << " elements\n";
    // }
    // std::cout << '\n';
}
#endif

/* EXPECTED OUTPUT
 trace() ~trace()
caught: calibration failed
 trace()
ok: 64 elements
 ~trace()
*/
