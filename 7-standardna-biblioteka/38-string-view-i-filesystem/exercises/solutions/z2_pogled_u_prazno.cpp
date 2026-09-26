// Rešenje zadatka z2_pogled_u_prazno.

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// Ako je član string_view (nije dobro): gleda u lokalni string funkcije
// napravi(), koji nestane na njenom izlasku -- pogled visi.
// Treba ovako: objekat koji čuva podatak ga i poseduje -- std::string.
struct Kanal {
    std::string ime;
    int broj;
};

Kanal napravi(int broj) { return Kanal{"senzor_temperature_hala_" + std::to_string(broj), broj}; }

int main() {
    std::vector<Kanal> kanali;
    for (int i = 1; i <= 2; ++i) kanali.push_back(napravi(i));
    for (const auto& k : kanali) std::cout << k.broj << ": " << k.ime << '\n';
}
