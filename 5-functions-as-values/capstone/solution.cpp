// Rešenje završne vežbe dela 5: sistem događaja.

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Poruka {
    std::string izvor;
    double vrednost;
};

// ---------------------------------------------------------------- korak 1
using Rukovalac = std::function<void(const Poruka&)>;
using Filter = std::function<bool(const Poruka&)>;

class Dispecer {
public:
    using Id = int;

    Id pretplati(const std::string& tema, Rukovalac r) {
        pretplate_.push_back({sledeci_, tema, std::move(r)});
        return sledeci_++;
    }

    bool odjavi(Id id) {
        for (auto it = pretplate_.begin(); it != pretplate_.end(); ++it) {
            if (it->id == id) {
                pretplate_.erase(it);
                return true;
            }
        }
        return false;
    }

    void dodajFilter(Filter f) { filteri_.push_back(std::move(f)); }

    // Vraća koliko je rukovalaca pozvano. Obilazi KOPIJU spiska: rukovalac
    // sme da odjavi sebe ili druge (korak 4), a erase iz vektora koji se
    // upravo obilazi poništio bi iteratore.
    int objavi(const std::string& tema, const Poruka& p) {
        for (const Filter& f : filteri_)
            if (!f(p)) return 0;
        const std::vector<Pretplata> kopija = pretplate_;
        int pozvano = 0;
        for (const Pretplata& s : kopija) {
            if (s.tema == tema && jeAktivna(s.id)) {
                s.r(p);
                ++pozvano;
            }
        }
        return pozvano;
    }

    std::size_t brojPretplata() const { return pretplate_.size(); }

private:
    struct Pretplata {
        Id id;
        std::string tema;
        Rukovalac r;
    };

    // Rukovalac odjavljen u toku iste objave se više ne poziva.
    bool jeAktivna(Id id) const {
        for (const Pretplata& s : pretplate_)
            if (s.id == id) return true;
        return false;
    }

    std::vector<Pretplata> pretplate_;
    std::vector<Filter> filteri_;
    Id sledeci_ = 1;
};

// ---------------------------------------------------------------- korak 2
double linearno(double x, double k, double n) { return k * x + n; }

double primeni(const std::vector<std::function<double(double)>>& lanac, double x) {
    for (const auto& f : lanac) x = f(x);
    return x;
}

// ---------------------------------------------------------------- korak 3
// Pretplaćuje se u konstruktoru, odjavljuje u destruktoru (RAII, lekcija 21):
// lambda hvata this, pa ne sme da nadživi objekat.
class Logger {
public:
    Logger(Dispecer& d, const std::string& tema) : d_(d) {
        id_ = d_.pretplati(tema, [this](const Poruka& p) { zapisi(p); });
    }
    ~Logger() { d_.odjavi(id_); }
    Logger(const Logger&) = delete;              // kopija bi delila isti id i odjavila ga dvaput
    Logger& operator=(const Logger&) = delete;

    int zapisano() const { return zapisano_; }

private:
    void zapisi(const Poruka& p) {
        ++zapisano_;
        std::cout << "  log: " << p.izvor << ' ' << p.vrednost << '\n';
    }

    Dispecer& d_;
    Dispecer::Id id_ = 0;
    int zapisano_ = 0;
};

int main() {
    using namespace std::placeholders;

    std::cout << "== korak 1: pretplata, capture, odjava\n";
    Dispecer d;
    int brojac = 0;
    const auto idBrojaca = d.pretplati("temp", [&brojac](const Poruka&) { ++brojac; });
    std::string prefiks = "  [temp] ";
    const auto idIspisa = d.pretplati("temp", [prefiks](const Poruka& p) { std::cout << prefiks << p.vrednost << '\n'; });
    prefiks = "promenjen ";                          // lambda ima svoju kopiju
    const int pozvano = d.objavi("temp", {"hala", 21.5});   // pre ispisa: rukovaoci i sami pišu na cout
    std::cout << "objava temp: " << pozvano << " rukovaoca\n";
    std::cout << "objava vlaga: " << d.objavi("vlaga", {"hala", 40}) << " rukovaoca\n";
    d.odjavi(idBrojaca);
    d.objavi("temp", {"hala", 22.0});
    std::cout << "brojač " << brojac << " (drugu objavu nije video), pretplata " << d.brojPretplata() << '\n';
    d.odjavi(idIspisa);

    std::cout << "== korak 2: filter i lanac obrade (bind i lambda)\n";
    d.dodajFilter([](const Poruka& p) { return p.vrednost > -50 && p.vrednost < 150; });
    const std::vector<std::function<double(double)>> uFarenhajt{
        std::bind(linearno, _1, 1.8, 32.0),          // isto što i [](double c) { return 1.8 * c + 32; }
        [](double f) { return std::round(f * 10) / 10; },
    };
    const auto idF =
        d.pretplati("temp", [&uFarenhajt](const Poruka& p) { std::cout << "  " << p.vrednost << " C = " << primeni(uFarenhajt, p.vrednost) << " F\n"; });
    d.objavi("temp", {"hala", 21.5});
    std::cout << "objava 999 (filter): " << d.objavi("temp", {"hala", 999}) << " rukovaoca\n";
    d.odjavi(idF);

    std::cout << "== korak 3: logger se odjavljuje u destruktoru\n";
    {
        Logger log(d, "alarm");
        d.objavi("alarm", {"kotao", 91});
        d.objavi("alarm", {"kotao", 95});
        std::cout << "zapisano " << log.zapisano() << ", pretplata " << d.brojPretplata() << '\n';
    }
    std::cout << "posle bloka: pretplata " << d.brojPretplata() << ", objava alarm: " << d.objavi("alarm", {"kotao", 99})
              << " rukovaoca\n";

    std::cout << "== korak 4: rukovalac koji se odjavi tokom objave\n";
    Dispecer::Id idJednom = 0;
    idJednom = d.pretplati("start", [&d, &idJednom](const Poruka& p) {
        std::cout << "  prvi start od " << p.izvor << ", odjavljujem se\n";
        d.odjavi(idJednom);
    });
    d.pretplati("start", [](const Poruka& p) { std::cout << "  start od " << p.izvor << '\n'; });
    d.objavi("start", {"pumpa", 1});
    d.objavi("start", {"ventil", 1});
    std::cout << "pretplata na kraju: " << d.brojPretplata() << '\n';
}
