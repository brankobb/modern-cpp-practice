// Rešenje zadatka ex1_fraction.

#include <iostream>
#include <numeric>
#include <stdexcept>

class Fraction {
public:
    // Korak 1: normalizovan oblik je invarijanta -- zato == može da poredi
    // članove direktno, a ispis nikad ne pokaže 2/4 ili 1/-2.
    Fraction(long n, long d = 1) : num_(n), den_(d) {
        if (d == 0) throw std::invalid_argument("denominator 0");
        if (den_ < 0) {
            num_ = -num_;
            den_ = -den_;
        }
        long g = std::gcd(num_, den_);   // gcd(0, d) = d, pa 0 postane 0/1
        num_ /= g;
        den_ /= g;
    }

    long num() const { return num_; }
    long den() const { return den_; }

    // Korak 2: složena dodela je član -- menja levu stranu.
    Fraction& operator+=(const Fraction& o) {
        *this = Fraction(num_ * o.den_ + o.num_ * den_, den_ * o.den_);
        return *this;
    }
    Fraction& operator*=(const Fraction& o) {
        *this = Fraction(num_ * o.num_, den_ * o.den_);
        return *this;
    }
    Fraction operator-() const { return Fraction(-num_, den_); }

private:
    long num_;
    long den_;
};

// Korak 2: kao član, a + b bi bilo a.operator+(b) -- leva strana MORA biti
// Fraction, pa 2 + a ne bi radilo (errors/e03). Slobodna funkcija dozvoli
// konverziju i levog operanda. Prvi parametar po vrednosti: to je kopija
// koju ionako menjamo.
Fraction operator+(Fraction a, const Fraction& b) { return a += b; }
Fraction operator*(Fraction a, const Fraction& b) { return a *= b; }

// Korak 3
bool operator==(const Fraction& a, const Fraction& b) {
    return a.num() == b.num() && a.den() == b.den();
}
bool operator<(const Fraction& a, const Fraction& b) {
    return a.num() * b.den() < b.num() * a.den();   // imenioci > 0
}

std::ostream& operator<<(std::ostream& os, const Fraction& f) {
    os << f.num();
    if (f.den() != 1) os << '/' << f.den();
    return os;
}

int main() {
    Fraction a(1, 2), b(3, 4);
    std::cout << "a + b = " << a + b << '\n';
    std::cout << "2 + a = " << 2 + a << '\n';
    std::cout << "a * b = " << a * b << '\n';
    std::cout << "-a = " << -a << '\n';
    std::cout << "Fraction(2, 4) == a: " << (Fraction(2, 4) == a) << '\n';
    std::cout << "a < b: " << (a < b) << '\n';
    std::cout << "Fraction(6, -3) = " << Fraction(6, -3) << '\n';
    try {
        Fraction bad(1, 0);
    } catch (const std::invalid_argument&) {
        std::cout << "denominator 0: rejected\n";
    }
}
