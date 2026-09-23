// VRSTA: zašto
// DEMO-UB: NAIVNO AddressSanitizer: (attempting double-free|heap-use-after-free)
//
// Zadatak 3 -- zašto je redosled uništavanja static objekata bitan (sekcija 5)
// Rešenje: exercises/solutions/z3_kraj_programa.cpp
//
// logger() vraća static lokalni Logger (Meyers singleton, lekcija 06).
// Globalni Uredjaj u destruktoru upiše poruku u log.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week1-cpp03-to-move/s01-object-lifetime/exercises/z3_kraj_programa.cpp -DNAIVNO
//   Static objekti se uništavaju obrnutim redom od ZAVRŠETKA konstrukcije.
//   Uredjaj je završen pre main-a, a Logger tek pri prvom pozivu logger()
//   u main-u -- pa se Logger uništi PRVI. Destruktor Uredjaja zatim piše u
//   uništen vector: ASan prijavi grešku (double-free ili
//   heap-use-after-free, zavisi od stanja vektora).
// Korak 2: u #else grani popravi redosled bez menjanja logger(): neka
//   konstruktor Uredjaja pozove logger() (npr. upiše "uredjaj upaljen").
//   Tada Logger završi konstrukciju PRE Uredjaja, pa se uništava POSLE.
// Korak 3: (za razmišljanje) druga poznata varijanta je logger koji se
//   nikad ne uništava: static Logger* l = new Logger; return *l;
//   Šta se time dobija, a šta gubi? (odgovor je u rešenju)

#include <iostream>
#include <string>
#include <vector>

struct Logger {
    std::vector<std::string> linije;
    void log(const std::string& s) {
        linije.push_back(s);
        std::cout << "log: " << s << '\n';
    }
    ~Logger() { std::cout << "~Logger (" << linije.size() << " linija)\n"; }
};

Logger& logger() {
    static Logger l;
    return l;
}

#ifdef NAIVNO
struct Uredjaj {
    ~Uredjaj() { logger().log("uredjaj ugasen"); }
};
#else
struct Uredjaj {
    // TODO korak 2
};
#endif

Uredjaj uredjaj;

int main() { logger().log("main"); }

/* OČEKIVANI IZLAZ
log: uredjaj upaljen
log: main
log: uredjaj ugasen
~Logger (3 linija)
*/
