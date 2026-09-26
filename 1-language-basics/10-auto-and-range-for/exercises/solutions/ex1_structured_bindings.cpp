// Rešenje zadatka ex1_structured_bindings.

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

// Korak 1: const auto& -- bez kopije para (a par sadrži std::string).
// Mapa je sortirana po ključu, pa je redosled abecedni.
void print(const std::map<std::string, int>& s) {
    const char* sep = "";
    for (const auto& [name, quantity] : s) {
        std::cout << sep << name << '=' << quantity;
        sep = ", ";
    }
    std::cout << '\n';
}

// Korak 2: auto& -- binding-i su tada imena za delove PRAVOG elementa mape.
// Sa "auto [name, quantity]" bi se menjala kopija para, a mapa ne.
// (name je i dalje const: ključ mape se ne sme menjati, jer je
// value_type = std::pair<const std::string, int>.)
void restock(std::map<std::string, int>& s, int amount) {
    for (auto& [name, quantity] : s) quantity += amount;
}

// Korak 3: auto povratni tip = tip izraza u return: *it je
// const std::pair<const std::string, int>&, a auto odbaci referencu i
// top-level const, pa se vraća KOPIJA std::pair<const std::string, int>.
auto lowest(const std::map<std::string, int>& s) {
    auto it = std::min_element(s.begin(), s.end(),
                               [](const auto& a, const auto& b) { return a.second < b.second; });
    return *it;
}

int main() {
    std::map<std::string, int> stock{{"resistor", 120}, {"capacitor", 45}, {"diode", 80}};
    print(stock);

    restock(stock, 10);
    print(stock);

    auto [name, qty] = lowest(stock);
    std::cout << "lowest: " << name << " (" << qty << ")\n";
}
