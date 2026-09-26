// KIND: usage
// SANITIZER: thread
//
// Zadatak 1 -- paralelno čitanje senzora preko std::async, izuzeci kroz
// future, čekanje sa rokom (sekcije 1, 4, 6)
//   ./build.sh 8-concurrency/40-async-and-future/exercises/ex1_reading_sensors.cpp
//   ./build.sh 8-concurrency/40-async-and-future/exercises/ex1_reading_sensors.cpp --tsan
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_reading_sensors.cpp
//
// Korak 1: std::vector<std::future<Reading>> startReadings(const std::vector<std::string>& sensors)
//   -- za svaki senzor std::async(std::launch::async, readSensor, name).
//   (Zašto ovde ne treba std::ref, a ne bi ni smeo?)
// Korak 2: void collect(std::vector<std::future<Reading>>& tasks)
//   -- get() svakog u try/catch; ispiši "ime = vrednost" ili
//   "greška: what()", pa "uspelo N od M". Zadatak koji je bacio izuzetak
//   ne sme da obori ostale.
// Korak 3: std::optional<int> withTimeout(std::future<int>& f, std::chrono::milliseconds limit)
//   -- wait_for; ako nije ready, std::nullopt, inače f.get().
//   Test koristi promise umesto async-a: future iz async-a koji nikad ne
//   završi bi u destruktoru čekao zauvek (sekcija 4).

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

// TODO korak 1, 2, 3

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // auto tasks = startReadings({"temp", "humidity", "pressure"});
    // collect(tasks);

    // Korak 3 -- otkomentariši:
    // std::promise<int> fast, slow;
    // std::future<int> fFast = fast.get_future(), fSlow = slow.get_future();
    // fast.set_value(7);                       // spor se nikad ne postavi
    // auto a = withTimeout(fFast, std::chrono::milliseconds(20));
    // auto b = withTimeout(fSlow, std::chrono::milliseconds(20));
    // std::cout << "fast: " << (a ? std::to_string(*a) : "timed out") << ", slow: " << (b ? std::to_string(*b) : "timed out")
    //           << '\n';
}

/* EXPECTED OUTPUT
temp = 40
error: humidity: no response
pressure = 80
succeeded 2 of 3
fast: 7, slow: timed out
*/
