// KIND: why
// DEMO-OUT: NAIVE after error: channels 4 5 6, name old
//
// Zadatak 3 -- zašto "sve ili ništa" (strong guarantee, sekcija 4, EC++ Item 29)
// Rešenje: exercises/solutions/ex3_strong_guarantee.cpp
//
// applyChanges() menja konfiguraciju: nove kanale i novo ime. Ime se
// proverava i može da bude neispravno (baci izuzetak).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/21-raii/exercises/ex3_strong_guarantee.cpp -DNAIVE
//   Posle neuspelog applyChanges() konfiguracija ima NOVE kanale i STARO
//   ime -- stanje koje nikad nije trebalo da postoji. Nema curenja (basic
//   guarantee je ispunjen), ali pozivalac ne zna u kakvom je stanju objekat.
// Korak 2: u #else grani napiši applyChanges() sa jakom garancijom:
//   napravi KOPIJU, promeni kopiju (tu sme da baci -- original je
//   netaknut), pa na kraju zameni original sa kopijom operacijom koja ne
//   baca (swap članova, ili std::swap na celom objektu -- move vector-a i
//   string-a je noexcept).

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct Config {
    std::vector<int> channels;
    std::string name;
};

void checkName(const std::string& name) {
    if (name.empty()) throw std::invalid_argument("empty name");
}

void print(const char* label, const Config& c) {
    std::cout << label << ": channels";
    for (int ch : c.channels) std::cout << ' ' << ch;
    std::cout << ", name " << c.name << '\n';
}

#ifdef NAIVE
void applyChanges(Config& c, const std::vector<int>& channels, const std::string& name) {
    c.channels = channels;  // uspe
    checkName(name);        // baci -- kanali su već promenjeni
    c.name = name;
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne radi ništa)
void applyChanges(Config&, const std::vector<int>&, const std::string&) {}
#endif

int main() {
    Config c{{1, 2, 3}, "old"};
    try {
        applyChanges(c, {4, 5, 6}, "");
    } catch (const std::invalid_argument&) {
        print("after error", c);
    }
    applyChanges(c, {7, 8}, "new");
    print("after success", c);
}

/* EXPECTED OUTPUT
after error: channels 1 2 3, name old
after success: channels 7 8, name new
*/
