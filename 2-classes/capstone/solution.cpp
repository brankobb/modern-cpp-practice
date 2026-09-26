// Rešenje završne vežbe dela 2: temperature i senzori.

#include <cassert>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// ---------------------------------------------------------------- korak 1
// Razlika dve temperature (u K, isto što i u °C). Poseban tip: 20 °C + 5 °C
// nema smisla, 20 °C + 5 K ima.
class Razlika {
public:
    explicit Razlika(double kelvina) : k_(kelvina) {}   // explicit: 2.0 nije Razlika sam od sebe
    double kelvina() const { return k_; }

    friend Razlika operator+(Razlika a, Razlika b) { return Razlika{a.k_ + b.k_}; }
    friend bool operator==(Razlika a, Razlika b) { return a.k_ == b.k_; }
    friend bool operator!=(Razlika a, Razlika b) { return !(a == b); }
    // Formatiranje u lokalni stream: operator<< ne sme trajno da promeni
    // podešavanja tuđeg stream-a (fixed, precision, showpos).
    friend std::ostream& operator<<(std::ostream& out, Razlika r) {
        std::ostringstream s;
        s << std::showpos << std::fixed << std::setprecision(1) << r.k_ << " K";
        return out << s.str();
    }

private:
    double k_;
};

// Invarijanta: nikad ispod apsolutne nule. Konstruktor je privatan; prave je
// samo fabričke funkcije, koje invarijantu proveravaju.
class Temperatura {
public:
    static constexpr double kApsolutnaNula = -273.15;

    static Temperatura izCelzijusa(double c) { return Temperatura{c}; }
    static Temperatura izKelvina(double k) { return Temperatura{k + kApsolutnaNula}; }

    double celzijus() const { return c_; }
    double kelvin() const { return c_ - kApsolutnaNula; }

    Temperatura& operator+=(Razlika r) {
        c_ += r.kelvina();
        assert(c_ >= kApsolutnaNula && "ispod apsolutne nule");
        return *this;
    }
    friend Temperatura operator+(Temperatura t, Razlika r) { return t += r; }
    friend Razlika operator-(Temperatura a, Temperatura b) { return Razlika{a.c_ - b.c_}; }

    friend bool operator==(Temperatura a, Temperatura b) { return a.c_ == b.c_; }
    friend bool operator!=(Temperatura a, Temperatura b) { return !(a == b); }
    friend bool operator<(Temperatura a, Temperatura b) { return a.c_ < b.c_; }
    friend bool operator>(Temperatura a, Temperatura b) { return b < a; }

    friend std::ostream& operator<<(std::ostream& out, Temperatura t) {
        std::ostringstream s;
        s << std::fixed << std::setprecision(1) << t.c_ << " °C";
        return out << s.str();
    }

private:
    explicit Temperatura(double c) : c_(c) { assert(c_ >= kApsolutnaNula && "ispod apsolutne nule"); }
    double c_;
};

// ---------------------------------------------------------------- korak 2
class Senzor {
public:
    virtual ~Senzor() { --zivih_; }                  // virtual: brisanje preko Senzor* je ispravno
    Senzor(const Senzor&) = delete;                  // polimorfni objekat se ne kopira (slicing)
    Senzor& operator=(const Senzor&) = delete;

    const std::string& ime() const { return ime_; }
    virtual Temperatura citaj() = 0;
    virtual std::string opis() const { return "senzor '" + ime_ + "'"; }

    static int zivih() { return zivih_; }

protected:
    explicit Senzor(const std::string& ime) : ime_(ime) { ++zivih_; }

private:
    std::string ime_;
    static inline int zivih_ = 0;                    // C++17 inline static član
};

class SimuliraniSenzor : public Senzor {
public:
    SimuliraniSenzor(const std::string& ime, const std::vector<double>& celzijusi)
        : Senzor(ime), vrednosti_(celzijusi) {
        assert(!vrednosti_.empty());
    }
    Temperatura citaj() override {
        const double c = vrednosti_[sledeca_];
        sledeca_ = (sledeca_ + 1) % vrednosti_.size();
        return Temperatura::izCelzijusa(c);
    }
    std::string opis() const override {
        return Senzor::opis() + ", simuliran, " + std::to_string(vrednosti_.size()) + " vrednosti";
    }

private:
    std::vector<double> vrednosti_;
    std::size_t sledeca_ = 0;
};

class KalibrisaniSenzor final : public SimuliraniSenzor {
public:
    KalibrisaniSenzor(const std::string& ime, const std::vector<double>& celzijusi, Razlika pomak)
        : SimuliraniSenzor(ime, celzijusi), pomak_(pomak) {}
    Temperatura citaj() override { return SimuliraniSenzor::citaj() + pomak_; }
    std::string opis() const override { return SimuliraniSenzor::opis() + ", kalibrisan"; }
    void kalibrisi(Razlika dodatno) { pomak_ = pomak_ + dodatno; }
    Razlika pomak() const { return pomak_; }

private:
    Razlika pomak_;
};

// ---------------------------------------------------------------- korak 3
// Samo senzori koji umeju da se kalibrišu; ostali se preskaču.
int kalibrisiSve(const std::vector<Senzor*>& senzori, Razlika dodatno) {
    int n = 0;
    for (Senzor* s : senzori) {
        if (auto* k = dynamic_cast<KalibrisaniSenzor*>(s)) {
            k->kalibrisi(dodatno);
            ++n;
        }
    }
    return n;
}

// ---------------------------------------------------------------- korak 4
class IznadPraga {
public:
    explicit IznadPraga(Temperatura prag) : prag_(prag) {}
    bool operator()(Temperatura t) const { return t > prag_; }

private:
    Temperatura prag_;
};

void nadzor(const std::vector<Senzor*>& senzori, int ciklusa, const IznadPraga& alarm) {
    for (Senzor* s : senzori) {
        const Temperatura prva = s->citaj();
        Temperatura najvisa = prva;
        int alarma = alarm(prva) ? 1 : 0;
        for (int i = 1; i < ciklusa; ++i) {
            const Temperatura t = s->citaj();
            if (t > najvisa) najvisa = t;
            if (alarm(t)) ++alarma;
        }
        std::cout << std::left << std::setw(8) << s->ime() << std::right << " najviša " << najvisa << ", porast od prve "
                  << (najvisa - prva) << ", alarma " << alarma << '\n';
    }
}

int main() {
    std::cout << "== korak 1: temperatura i razlika\n";
    const Temperatura t = Temperatura::izCelzijusa(21.5);
    const Temperatura t2 = t + Razlika{2.0};
    std::cout << t << " + 2 K = " << t2 << "; razlika " << (t2 - t) << "; kelvin " << t2.kelvin() << '\n';
    std::cout << std::boolalpha << "t < t2: " << (t < t2) << ", t == 21.5 °C: " << (t == Temperatura::izCelzijusa(21.5))
              << ", 273.15 K = " << Temperatura::izKelvina(273.15) << '\n';

    std::cout << "== korak 2: hijerarhija senzora\n";
    SimuliraniSenzor hala("hala", {21.5, 22.0, 22.5});
    KalibrisaniSenzor kotao("kotao", {78.0, 81.0, 84.0}, Razlika{-1.5});
    SimuliraniSenzor napolje("napolje", {-3.0, -1.0});
    const std::vector<Senzor*> svi{&hala, &kotao, &napolje};   // ne poseduje: objekti su na steku
    for (const Senzor* s : svi) std::cout << s->opis() << '\n';
    std::cout << "živih senzora: " << Senzor::zivih() << '\n';

    std::cout << "== korak 3: kalibracija preko dynamic_cast\n";
    std::cout << "kalibrisano: " << kalibrisiSve(svi, Razlika{0.5}) << ", pomak kotla sada " << kotao.pomak() << '\n';

    std::cout << "== korak 4: nadzor, prag 80 °C\n";
    nadzor(svi, 3, IznadPraga{Temperatura::izCelzijusa(80.0)});
}
