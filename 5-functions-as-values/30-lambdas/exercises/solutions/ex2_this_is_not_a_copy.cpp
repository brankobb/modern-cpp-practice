// Rešenje zadatka ex2_this_is_not_a_copy.

#include <iostream>

class Thermostat {
public:
    explicit Thermostat(int threshold) : threshold_(threshold) {}
    void setThreshold(int p) { threshold_ = p; }
    int threshold() const { return threshold_; }

    // Ako u metodi pišeš [=] i koristiš član (nije dobro): zarobljen je
    // this, pa lambda čita TRENUTNI član -- i visi ako objekat nestane.
    // Treba ovako: kopiraj tačno ono što treba, init capture-om.
    auto makeCheck() const {
        return [threshold = threshold_](int t) { return t < threshold; };
    }
    // Možeš i ovako (C++17): [*this] -- kopija celog objekta u lambdi.

private:
    int threshold_;
};

int main() {
    std::cout << std::boolalpha;
    Thermostat ts(20);
    auto check = ts.makeCheck();
    std::cout << "check created with threshold " << ts.threshold() << '\n';
    ts.setThreshold(10);
    std::cout << "threshold now " << ts.threshold() << ", 15 below threshold: " << check(15) << '\n';
}
