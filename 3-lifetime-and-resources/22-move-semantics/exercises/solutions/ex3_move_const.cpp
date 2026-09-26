// Rešenje zadatka ex3_move_const.

#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

struct Tekst {
    std::string s;
    explicit Tekst(std::string x) : s(std::move(x)) {}
    Tekst(const Tekst& o) : s(o.s) { ++brojac.kopija; }
    Tekst(Tekst&& o) noexcept : s(std::move(o.s)) { ++brojac.pomeranja; }
};

int main() {
    std::vector<Tekst> red;
    red.reserve(1);
    // Ako je promenljiva const, a posle je pomeraš (nije dobro): std::move
    // da const Tekst&&, move konstruktor ne može da ga primi, i dobije se
    // kopija -- bez ikakvog upozorenja.
    // Treba ovako: objekat koji ćeš pomeriti nije const (EMC Item 23:
    // "ne deklariši objekte const ako želiš da ih pomeriš").
    Tekst poruka("dugačka poruka koja se šalje u red");
    red.push_back(std::move(poruka));
    std::cout << "kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
    // Moved-from std::string je u "validnom, ali nespecifikovanom stanju".
    // libstdc++ ga ostavi praznim (provereno); standard to ne garantuje, pa
    // se na to ne oslanjaj u pravom kodu. (Ako na Windows-u/libc++ vidiš
    // nešto drugo, javi.)
    std::cout << "poruka posle: \"" << poruka.s << "\"\n";
}
