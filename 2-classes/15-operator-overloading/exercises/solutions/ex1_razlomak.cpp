// Rešenje zadatka ex1_razlomak.

#include <iostream>
#include <numeric>
#include <stdexcept>

class Razlomak {
public:
    // Korak 1: normalizovan oblik je invarijanta -- zato == može da poredi
    // članove direktno, a ispis nikad ne pokaže 2/4 ili 1/-2.
    Razlomak(long b, long i = 1) : brojilac_(b), imenilac_(i) {
        if (i == 0) throw std::invalid_argument("imenilac 0");
        if (imenilac_ < 0) {
            brojilac_ = -brojilac_;
            imenilac_ = -imenilac_;
        }
        long g = std::gcd(brojilac_, imenilac_);   // gcd(0, i) = i, pa 0 postane 0/1
        brojilac_ /= g;
        imenilac_ /= g;
    }

    long brojilac() const { return brojilac_; }
    long imenilac() const { return imenilac_; }

    // Korak 2: složena dodela je član -- menja levu stranu.
    Razlomak& operator+=(const Razlomak& o) {
        *this = Razlomak(brojilac_ * o.imenilac_ + o.brojilac_ * imenilac_, imenilac_ * o.imenilac_);
        return *this;
    }
    Razlomak& operator*=(const Razlomak& o) {
        *this = Razlomak(brojilac_ * o.brojilac_, imenilac_ * o.imenilac_);
        return *this;
    }
    Razlomak operator-() const { return Razlomak(-brojilac_, imenilac_); }

private:
    long brojilac_;
    long imenilac_;
};

// Korak 2: kao član, a + b bi bilo a.operator+(b) -- leva strana MORA biti
// Razlomak, pa 2 + a ne bi radilo (errors/e03). Slobodna funkcija dozvoli
// konverziju i levog operanda. Prvi parametar po vrednosti: to je kopija
// koju ionako menjamo.
Razlomak operator+(Razlomak a, const Razlomak& b) { return a += b; }
Razlomak operator*(Razlomak a, const Razlomak& b) { return a *= b; }

// Korak 3
bool operator==(const Razlomak& a, const Razlomak& b) {
    return a.brojilac() == b.brojilac() && a.imenilac() == b.imenilac();
}
bool operator<(const Razlomak& a, const Razlomak& b) {
    return a.brojilac() * b.imenilac() < b.brojilac() * a.imenilac();   // imenioci > 0
}

std::ostream& operator<<(std::ostream& os, const Razlomak& r) {
    os << r.brojilac();
    if (r.imenilac() != 1) os << '/' << r.imenilac();
    return os;
}

int main() {
    Razlomak a(1, 2), b(3, 4);
    std::cout << "a + b = " << a + b << '\n';
    std::cout << "2 + a = " << 2 + a << '\n';
    std::cout << "a * b = " << a * b << '\n';
    std::cout << "-a = " << -a << '\n';
    std::cout << "Razlomak(2, 4) == a: " << (Razlomak(2, 4) == a) << '\n';
    std::cout << "a < b: " << (a < b) << '\n';
    std::cout << "Razlomak(6, -3) = " << Razlomak(6, -3) << '\n';
    try {
        Razlomak los(1, 0);
    } catch (const std::invalid_argument&) {
        std::cout << "imenilac 0: odbijeno\n";
    }
}
