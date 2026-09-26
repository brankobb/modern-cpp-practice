// KIND: why
// DEMO-ERR: NAIVE use of deleted function|implicitly deleted
//
// Zadatak 3 -- zašto član-referenca ukida dodelu (sekcija 3)
// Rešenje: exercises/solutions/ex3_reference_member.cpp
//
// Reading pamti iz kog senzora je stiglo (referenca na Sensor) i vrednost.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/20-copying/exercises/ex3_reference_member.cpp -DNAIVE
//   std::sort mora da DODELJUJE elemente, a Reading nema dodelu:
//   referenca se ne može preusmeriti, pa kompajler dodelu obriše
//   ([class.copy.assign]). Greška se pojavi duboko u <algorithm>; traži
//   red sa "deleted" i "Reading".
//   (Isto važi i za const član.)
// Korak 2: u #else grani zameni referencu pokazivačem na const Sensor
//   (const Sensor* source) -- pokazivač se može preusmeriti, pa dodela
//   postoji. Otkomentariši test.
// Korak 3: (možeš i ovako) std::reference_wrapper<const Sensor> iz
//   <functional> -- ponaša se kao referenca (ne može biti null), a može da
//   se dodeli. Pristup: source.get().name.

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

struct Sensor {
    std::string name;
};

#ifdef NAIVE
struct Reading {
    const Sensor& source;
    double value;
};

int main() {
    Sensor t{"temp"};
    std::vector<Reading> v{{t, 2.0}, {t, 1.0}};
    std::sort(v.begin(), v.end(),
              [](const Reading& a, const Reading& b) { return a.value < b.value; });
}
#else
// TODO korak 2 (ili 3)

int main() {
    // Korak 2 -- otkomentariši:
    // Sensor t{"temp"}, p{"pressure"};
    // std::vector<Reading> v{{&t, 2.0}, {&p, 1.0}, {&t, 3.0}};
    // std::sort(v.begin(), v.end(),
    //           [](const Reading& a, const Reading& b) { return a.value < b.value; });
    // const char* sep = "";
    // for (const Reading& r : v) {
    //     std::cout << sep << r.source->name << '=' << r.value;
    //     sep = " ";
    // }
    // std::cout << '\n';
}
#endif

/* EXPECTED OUTPUT
pressure=1 temp=2 temp=3
*/
