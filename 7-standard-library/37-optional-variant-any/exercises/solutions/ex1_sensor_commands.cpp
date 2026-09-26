// Rešenje zadatka ex1_sensor_commands.

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

// Korak 1: >> mora da uspe i da ne ostane ništa posle broja ("2x" nije broj).
std::optional<double> readNumber(const std::string& s) {
    std::istringstream in(s);
    double v = 0;
    char extra = 0;
    if (!(in >> v) || (in >> extra)) return std::nullopt;
    return v;
}

// Korak 2: svaki oblik reda daje drugu alternativu; sve ostalo je nullopt.
std::optional<Command> parseCommand(const std::string& line) {
    std::istringstream in(line);
    std::string word, channel, number;
    in >> word;
    if (word == "reset") return Reset{};
    if (word == "get" && in >> channel) return Get{channel};
    if (word == "set" && in >> channel >> number) {
        if (auto v = readNumber(number)) return Set{channel, *v};
    }
    return std::nullopt;
}

// Korak 3: visit -- ako se doda nova komanda, a ovde ne, ne kompajlira se
// (errors/e03). Sve grane moraju da vrate ISTI tip: "ok" je const char*,
// pa -> std::string na svakoj lambdi (errors/e08).
std::string execute(std::map<std::string, double>& state, const Command& k) {
    return std::visit(Overloaded{
                          [&](const Set& p) -> std::string {
                              state[p.channel] = p.value;
                              return "ok";
                          },
                          [&](const Get& p) -> std::string {
                              auto it = state.find(p.channel);
                              if (it == state.end()) return p.channel + ": missing";
                              std::ostringstream out;
                              out << p.channel << " = " << it->second;
                              return out.str();
                          },
                          [&](const Reset&) -> std::string {
                              state.clear();
                              return "cleared";
                          },
                      },
                      k);
}

int main() {
    for (const char* s : {"21.5", "2x", ""}) {
        auto v = readNumber(s);
        std::cout << '"' << s << "\" -> " << (v ? std::to_string(*v).substr(0, 4) : "not a number") << '\n';
    }

    std::map<std::string, double> state;
    std::vector<std::string> lines{"set temp 21.5", "get temp", "get humidity", "set humidity x", "reset", "get temp", "jump"};
    for (const auto& line : lines) {
        std::cout << line << " => ";
        if (auto k = parseCommand(line))
            std::cout << execute(state, *k) << '\n';
        else
            std::cout << "invalid command\n";
    }
}
