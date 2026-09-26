// Rešenje zadatka ex1_structured_bindings.

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

// Korak 1: const auto& -- bez kopije para (a par sadrži std::string).
// Mapa je sortirana po ključu, pa je redosled abecedni.
void ispisi(const std::map<std::string, int>& z) {
    const char* sep = "";
    for (const auto& [ime, kolicina] : z) {
        std::cout << sep << ime << '=' << kolicina;
        sep = ", ";
    }
    std::cout << '\n';
}

// Korak 2: auto& -- binding-i su tada imena za delove PRAVOG elementa mape.
// Sa "auto [ime, kolicina]" bi se menjala kopija para, a mapa ne.
// (ime je i dalje const: ključ mape se ne sme menjati, jer je
// value_type = std::pair<const std::string, int>.)
void dopuni(std::map<std::string, int>& z, int koliko) {
    for (auto& [ime, kolicina] : z) kolicina += koliko;
}

// Korak 3: auto povratni tip = tip izraza u return: *it je
// const std::pair<const std::string, int>&, a auto odbaci referencu i
// top-level const, pa se vraća KOPIJA std::pair<const std::string, int>.
auto najmanje(const std::map<std::string, int>& z) {
    auto it = std::min_element(z.begin(), z.end(),
                               [](const auto& a, const auto& b) { return a.second < b.second; });
    return *it;
}

int main() {
    std::map<std::string, int> zalihe{{"otpornik", 120}, {"kondenzator", 45}, {"dioda", 80}};
    ispisi(zalihe);

    dopuni(zalihe, 10);
    ispisi(zalihe);

    auto [ime, kol] = najmanje(zalihe);
    std::cout << "najmanje: " << ime << " (" << kol << ")\n";
}
