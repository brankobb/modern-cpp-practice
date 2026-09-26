// Rešenje zadatka ex2_index_inserts.

#include <iostream>
#include <map>
#include <string>

// Ako proveravaš m[k] (nije dobro): nepostojeći ključ se ubaci sa
// podrazumevanom vrednošću -- provera menja mapu, i ne radi za const.
// Treba ovako: find ne menja ništa, i radi preko const&.
bool isConfigured(const std::map<std::string, int>& m, const std::string& k) {
    auto it = m.find(k);
    return it != m.end() && it->second != 0;
}

int main() {
    std::map<std::string, int> channels{{"temp", 1}, {"pressure", 2}};
    std::cout << std::boolalpha;
    for (const char* k : {"temp", "humidity", "current", "voltage"})
        std::cout << k << ": " << isConfigured(channels, k) << '\n';
    std::cout << "configured channels after the checks: " << channels.size() << '\n';
}
