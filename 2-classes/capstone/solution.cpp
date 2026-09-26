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
class Delta {
public:
    explicit Delta(double kelvins) : k_(kelvins) {}   // explicit: 2.0 nije Delta sam od sebe
    double kelvins() const { return k_; }

    friend Delta operator+(Delta a, Delta b) { return Delta{a.k_ + b.k_}; }
    friend bool operator==(Delta a, Delta b) { return a.k_ == b.k_; }
    friend bool operator!=(Delta a, Delta b) { return !(a == b); }
    // Formatiranje u lokalni stream: operator<< ne sme trajno da promeni
    // podešavanja tuđeg stream-a (fixed, precision, showpos).
    friend std::ostream& operator<<(std::ostream& out, Delta d) {
        std::ostringstream s;
        s << std::showpos << std::fixed << std::setprecision(1) << d.k_ << " K";
        return out << s.str();
    }

private:
    double k_;
};

// Invarijanta: nikad ispod apsolutne nule. Konstruktor je privatan; prave je
// samo fabričke funkcije, koje invarijantu proveravaju.
class Temperature {
public:
    static constexpr double kAbsoluteZero = -273.15;

    static Temperature fromCelsius(double c) { return Temperature{c}; }
    static Temperature fromKelvin(double k) { return Temperature{k + kAbsoluteZero}; }

    double celsius() const { return c_; }
    double kelvin() const { return c_ - kAbsoluteZero; }

    Temperature& operator+=(Delta d) {
        c_ += d.kelvins();
        assert(c_ >= kAbsoluteZero && "below absolute zero");
        return *this;
    }
    friend Temperature operator+(Temperature t, Delta d) { return t += d; }
    friend Delta operator-(Temperature a, Temperature b) { return Delta{a.c_ - b.c_}; }

    friend bool operator==(Temperature a, Temperature b) { return a.c_ == b.c_; }
    friend bool operator!=(Temperature a, Temperature b) { return !(a == b); }
    friend bool operator<(Temperature a, Temperature b) { return a.c_ < b.c_; }
    friend bool operator>(Temperature a, Temperature b) { return b < a; }

    friend std::ostream& operator<<(std::ostream& out, Temperature t) {
        std::ostringstream s;
        s << std::fixed << std::setprecision(1) << t.c_ << " °C";
        return out << s.str();
    }

private:
    explicit Temperature(double c) : c_(c) { assert(c_ >= kAbsoluteZero && "below absolute zero"); }
    double c_;
};

// ---------------------------------------------------------------- korak 2
class Sensor {
public:
    virtual ~Sensor() { --alive_; }                  // virtual: brisanje preko Sensor* je ispravno
    Sensor(const Sensor&) = delete;                  // polimorfni objekat se ne kopira (slicing)
    Sensor& operator=(const Sensor&) = delete;

    const std::string& name() const { return name_; }
    virtual Temperature read() = 0;
    virtual std::string describe() const { return "sensor '" + name_ + "'"; }

    static int alive() { return alive_; }

protected:
    explicit Sensor(const std::string& name) : name_(name) { ++alive_; }

private:
    std::string name_;
    static inline int alive_ = 0;                    // C++17 inline static član
};

class SimulatedSensor : public Sensor {
public:
    SimulatedSensor(const std::string& name, const std::vector<double>& celsiusValues)
        : Sensor(name), values_(celsiusValues) {
        assert(!values_.empty());
    }
    Temperature read() override {
        const double c = values_[next_];
        next_ = (next_ + 1) % values_.size();
        return Temperature::fromCelsius(c);
    }
    std::string describe() const override {
        return Sensor::describe() + ", simulated, " + std::to_string(values_.size()) + " values";
    }

private:
    std::vector<double> values_;
    std::size_t next_ = 0;
};

class CalibratedSensor final : public SimulatedSensor {
public:
    CalibratedSensor(const std::string& name, const std::vector<double>& celsiusValues, Delta offset)
        : SimulatedSensor(name, celsiusValues), offset_(offset) {}
    Temperature read() override { return SimulatedSensor::read() + offset_; }
    std::string describe() const override { return SimulatedSensor::describe() + ", calibrated"; }
    void calibrate(Delta extra) { offset_ = offset_ + extra; }
    Delta offset() const { return offset_; }

private:
    Delta offset_;
};

// ---------------------------------------------------------------- korak 3
// Samo senzori koji umeju da se kalibrišu; ostali se preskaču.
int calibrateAll(const std::vector<Sensor*>& sensors, Delta extra) {
    int n = 0;
    for (Sensor* s : sensors) {
        if (auto* c = dynamic_cast<CalibratedSensor*>(s)) {
            c->calibrate(extra);
            ++n;
        }
    }
    return n;
}

// ---------------------------------------------------------------- korak 4
class AboveThreshold {
public:
    explicit AboveThreshold(Temperature threshold) : threshold_(threshold) {}
    bool operator()(Temperature t) const { return t > threshold_; }

private:
    Temperature threshold_;
};

void monitor(const std::vector<Sensor*>& sensors, int cycles, const AboveThreshold& alarm) {
    for (Sensor* s : sensors) {
        const Temperature first = s->read();
        Temperature highest = first;
        int alarms = alarm(first) ? 1 : 0;
        for (int i = 1; i < cycles; ++i) {
            const Temperature t = s->read();
            if (t > highest) highest = t;
            if (alarm(t)) ++alarms;
        }
        std::cout << std::left << std::setw(8) << s->name() << std::right << " max " << highest << ", rise from first "
                  << (highest - first) << ", alarms " << alarms << '\n';
    }
}

int main() {
    std::cout << "== step 1: temperature and delta\n";
    const Temperature t = Temperature::fromCelsius(21.5);
    const Temperature t2 = t + Delta{2.0};
    std::cout << t << " + 2 K = " << t2 << "; delta " << (t2 - t) << "; kelvin " << t2.kelvin() << '\n';
    std::cout << std::boolalpha << "t < t2: " << (t < t2) << ", t == 21.5 °C: " << (t == Temperature::fromCelsius(21.5))
              << ", 273.15 K = " << Temperature::fromKelvin(273.15) << '\n';

    std::cout << "== step 2: sensor hierarchy\n";
    SimulatedSensor hall("hall", {21.5, 22.0, 22.5});
    CalibratedSensor boiler("boiler", {78.0, 81.0, 84.0}, Delta{-1.5});
    SimulatedSensor outside("outside", {-3.0, -1.0});
    const std::vector<Sensor*> all{&hall, &boiler, &outside};   // ne poseduje: objekti su na steku
    for (const Sensor* s : all) std::cout << s->describe() << '\n';
    std::cout << "sensors alive: " << Sensor::alive() << '\n';

    std::cout << "== step 3: calibration via dynamic_cast\n";
    std::cout << "calibrated: " << calibrateAll(all, Delta{0.5}) << ", boiler offset now " << boiler.offset() << '\n';

    std::cout << "== step 4: monitoring, threshold 80 °C\n";
    monitor(all, 3, AboveThreshold{Temperature::fromCelsius(80.0)});
}
