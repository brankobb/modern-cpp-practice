// Rešenje zadatka ex2_range_for_copy.

#include <iostream>
#include <vector>

int copies = 0;

struct Sensor {
    double temp;
    Sensor(double t) : temp(t) {}
    Sensor(const Sensor& o) : temp(o.temp) { ++copies; }
    Sensor& operator=(const Sensor&) = default;
};

// Ako pišeš "for (auto s : v) s.temp = 0;" (nije dobro): s je kopija, pa
// se menja kopija, a original ostaje isti; uz to se plati kopija elementa.
// Treba ovako: auto& kad menjaš, const auto& kad samo čitaš.
void reset(std::vector<Sensor>& v) {
    for (auto& s : v) s.temp = 0;
}

double average(const std::vector<Sensor>& v) {
    double sum = 0;
    for (const auto& s : v) sum += s.temp;
    return sum / static_cast<double>(v.size());
}

int main() {
    std::vector<Sensor> sensors{70.0, 80.0, 90.0};
    copies = 0;
    std::cout << "average before: " << average(sensors) << '\n';
    reset(sensors);
    std::cout << "copies: " << copies << ", s[0].temp = " << sensors[0].temp << '\n';
    std::cout << "average after: " << average(sensors) << '\n';
}
