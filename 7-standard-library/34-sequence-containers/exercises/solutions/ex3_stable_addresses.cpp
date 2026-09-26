// Rešenje zadatka ex3_stable_addresses.

#include <deque>
#include <iostream>
#include <string>
#include <vector>

struct Device {
    std::string name;
    int address;               // adresa na magistrali (npr. I2C)
    const Device* master;
};

int main() {
    // Ako čuvaš pokazivače na elemente vektora koji raste (nije dobro):
    // rast premesti elemente, pokazivači vise.
    // Treba ovako (kad pokazivači moraju da postoje): kontejner koji ne
    // premešta elemente pri dodavanju na kraj -- deque (ili list).
    // Cena: deque nema neprekidnu memoriju (nema data()), a pristup je malo
    // sporiji; list ima alokaciju po elementu i nema pristup po indeksu.
    std::deque<Device> bus;
    bus.push_back({"master", 1, nullptr});
    const Device* master = &bus.front();
    for (int i = 1; i <= 20; ++i) bus.push_back({"sensor-" + std::to_string(i), 10 + i, master});
    std::cout << bus.back().name << " -> master at address " << bus.back().master->address << '\n';
    std::cout << "devices: " << bus.size() << '\n';
}
