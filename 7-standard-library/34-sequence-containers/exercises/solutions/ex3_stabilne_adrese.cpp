// Rešenje zadatka ex3_stabilne_adrese.

#include <deque>
#include <iostream>
#include <string>
#include <vector>

struct Uredjaj {
    std::string ime;
    int adresa;               // adresa na magistrali (npr. I2C)
    const Uredjaj* master;
};

int main() {
    // Ako čuvaš pokazivače na elemente vektora koji raste (nije dobro):
    // rast premesti elemente, pokazivači vise.
    // Treba ovako (kad pokazivači moraju da postoje): kontejner koji ne
    // premešta elemente pri dodavanju na kraj -- deque (ili list).
    // Cena: deque nema neprekidnu memoriju (nema data()), a pristup je malo
    // sporiji; list ima alokaciju po elementu i nema pristup po indeksu.
    std::deque<Uredjaj> magistrala;
    magistrala.push_back({"master", 1, nullptr});
    const Uredjaj* master = &magistrala.front();
    for (int i = 1; i <= 20; ++i) magistrala.push_back({"senzor-" + std::to_string(i), 10 + i, master});
    std::cout << magistrala.back().ime << " -> master na adresi " << magistrala.back().master->adresa << '\n';
    std::cout << "uređaja: " << magistrala.size() << '\n';
}
