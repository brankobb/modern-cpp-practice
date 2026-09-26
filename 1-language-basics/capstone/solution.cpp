// Rešenje završne vežbe dela 1: izveštaj o merenjima.

#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace merenja {

struct Kanal {
    std::string ime;
    double min = 0;
    double max = 0;
    std::string jedinica;
};

struct Merenje {
    std::size_t kanal;   // indeks u vektoru kanala
    double vrednost;
};

enum class Greska { NepoznatKanal, NijeBroj, VanOpsega, LosFormat };

// Jedna tabela za sve nazive; static_assert čuva da prati enum.
constexpr std::array<const char*, 4> kNaziviGresaka{"nepoznat kanal", "nije broj", "van opsega", "loš format"};
static_assert(kNaziviGresaka.size() == static_cast<std::size_t>(Greska::LosFormat) + 1,
              "tabela naziva mora da prati enum Greska");

constexpr const char* naziv(Greska g) { return kNaziviGresaka[static_cast<std::size_t>(g)]; }

struct Podaci {
    std::vector<Kanal> kanali;
    std::vector<Merenje> merenja;
    std::array<int, kNaziviGresaka.size()> greske{};   // brojač po vrsti greške
    int redovaMerenja = 0;
    int losihRedovaKonfiguracije = 0;
};

// ---------------------------------------------------------------- korak 1
// Ceo tekst mora da bude broj: "21.5" da, "2x" i "" ne.
bool procitajBroj(const std::string& tekst, double& izlaz) {
    std::istringstream in(tekst);
    char visak = 0;
    return (in >> izlaz) && !(in >> visak);
}

// "kanal temp min=-20 max=60 jedinica=C"
bool parsirajKanal(const std::string& red, Kanal& k) {
    std::istringstream in(red);
    std::string rec;
    if (!(in >> rec) || rec != "kanal" || !(in >> k.ime)) return false;
    bool imaMin = false, imaMax = false;
    while (in >> rec) {
        const auto jednako = rec.find('=');
        if (jednako == std::string::npos) return false;
        const std::string kljuc = rec.substr(0, jednako);
        const std::string vrednost = rec.substr(jednako + 1);
        if (kljuc == "min")
            imaMin = procitajBroj(vrednost, k.min);
        else if (kljuc == "max")
            imaMax = procitajBroj(vrednost, k.max);
        else if (kljuc == "jedinica")
            k.jedinica = vrednost;
        else
            return false;
    }
    return imaMin && imaMax && k.min < k.max;
}

// ---------------------------------------------------------------- korak 2
int nadjiKanal(const std::vector<Kanal>& kanali, const std::string& ime) {
    for (std::size_t i = 0; i < kanali.size(); ++i)
        if (kanali[i].ime == ime) return static_cast<int>(i);
    return -1;
}

void zabeleziGresku(Podaci& p, Greska g) { ++p.greske[static_cast<std::size_t>(g)]; }

void obradiMerenje(Podaci& p, const std::string& red) {
    ++p.redovaMerenja;
    std::istringstream in(red);
    std::string ime, tekst, visak;
    if (!(in >> ime >> tekst) || (in >> visak)) return zabeleziGresku(p, Greska::LosFormat);
    const int k = nadjiKanal(p.kanali, ime);
    if (k < 0) return zabeleziGresku(p, Greska::NepoznatKanal);
    double v = 0;
    if (!procitajBroj(tekst, v)) return zabeleziGresku(p, Greska::NijeBroj);
    const Kanal& kanal = p.kanali[static_cast<std::size_t>(k)];
    if (v < kanal.min || v > kanal.max) return zabeleziGresku(p, Greska::VanOpsega);
    p.merenja.push_back({static_cast<std::size_t>(k), v});
}

Podaci ucitaj(std::istream& ulaz) {
    Podaci p;
    std::string red;
    while (std::getline(ulaz, red)) {
        if (red.empty() || red[0] == '#') continue;
        if (red.compare(0, 6, "kanal ") == 0) {
            Kanal k;
            if (parsirajKanal(red, k))
                p.kanali.push_back(k);
            else
                ++p.losihRedovaKonfiguracije;
        } else {
            obradiMerenje(p, red);
        }
    }
    return p;
}

// ---------------------------------------------------------------- korak 3
struct Statistika {
    int n = 0;
    double min = 0, max = 0, zbir = 0;
};

std::vector<Statistika> statistika(const Podaci& p) {
    std::vector<Statistika> s(p.kanali.size());
    for (const auto& [kanal, v] : p.merenja) {
        Statistika& st = s[kanal];
        if (st.n == 0 || v < st.min) st.min = v;
        if (st.n == 0 || v > st.max) st.max = v;
        st.zbir += v;
        ++st.n;
    }
    return s;
}

// ---------------------------------------------------------------- korak 4
// Overload: isti posao ("kolona širine w"), različit tip.
void kolona(std::ostream& out, double v, int w) { out << std::setw(w) << std::fixed << std::setprecision(1) << v; }
void kolona(std::ostream& out, const std::string& s, int w) { out << std::setw(w) << s; }

void ispisiIzvestaj(std::ostream& out, const Podaci& p) {
    const int ispravnih = static_cast<int>(p.merenja.size());
    out << "kanala: " << p.kanali.size() << " (neispravnih redova konfiguracije: " << p.losihRedovaKonfiguracije
        << ")\n";
    out << "merenja: " << ispravnih << " ispravnih od " << p.redovaMerenja << '\n';
    out << "greške:";
    const char* sep = " ";
    for (std::size_t i = 0; i < p.greske.size(); ++i) {
        out << sep << naziv(static_cast<Greska>(i)) << ' ' << p.greske[i];
        sep = ", ";
    }
    out << "\n\n" << std::left << std::setw(10) << "kanal" << std::right << std::setw(3) << "n" << std::setw(10)
        << "min" << std::setw(10) << "max" << std::setw(10) << "prosek" << '\n';
    const auto st = statistika(p);
    for (std::size_t i = 0; i < p.kanali.size(); ++i) {
        const Kanal& k = p.kanali[i];
        out << std::left << std::setw(10) << k.ime << std::right << std::setw(3) << st[i].n;
        if (st[i].n > 0) {
            kolona(out, st[i].min, 10);
            kolona(out, st[i].max, 10);
            kolona(out, st[i].zbir / st[i].n, 10);
        } else {
            for (int j = 0; j < 3; ++j) kolona(out, "-", 10);
        }
        out << ' ' << k.jedinica << '\n';
    }
}

}  // namespace merenja

const char* const kUlaz = R"(# konfiguracija
kanal temp min=-20 max=60 jedinica=C
kanal vlaga min=0 max=100 jedinica=%
kanal pritisak min=900 max=1100 jedinica=hPa
kanal struja min=0 max=10 jedinica=A
kanal los min=5

# merenja
temp 21.5
vlaga 40
temp 85
pritisak 1013.2
vlaga abc
napon 12
temp 22
pritisak
vlaga 55
temp 21.8
)";

int main() {
    std::istringstream ulaz(kUlaz);
    const merenja::Podaci p = merenja::ucitaj(ulaz);
    merenja::ispisiIzvestaj(std::cout, p);
}
