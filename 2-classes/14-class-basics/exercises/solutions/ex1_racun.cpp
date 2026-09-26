// Rešenje zadatka ex1_racun.

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

class Racun {
public:
    // Korak 1: provera PRE nego što objekat postoji. Ako konstruktor baci,
    // destruktor se ne poziva, pa ni brojač ne sme biti uvećan -- zato
    // ++zivih_ ide na kraj tela.
    Racun(std::string vlasnik, long pocetno) : vlasnik_(std::move(vlasnik)), stanje_(pocetno) {
        if (pocetno < 0) throw std::invalid_argument("početno stanje < 0");
        ++zivih_;
    }
    // Delegirajući: ceo posao radi ciljni konstruktor (i on broji), pa se
    // ovde ne broji ponovo.
    explicit Racun(std::string vlasnik) : Racun(std::move(vlasnik), 0) {}

    ~Racun() { --zivih_; }

    Racun(const Racun&) = delete;
    Racun& operator=(const Racun&) = delete;

    // Korak 2: vraćanje *this po referenci omogućava nizanje poziva.
    Racun& uplati(long iznos) {
        stanje_ += iznos;
        return *this;
    }
    bool isplati(long iznos) {
        if (iznos > stanje_) return false;   // invarijanta: nikad ispod 0
        stanje_ -= iznos;
        return true;
    }
    long stanje() const { return stanje_; }
    const std::string& vlasnik() const { return vlasnik_; }

    // Korak 3: static funkcija nema this -- vidi samo static članove.
    static int brojZivih() { return zivih_; }

private:
    std::string vlasnik_;
    long stanje_;
    static inline int zivih_ = 0;   // C++17: inline static, bez definicije van klase
};

int main() {
    Racun a("Ana", 1000);
    a.uplati(100).uplati(50);
    bool ok = a.isplati(2000);
    std::cout << a.vlasnik() << ": " << a.stanje() << ", isplata 2000: " << ok << '\n';
    try {
        Racun los("Loš", -5);
    } catch (const std::invalid_argument& e) {
        std::cout << "odbijeno: " << e.what() << '\n';
    }

    std::cout << "živih: " << Racun::brojZivih() << '\n';
    {
        Racun b("Bora");
        std::cout << "živih u bloku: " << Racun::brojZivih() << ", Bora: " << b.stanje() << '\n';
    }
    std::cout << "živih posle bloka: " << Racun::brojZivih() << '\n';
}
