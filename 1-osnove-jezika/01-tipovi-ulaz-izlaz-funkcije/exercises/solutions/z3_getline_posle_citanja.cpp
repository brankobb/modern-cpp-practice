// Rešenje zadatka z3_getline_posle_citanja.

#include <iostream>
#include <sstream>
#include <string>

const char* konfiguracija =
    "2\n"
    "101\n"
    "Senzor temperature motora\n"
    "102\n"
    "Senzor pritiska ulja\n";

// Ako posle in >> x odmah pozoveš getline (nije dobro): getline pročita
// ostatak TEKUĆEG reda -- samo '\n' iza broja -- i vrati prazan string.
// Treba ovako: in >> std::ws preskoči praznine pre getline-a. I svako
// čitanje se proverava: loš ulaz prekida učitavanje umesto da nastavi sa
// smećem.
void ucitaj(std::istream& in) {
    int n = 0;
    if (!(in >> n)) return;
    for (int i = 1; i <= n; ++i) {
        int id = 0;
        std::string ime;
        if (!(in >> id) || !std::getline(in >> std::ws, ime)) {
            std::cout << "senzor " << i << ": loš ulaz\n";
            return;
        }
        std::cout << "senzor " << i << ": id " << id << ", ime [" << ime << "]\n";
    }
}

int main() {
    std::istringstream in(konfiguracija);
    ucitaj(in);
}
