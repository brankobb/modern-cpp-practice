// Rešenje zadatka z1_komande_senzora.

#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

struct Postavi {
    std::string kanal;
    double vrednost;
};
struct Procitaj {
    std::string kanal;
};
struct Reset {};
using Komanda = std::variant<Postavi, Procitaj, Reset>;

template <typename... F>
struct Preopterecen : F... {
    using F::operator()...;
};
template <typename... F>
Preopterecen(F...) -> Preopterecen<F...>;

// Korak 1: >> mora da uspe i da ne ostane ništa posle broja ("2x" nije broj).
std::optional<double> procitajBroj(const std::string& s) {
    std::istringstream in(s);
    double v = 0;
    char visak = 0;
    if (!(in >> v) || (in >> visak)) return std::nullopt;
    return v;
}

// Korak 2: svaki oblik reda daje drugu alternativu; sve ostalo je nullopt.
std::optional<Komanda> parsirajKomandu(const std::string& red) {
    std::istringstream in(red);
    std::string rec, kanal, broj;
    in >> rec;
    if (rec == "reset") return Reset{};
    if (rec == "get" && in >> kanal) return Procitaj{kanal};
    if (rec == "set" && in >> kanal >> broj) {
        if (auto v = procitajBroj(broj)) return Postavi{kanal, *v};
    }
    return std::nullopt;
}

// Korak 3: visit -- ako se doda nova komanda, a ovde ne, ne kompajlira se
// (errors/e03). Sve grane moraju da vrate ISTI tip: "ok" je const char*,
// pa -> std::string na svakoj lambdi (errors/e08).
std::string izvrsi(std::map<std::string, double>& stanje, const Komanda& k) {
    return std::visit(Preopterecen{
                          [&](const Postavi& p) -> std::string {
                              stanje[p.kanal] = p.vrednost;
                              return "ok";
                          },
                          [&](const Procitaj& p) -> std::string {
                              auto it = stanje.find(p.kanal);
                              if (it == stanje.end()) return p.kanal + ": nema";
                              std::ostringstream out;
                              out << p.kanal << " = " << it->second;
                              return out.str();
                          },
                          [&](const Reset&) -> std::string {
                              stanje.clear();
                              return "obrisano";
                          },
                      },
                      k);
}

int main() {
    for (const char* s : {"21.5", "2x", ""}) {
        auto v = procitajBroj(s);
        std::cout << '"' << s << "\" -> " << (v ? std::to_string(*v).substr(0, 4) : "nije broj") << '\n';
    }

    std::map<std::string, double> stanje;
    std::vector<std::string> redovi{"set temp 21.5", "get temp", "get vlaga", "set vlaga x", "reset", "get temp", "skok"};
    for (const auto& red : redovi) {
        std::cout << red << " => ";
        if (auto k = parsirajKomandu(red))
            std::cout << izvrsi(stanje, *k) << '\n';
        else
            std::cout << "neispravna komanda\n";
    }
}
