// Rešenje zadatka ex2_move_on_forwarding.

#include <iostream>
#include <string>
#include <utility>

// Ako na forwarding referenci pišeš std::move (nije dobro): lvalue
// pozivaoca se pomeri, i on ostane sa praznim (moved-from) objektom.
// Treba ovako: std::forward<T> -- pomeri samo ako je argument bio rvalue.
struct Device {
    std::string name;
    template <typename T>
    void setName(T&& newName) { name = std::forward<T>(newName); }
};

int main() {
    Device u;
    std::string name = "temperature-sensor-01";
    u.setName(name);
    std::cout << "device: " << u.name << '\n';
    std::cout << "caller's name after: \"" << name << "\"\n";
    u.setName(std::string("pressure-sensor-02"));
    std::cout << "device: " << u.name << '\n';
}
