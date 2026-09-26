// Rešenje zadatka ex1_parsiranje_konfiguracije.

#include <iostream>
#include <map>
#include <sstream>
#include <string>

// Korak 1: npos znači "nije nađeno" -- string od samih razmaka.
std::string trim(const std::string& s) {
    const char* razmaci = " \t";
    auto prvi = s.find_first_not_of(razmaci);
    if (prvi == std::string::npos) return "";
    auto poslednji = s.find_last_not_of(razmaci);
    return s.substr(prvi, poslednji - prvi + 1);
}

// Korak 2: getline sa graničnikom je najjednostavniji "split".
std::map<std::string, std::string> parsiraj(const std::string& ulaz) {
    std::map<std::string, std::string> m;
    std::istringstream is(ulaz);
    std::string deo;
    while (std::getline(is, deo, ';')) {
        deo = trim(deo);
        auto jednako = deo.find('=');
        if (deo.empty() || jednako == std::string::npos) continue;
        m[trim(deo.substr(0, jednako))] = trim(deo.substr(jednako + 1));
    }
    return m;
}

// Korak 3: R"(...)" -- sve između zagrada je doslovno, i navodnici.
std::string uJson(const std::map<std::string, std::string>& m) {
    std::ostringstream os;
    os << '{';
    const char* sep = "";
    for (const auto& [k, v] : m) {
        os << sep << R"(")" << k << R"(":")" << v << R"(")";
        sep = ",";
    }
    os << '}';
    return os.str();
}

int main() {
    std::cout << '[' << trim("  a b  ") << "] [" << trim("   ") << "]\n";

    auto m = parsiraj("baud=115200; parity=N ;stop=1;;bez_jednako");
    for (const auto& [k, v] : m) std::cout << k << " -> " << v << '\n';

    std::cout << uJson(m) << '\n';
}
