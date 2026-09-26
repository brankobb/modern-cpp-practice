// Rešenje zadatka ex2_catch_by_value.

#include <iostream>
#include <stdexcept>
#include <string>

class SensorError : public std::runtime_error {
public:
    SensorError(int sensorId, const std::string& message)
        : std::runtime_error("sensor " + std::to_string(sensorId) + ": " + message), id_(sensorId) {}
    int id() const noexcept { return id_; }

private:
    int id_;
};

// Ako hvataš po vrednosti i prosleđuješ sa "throw e;" (nije dobro): obe
// operacije prave kopiju tipa runtime_error, pa pravi tip izuzetka nestane.
// Treba ovako: catch po const& (bez kopije, bez slicing-a) i "throw;"
// (isti objekat ide dalje).
void handle() {
    try {
        throw SensorError(7, "timeout");
    } catch (const std::runtime_error& e) {
        std::cout << "log: " << e.what() << '\n';
        throw;
    }
}

int main() {
    try {
        handle();
    } catch (const SensorError& e) {
        std::cout << "outside: SensorError, id " << e.id() << '\n';
    } catch (const std::exception& e) {
        std::cout << "outside: general error (" << e.what() << ")\n";
    }
}
