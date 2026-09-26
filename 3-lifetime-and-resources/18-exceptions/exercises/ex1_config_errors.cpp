// KIND: usage
//
// Zadatak 1 -- sopstvena klasa izuzetka, prevođenje izuzetaka i lanac
// uzroka (sekcije 2, 3, 6)
//   ./build.sh 3-lifetime-and-resources/18-exceptions/exercises/ex1_config_errors.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_config_errors.cpp
//
// Konfiguracija je niz redova oblika "key=number".
// Korak 1: class ConfigError nasleđuje std::runtime_error i pamti broj
//   reda: ConfigError(int lineNumber, const std::string& message), what()
//   vraća "line <broj>: <poruka>", a int line() const noexcept.
// Korak 2: int readValue(const std::string& text, int lineNumber) --
//   ako nema '=', baci ConfigError(lineNumber, "missing '='"). Broj posle
//   '=' pročitaj sa std::stoi. stoi baca std::invalid_argument ili
//   std::out_of_range -- uhvati ih (obe su std::logic_error) i PREVEDI u
//   ConfigError(lineNumber, "value is not a number: '<tekst>'"). Pozivalac
//   ne treba da zna da unutra radi stoi.
// Korak 3: int load(const std::vector<std::string>& lines) sabira sve
//   vrednosti. Svaku grešku zamota sa
//   std::throw_with_nested(std::runtime_error("configuration not loaded")),
//   a u main-u ispiši lanac (std::rethrow_if_nested, kao u main.cpp
//   sekcija 6), uvučeno po 2 razmaka po nivou.

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// TODO korak 1, 2, 3

// void printChain(const std::exception& e, int level = 0) { ... }

int main() {
    // Korak 1-3 -- otkomentariši:
    // std::cout << "sum: " << load({"a=10", "b=20", "c=30"}) << '\n';
    // for (const std::vector<std::string>& config :
    //      {std::vector<std::string>{"a=10", "b=abc"}, std::vector<std::string>{"a=10", "b 20"}}) {
    //     try {
    //         load(config);
    //     } catch (const std::exception& e) {
    //         printChain(e);
    //     }
    // }
}

/* EXPECTED OUTPUT
sum: 60
configuration not loaded
  line 2: value is not a number: 'abc'
configuration not loaded
  line 2: missing '='
*/
