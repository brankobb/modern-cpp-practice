// Rešenje zadatka ex2_member_order.

#include <iostream>
#include <memory>
#include <vector>

struct Calibration {
    std::vector<double> coef;
    Calibration() : coef{2.0, 0.5} { std::cout << "Calibration\n"; }
};

struct Filter {
    double gain;
    explicit Filter(const Calibration& c) : gain(c.coef[0]) { std::cout << "Filter\n"; }
};

// Ako je filt_ deklarisan pre cal_ (nije dobro): pravi se prvi, bez obzira
// na init listu, i čita neinicijalizovanu memoriju.
// Treba ovako: član od kog drugi zavisi deklariši PRE njega, a init listu
// piši istim redom (tada -Wreorder ćuti i kod se čita kako se izvršava).
struct Sensor {
    Calibration cal_;
    Filter filt_;
    Sensor() : cal_(), filt_(cal_) {}
};

int main() {
    auto s = std::make_unique<Sensor>();
    std::cout << "gain: " << s->filt_.gain << '\n';
}
