// KIND: usage
//
// Zadatak 1 -- std::string, getline sa graničnikom, string streams i raw
// string (sekcije 2, 3, 5)
//   ./build.sh 1-language-basics/06-strings/exercises/ex1_config_parsing.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_config_parsing.cpp
//
// Ulaz: "baud=115200; parity=N ;stop=1;;" (razmaci i prazni delovi su
// namerni).
// Korak 1: std::string trim(const std::string& s) -- skine razmake sa
//   početka i kraja (find_first_not_of / find_last_not_of, substr). Za
//   string od samih razmaka vrati "".
// Korak 2: std::map<std::string, std::string> parseConfig(const std::string& input)
//   -- std::istringstream + std::getline(is, part, ';') daje delove; svaki
//   neprazan (posle trim) podeli na prvom '=' (find, substr) u ključ i
//   vrednost, oba trim-ovana. Deo bez '=' preskoči.
// Korak 3: std::string toJson(const std::map<...>& m) -- std::ostringstream,
//   format {"key":"value",...}. Navodnike piši kroz raw string
//   literal R"(...)" umesto \" (sekcija 2).

#include <iostream>
#include <map>
#include <sstream>
#include <string>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << '[' << trim("  a b  ") << "] [" << trim("   ") << "]\n";

    // Korak 2 -- otkomentariši:
    // auto m = parseConfig("baud=115200; parity=N ;stop=1;;no_equals");
    // for (const auto& [k, v] : m) std::cout << k << " -> " << v << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << toJson(m) << '\n';
}

/* EXPECTED OUTPUT
[a b] []
baud -> 115200
parity -> N
stop -> 1
{"baud":"115200","parity":"N","stop":"1"}
*/
