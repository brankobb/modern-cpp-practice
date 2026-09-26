// Rešenje zadatka ex3_end_of_program.

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

// Ako Device koristi logger() samo u destruktoru (nije dobro): Logger
// nastane kasnije (u main-u), pa se uništi ranije, a destruktor Device-a
// piše u uništen objekat.
// Treba ovako: objekat koji će ti trebati u destruktoru, dohvati već u
// konstruktoru. Njegova konstrukcija se tada završi pre tvoje, pa se
// uništava posle tvoje.
//
// Korak 3: "static Logger* l = new Logger;" se nikad ne uništava, pa je
// uvek dostupan (i iz destruktora drugih static objekata). Cena: destruktor
// Logger-a se nikad ne pozove -- ako bi trebalo da isprazni bafer u fajl,
// to se ne desi.
struct Device {
    Device() { logger().log("device on"); }
    ~Device() { logger().log("device off"); }
};

Device device;

int main() { logger().log("main"); }
