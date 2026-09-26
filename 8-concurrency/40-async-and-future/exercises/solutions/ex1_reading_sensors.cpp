// Rešenje zadatka ex1_reading_sensors.

#include <chrono>
#include <future>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

struct Reading {
    std::string sensor;
    int value;
};

// Dato: "čitanje" jednog senzora; baca za senzor koji ne odgovara.
Reading readSensor(const std::string& name) {
    if (name == "humidity") throw std::runtime_error("humidity: no response");
    return {name, static_cast<int>(name.size()) * 10};
}

// Korak 1: jedan async zadatak po senzoru; ime se KOPIRA u zadatak.
// Sa std::ref(s) zadatak bi čitao element vektora senzori -- a u main-u
// je to privremeni vektor ({"temp", ...}), koji nestane na kraju iskaza,
// dok zadaci možda još rade. Kopija ne može da "istekne" (CP.31).
std::vector<std::future<Reading>> startReadings(const std::vector<std::string>& sensors) {
    std::vector<std::future<Reading>> f;
    for (const auto& s : sensors) f.push_back(std::async(std::launch::async, readSensor, s));
    return f;
}

// Korak 2: get() baci izuzetak iz zadatka -- ovde, u pozivaocu.
void collect(std::vector<std::future<Reading>>& tasks) {
    int succeeded = 0;
    for (auto& z : tasks) {
        try {
            Reading m = z.get();
            std::cout << m.sensor << " = " << m.value << '\n';
            ++succeeded;
        } catch (const std::exception& e) {
            std::cout << "error: " << e.what() << '\n';
        }
    }
    std::cout << "succeeded " << succeeded << " of " << tasks.size() << '\n';
}

// Korak 3: wait_for ne uzima rezultat; get samo ako je spreman.
std::optional<int> withTimeout(std::future<int>& f, std::chrono::milliseconds limit) {
    if (f.wait_for(limit) != std::future_status::ready) return std::nullopt;
    return f.get();
}

int main() {
    auto tasks = startReadings({"temp", "humidity", "pressure"});
    collect(tasks);

    std::promise<int> fast, slow;
    std::future<int> fFast = fast.get_future(), fSlow = slow.get_future();
    fast.set_value(7);                       // spor se nikad ne postavi
    auto a = withTimeout(fFast, std::chrono::milliseconds(20));
    auto b = withTimeout(fSlow, std::chrono::milliseconds(20));
    std::cout << "fast: " << (a ? std::to_string(*a) : "timed out") << ", slow: " << (b ? std::to_string(*b) : "timed out")
              << '\n';
}
