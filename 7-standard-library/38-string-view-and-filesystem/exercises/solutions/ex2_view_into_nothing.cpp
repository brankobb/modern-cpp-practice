// Rešenje zadatka ex2_view_into_nothing.

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// Ako je član string_view (nije dobro): gleda u lokalni string funkcije
// napravi(), koji nestane na njenom izlasku -- pogled visi.
// Treba ovako: objekat koji čuva podatak ga i poseduje -- std::string.
struct Channel {
    std::string name;
    int stamp;
};

Channel make(int stamp) { return Channel{"temperature_sensor_hall_" + std::to_string(stamp), stamp}; }

int main() {
    std::vector<Channel> channels;
    for (int i = 1; i <= 2; ++i) channels.push_back(make(i));
    for (const auto& k : channels) std::cout << k.stamp << ": " << k.name << '\n';
}
