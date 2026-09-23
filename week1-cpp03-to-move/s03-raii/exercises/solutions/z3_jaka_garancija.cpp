// Rešenje zadatka z3_jaka_garancija.

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct Konfiguracija {
    std::vector<int> kanali;
    std::string ime;
};

void proveriIme(const std::string& ime) {
    if (ime.empty()) throw std::invalid_argument("prazno ime");
}

void ispisi(const char* opis, const Konfiguracija& k) {
    std::cout << opis << ": kanali";
    for (int c : k.kanali) std::cout << ' ' << c;
    std::cout << ", ime " << k.ime << '\n';
}

// Ako menjaš original deo po deo (nije dobro): izuzetak u sredini ostavi
// objekat napola promenjen.
// Treba ovako: sve što može da baci radi na kopiji, a original se menja
// samo operacijom koja ne baca (copy-and-swap).
void primeni(Konfiguracija& k, const std::vector<int>& kanali, const std::string& ime) {
    Konfiguracija nova = k;       // može da baci (alokacija) -- k netaknut
    nova.kanali = kanali;         // može da baci -- k netaknut
    proveriIme(ime);              // može da baci -- k netaknut
    nova.ime = ime;
    std::swap(k, nova);           // move vector-a i string-a: noexcept
}
// Možeš i ovako, kad je jeftinije: prvo SVE provere, pa tek onda izmene
// (ovde bi bilo dovoljno pozvati proveriIme pre k.kanali = kanali). To
// radi samo dok nijedna izmena ne može da baci -- a k.kanali = kanali
// može (alokacija).

int main() {
    Konfiguracija k{{1, 2, 3}, "staro"};
    try {
        primeni(k, {4, 5, 6}, "");
    } catch (const std::invalid_argument&) {
        ispisi("posle greške", k);
    }
    primeni(k, {7, 8}, "novo");
    ispisi("posle uspeha", k);
}
