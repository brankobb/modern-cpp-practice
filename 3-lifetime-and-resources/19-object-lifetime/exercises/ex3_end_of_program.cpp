// KIND: why
// DEMO-UB: NAIVE AddressSanitizer: (attempting double-free|heap-use-after-free)
//
// Zadatak 3 -- zašto je redosled uništavanja static objekata bitan (sekcija 5)
// Rešenje: exercises/solutions/ex3_end_of_program.cpp
//
// logger() vraća static lokalni Logger (Meyers singleton, lekcija 08).
// Globalni Device u destruktoru upiše poruku u log.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/19-object-lifetime/exercises/ex3_end_of_program.cpp -DNAIVE
//   Static objekti se uništavaju obrnutim redom od ZAVRŠETKA konstrukcije.
//   Device je završen pre main-a, a Logger tek pri prvom pozivu logger()
//   u main-u -- pa se Logger uništi PRVI. Destruktor Device-a zatim piše u
//   uništen vector: ASan prijavi grešku (double-free ili
//   heap-use-after-free, zavisi od stanja vektora).
// Korak 2: u #else grani popravi redosled bez menjanja logger(): neka
//   konstruktor Device-a pozove logger() (npr. upiše "device on").
//   Tada Logger završi konstrukciju PRE Device-a, pa se uništava POSLE.
// Korak 3: (za razmišljanje) druga poznata varijanta je logger koji se
//   nikad ne uništava: static Logger* l = new Logger; return *l;
//   Šta se time dobija, a šta gubi? (odgovor je u rešenju)

#include <iostream>
#include <string>
#include <vector>

struct Logger {
    std::vector<std::string> lines;
    void log(const std::string& s) {
        lines.push_back(s);
        std::cout << "log: " << s << '\n';
    }
    ~Logger() { std::cout << "~Logger (" << lines.size() << " lines)\n"; }
};

Logger& logger() {
    static Logger l;
    return l;
}

#ifdef NAIVE
struct Device {
    ~Device() { logger().log("device off"); }
};
#else
struct Device {
    // TODO korak 2
};
#endif

Device device;

int main() { logger().log("main"); }

/* EXPECTED OUTPUT
log: device on
log: main
log: device off
~Logger (3 lines)
*/
