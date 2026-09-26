// KIND: why
// DEMO-ERR: NAIVNO use of deleted function|implicitly deleted
//
// Zadatak 3 -- zašto član-referenca ukida dodelu (sekcija 3)
// Rešenje: exercises/solutions/ex3_referenca_clan.cpp
//
// Merenje pamti iz kog senzora je stiglo (referenca na Senzor) i vrednost.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/20-copying/exercises/ex3_referenca_clan.cpp -DNAIVNO
//   std::sort mora da DODELJUJE elemente, a Merenje nema dodelu:
//   referenca se ne može preusmeriti, pa kompajler dodelu obriše
//   ([class.copy.assign]). Greška se pojavi duboko u <algorithm>; traži
//   red sa "deleted" i "Merenje".
//   (Isto važi i za const član.)
// Korak 2: u #else grani zameni referencu pokazivačem na const Senzor
//   (const Senzor* izvor) -- pokazivač se može preusmeriti, pa dodela
//   postoji. Otkomentariši test.
// Korak 3: (možeš i ovako) std::reference_wrapper<const Senzor> iz
//   <functional> -- ponaša se kao referenca (ne može biti null), a može da
//   se dodeli. Pristup: izvor.get().ime.

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

struct Senzor {
    std::string ime;
};

#ifdef NAIVNO
struct Merenje {
    const Senzor& izvor;
    double vrednost;
};

int main() {
    Senzor t{"temp"};
    std::vector<Merenje> v{{t, 2.0}, {t, 1.0}};
    std::sort(v.begin(), v.end(),
              [](const Merenje& a, const Merenje& b) { return a.vrednost < b.vrednost; });
}
#else
// TODO korak 2 (ili 3)

int main() {
    // Korak 2 -- otkomentariši:
    // Senzor t{"temp"}, p{"pritisak"};
    // std::vector<Merenje> v{{&t, 2.0}, {&p, 1.0}, {&t, 3.0}};
    // std::sort(v.begin(), v.end(),
    //           [](const Merenje& a, const Merenje& b) { return a.vrednost < b.vrednost; });
    // const char* sep = "";
    // for (const Merenje& m : v) {
    //     std::cout << sep << m.izvor->ime << '=' << m.vrednost;
    //     sep = " ";
    // }
    // std::cout << '\n';
}
#endif

/* EXPECTED OUTPUT
pritisak=1 temp=2 temp=3
*/
