// Rešenje zadatka ex3_reference_member.

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

struct Sensor {
    std::string name;
};

// Ako je član referenca (ili const) (nije dobro za tip koji se čuva u
// kontejneru): dodela je obrisana, pa sort, remove_if, dodela celog
// vektora... ne rade.
// Treba ovako: pokazivač (ovde ne-vlasnički, samo "pokazuje na"). Senzori
// moraju da žive duže od merenja -- isto kao i sa referencom.
// (Možeš i ovako: std::reference_wrapper<const Sensor> source; pa
// source.get().name.)
struct Reading {
    const Sensor* source;
    double value;
};

int main() {
    Sensor t{"temp"}, p{"pressure"};
    std::vector<Reading> v{{&t, 2.0}, {&p, 1.0}, {&t, 3.0}};
    std::sort(v.begin(), v.end(),
              [](const Reading& a, const Reading& b) { return a.value < b.value; });
    const char* sep = "";
    for (const Reading& r : v) {
        std::cout << sep << r.source->name << '=' << r.value;
        sep = " ";
    }
    std::cout << '\n';
}
