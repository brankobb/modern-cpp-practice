// EXPECT-UB: LeakSanitizer: detected memory leaks
// POGREŠNO: konstruktor zauzme memoriju sirovim new, pa sledeći korak baci.
// Zašto: kad Config() baci, Service nikad nije napravljen, pa se ~Service
//   ne poziva (s01, sekcija 4). buffer_ je int* -- pokazivač nema
//   destruktor, pa 1024 bajta ostanu zauzeta zauvek.
// Ispravno: resurs drži član koji se sam oslobađa:
//   std::unique_ptr<int[]> buffer_; std::unique_ptr<Config> config_;
//   (main.cpp, sekcija 3). Tada se već napravljen buffer_ uništi.
#include <cstdio>
#include <stdexcept>

struct Config {
    Config() { throw std::runtime_error("loš config"); }
};

class Service {
public:
    Service() : buffer_(new int[256]), config_(new Config) {}
    ~Service() {
        delete config_;
        delete[] buffer_;
    }

private:
    int* buffer_;
    Config* config_;
};

int main() {
    try {
        Service s;
    } catch (const std::exception& e) {
        std::printf("uhvaćen: %s\n", e.what());
    }
}
