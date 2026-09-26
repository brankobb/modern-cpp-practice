// Rešenje zadatka z3_referenca_clan.

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

struct Senzor {
    std::string ime;
};

// Ako je član referenca (ili const) (nije dobro za tip koji se čuva u
// kontejneru): dodela je obrisana, pa sort, remove_if, dodela celog
// vektora... ne rade.
// Treba ovako: pokazivač (ovde ne-vlasnički, samo "pokazuje na"). Senzori
// moraju da žive duže od merenja -- isto kao i sa referencom.
// (Možeš i ovako: std::reference_wrapper<const Senzor> izvor; pa
// izvor.get().ime.)
struct Merenje {
    const Senzor* izvor;
    double vrednost;
};

int main() {
    Senzor t{"temp"}, p{"pritisak"};
    std::vector<Merenje> v{{&t, 2.0}, {&p, 1.0}, {&t, 3.0}};
    std::sort(v.begin(), v.end(),
              [](const Merenje& a, const Merenje& b) { return a.vrednost < b.vrednost; });
    const char* sep = "";
    for (const Merenje& m : v) {
        std::cout << sep << m.izvor->ime << '=' << m.vrednost;
        sep = " ";
    }
    std::cout << '\n';
}
