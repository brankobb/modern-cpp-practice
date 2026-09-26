// Rešenje zadatka ex1_config_parsing.

#include <iostream>
#include <map>
#include <sstream>
#include <string>

// Korak 1: npos znači "nije nađeno" -- string od samih razmaka.
std::string trim(const std::string& s) {
    const char* spaces = " \t";
    auto first = s.find_first_not_of(spaces);
    if (first == std::string::npos) return "";
    auto last = s.find_last_not_of(spaces);
    return s.substr(first, last - first + 1);
}

// Korak 2: getline sa graničnikom je najjednostavniji "split".
std::map<std::string, std::string> parseConfig(const std::string& input) {
    std::map<std::string, std::string> m;
    std::istringstream is(input);
    std::string part;
    while (std::getline(is, part, ';')) {
        part = trim(part);
        auto eq = part.find('=');
        if (part.empty() || eq == std::string::npos) continue;
        m[trim(part.substr(0, eq))] = trim(part.substr(eq + 1));
    }
    return m;
}

// Korak 3: R"(...)" -- sve između zagrada je doslovno, i navodnici.
std::string toJson(const std::map<std::string, std::string>& m) {
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

    auto m = parseConfig("baud=115200; parity=N ;stop=1;;no_equals");
    for (const auto& [k, v] : m) std::cout << k << " -> " << v << '\n';

    std::cout << toJson(m) << '\n';
}
