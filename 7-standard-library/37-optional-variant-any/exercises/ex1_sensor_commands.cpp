// KIND: usage
//
// Zadatak 1 -- optional za "možda uspe", variant za komande, visit za
// izvršavanje (sekcije 1, 4, 5)
//   ./build.sh 7-standard-library/37-optional-variant-any/exercises/ex1_sensor_commands.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_sensor_commands.cpp
//
// Korak 1: std::optional<double> readNumber(const std::string& s)
//   -- std::istringstream, >> u double; nullopt ako čitanje ne uspe ILI
//   posle broja ostane još nešto ("2x").
// Korak 2: std::optional<Command> parseCommand(const std::string& line)
//   -- "reset" -> Reset{}; "get channel" -> Get{channel};
//   "set channel number" -> Set{channel, number} (number preko readNumber);
//   sve ostalo -> nullopt.
// Korak 3: std::string execute(std::map<std::string, double>& state, const Command& k)
//   -- std::visit sa Overloaded{...}: Set upiše i vrati "ok";
//   Get vrati "channel = value" ili "channel: missing"; Reset obriše sve
//   i vrati "obrisano". (Šta se desi ako dodaš četvrtu komandu u
//   Command, a ovde je zaboraviš?)

#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

struct Set {
    std::string channel;
    double value;
};
struct Get {
    std::string channel;
};
struct Reset {};
using Command = std::variant<Set, Get, Reset>;

template <typename... F>
struct Overloaded : F... {
    using F::operator()...;
};
template <typename... F>
Overloaded(F...) -> Overloaded<F...>;

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // for (const char* s : {"21.5", "2x", ""}) {
    //     auto v = readNumber(s);
    //     std::cout << '"' << s << "\" -> " << (v ? std::to_string(*v).substr(0, 4) : "not a number") << '\n';
    // }

    // Korak 2 i 3 -- otkomentariši:
    // std::map<std::string, double> state;
    // std::vector<std::string> lines{"set temp 21.5", "get temp", "get humidity", "set humidity x", "reset", "get temp", "jump"};
    // for (const auto& line : lines) {
    //     std::cout << line << " => ";
    //     if (auto k = parseCommand(line))
    //         std::cout << execute(state, *k) << '\n';
    //     else
    //         std::cout << "invalid command\n";
    // }
}

/* EXPECTED OUTPUT
"21.5" -> 21.5
"2x" -> not a number
"" -> not a number
set temp 21.5 => ok
get temp => temp = 21.5
get humidity => humidity: missing
set humidity x => invalid command
reset => cleared
get temp => temp: missing
jump => invalid command
*/
