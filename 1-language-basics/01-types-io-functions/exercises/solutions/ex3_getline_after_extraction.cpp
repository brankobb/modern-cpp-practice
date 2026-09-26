// Rešenje zadatka ex3_getline_after_extraction.

#include <iostream>
#include <sstream>
#include <string>

const char* config =
    "2\n"
    "101\n"
    "Engine temperature sensor\n"
    "102\n"
    "Oil pressure sensor\n";

// Ako posle in >> x odmah pozoveš getline (nije dobro): getline pročita
// ostatak TEKUĆEG reda -- samo '\n' iza broja -- i vrati prazan string.
// Treba ovako: in >> std::ws preskoči praznine pre getline-a. I svako
// čitanje se proverava: loš ulaz prekida učitavanje umesto da nastavi sa
// smećem.
void load(std::istream& in) {
    int n = 0;
    if (!(in >> n)) return;
    for (int i = 1; i <= n; ++i) {
        int id = 0;
        std::string name;
        if (!(in >> id) || !std::getline(in >> std::ws, name)) {
            std::cout << "sensor " << i << ": bad input\n";
            return;
        }
        std::cout << "sensor " << i << ": id " << id << ", name [" << name << "]\n";
    }
}

int main() {
    std::istringstream in(config);
    load(in);
}
