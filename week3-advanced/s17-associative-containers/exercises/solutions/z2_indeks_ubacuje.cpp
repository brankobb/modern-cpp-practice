// Rešenje zadatka z2_indeks_ubacuje.

#include <iostream>
#include <map>
#include <string>

// Ako proveravaš m[k] (nije dobro): nepostojeći ključ se ubaci sa
// podrazumevanom vrednošću -- provera menja mapu, i ne radi za const.
// Treba ovako: find ne menja ništa, i radi preko const&.
bool konfigurisan(const std::map<std::string, int>& m, const std::string& k) {
    auto it = m.find(k);
    return it != m.end() && it->second != 0;
}

int main() {
    std::map<std::string, int> kanali{{"temp", 1}, {"pritisak", 2}};
    std::cout << std::boolalpha;
    for (const char* k : {"temp", "vlaga", "struja", "napon"})
        std::cout << k << ": " << konfigurisan(kanali, k) << '\n';
    std::cout << "konfigurisanih kanala posle provera: " << kanali.size() << '\n';
}
