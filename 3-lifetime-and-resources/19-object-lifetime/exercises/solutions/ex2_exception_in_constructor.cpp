// Rešenje zadatka ex2_exception_in_constructor.

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

void calibrate(bool success) {
    if (!success) throw std::runtime_error("calibration failed");
}

class Trace {
public:
    explicit Trace(const char* name) : name_(name) { std::cout << ' ' << name_ << "()"; }
    ~Trace() { std::cout << " ~" << name_ << "()"; }
    Trace(const Trace&) = delete;
    Trace& operator=(const Trace&) = delete;

private:
    const char* name_;
};

// Ako resurse drže sirovi pokazivači, a oslobađa ih destruktor Sensor-a
// (nije dobro): kad konstruktor baci, taj destruktor se ne pozove, i sve
// što je zauzeto u konstruktoru curi.
// Treba ovako: svaki resurs drži član koji je već "ceo objekat" sa svojim
// destruktorom. Pri izuzetku se napravljeni članovi uništavaju obrnutim
// redom -- trace_ se vidi u izlazu, a vektori oslobode memoriju.
class Sensor {
public:
    explicit Sensor(bool success) : trace_("trace"), raw_(64), filtered_(64) {
        calibrate(success);
    }
    // Nema destruktora, kopija se generiše ispravno (rule of 0) -- ali
    // Trace nema kopiju, pa je nema ni Sensor.

    std::size_t size() const { return raw_.size(); }

private:
    Trace trace_;
    std::vector<int> raw_;
    std::vector<int> filtered_;
};

int main() {
    try {
        Sensor s(false);
    } catch (const std::exception& e) {
        std::cout << "\ncaught: " << e.what() << '\n';
    }
    {
        Sensor ok(true);
        std::cout << "\nok: " << ok.size() << " elements\n";
    }
    std::cout << '\n';
}
