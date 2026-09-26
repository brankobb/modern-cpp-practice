// Rešenje zadatka z1_oblici.

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Korak 1: interfejs = apstraktna klasa (bar jedna čista virtualna
// funkcija). Virtualni destruktor: objekti se brišu preko Oblik* (unique_ptr
// u vektoru), pa mora da se pozove destruktor IZVEDENE klase (ub/u01).
class Oblik {
public:
    virtual ~Oblik() = default;
    virtual double povrsina() const = 0;
    virtual std::string ime() const = 0;
    // Korak 3: "virtuelni konstruktor kopije" (C.130).
    virtual std::unique_ptr<Oblik> clone() const = 0;

    Oblik& operator=(const Oblik&) = delete;

protected:
    // Korak 3: protected -- izvedene klase mogu da se kopiraju (za clone),
    // ali "Oblik o = krug;" (slicing) spolja ne prolazi.
    Oblik() = default;
    Oblik(const Oblik&) = default;
};

// Korak 2: override -- kompajler proveri da zaista nadjačava (zadatak z2).
class Krug : public Oblik {
public:
    explicit Krug(double r) : r_(r) {}
    double povrsina() const override { return 3.14159 * r_ * r_; }
    std::string ime() const override { return "krug"; }
    std::unique_ptr<Oblik> clone() const override { return std::make_unique<Krug>(*this); }

private:
    double r_;
};

class Pravougaonik : public Oblik {
public:
    Pravougaonik(double a, double b) : a_(a), b_(b) {}
    double povrsina() const override { return a_ * b_; }
    std::string ime() const override { return "pravougaonik"; }
    std::unique_ptr<Oblik> clone() const override { return std::make_unique<Pravougaonik>(*this); }

private:
    double a_;
    double b_;
};

// final: od Kvadrata se dalje ne nasleđuje, a pozivi preko Kvadrat& mogu da
// se devirtualizuju (sekcija 7).
class Kvadrat final : public Pravougaonik {
public:
    explicit Kvadrat(double a) : Pravougaonik(a, a) {}
    std::string ime() const override { return "kvadrat"; }
    std::unique_ptr<Oblik> clone() const override { return std::make_unique<Kvadrat>(*this); }
};

int main() {
    std::vector<std::unique_ptr<Oblik>> oblici;
    oblici.push_back(std::make_unique<Krug>(1.0));
    oblici.push_back(std::make_unique<Pravougaonik>(2.0, 3.0));
    oblici.push_back(std::make_unique<Kvadrat>(4.0));
    double ukupno = 0;
    for (const auto& o : oblici) {
        std::cout << o->ime() << ": " << o->povrsina() << '\n';
        ukupno += o->povrsina();
    }
    std::cout << "ukupno: " << ukupno << '\n';

    std::unique_ptr<Oblik> kopija = oblici[2]->clone();
    std::cout << "kopija: " << kopija->ime() << ' ' << kopija->povrsina() << '\n';
}
