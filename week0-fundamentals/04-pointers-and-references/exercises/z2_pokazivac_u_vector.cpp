// VRSTA: zašto
// DEMO-UB: NAIVNO heap-use-after-free
//
// Zadatak 2 -- zašto pokazivač na element vector-a "ne drži" (sekcija 12)
// Rešenje: exercises/solutions/z2_pokazivac_u_vector.cpp
//
// Registar čuva senzore u std::vector. Naivna ideja: dodaj() vrati
// pokazivač na upravo dodat senzor, pa ga pozivalac čuva kao "ručku".
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/04-pointers-and-references/exercises/z2_pokazivac_u_vector.cpp -DNAIVNO
//   ASan prijavi heap-use-after-free. Nađi u izveštaju tri steka: gde je
//   čitano, gde je memorija oslobođena (unutar push_back!) i gde je
//   alocirana. Zašto push_back oslobađa staru memoriju?
// Korak 2: u #else grani napiši isti registar, ali dodaj() vraća INDEKS
//   (std::size_t), a Senzor& uzmi(std::size_t i) vraća referencu na
//   element tek kad ti zatreba. Otkomentariši test.
// Korak 3: zašto reserve() pre dodavanja nije pravo rešenje za registar
//   koji raste neograničeno? (upiši odgovor u komentar)

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

struct Senzor {
    std::string ime;
    double temp;
};

#ifdef NAIVNO
class Registar {
public:
    Senzor* dodaj(std::string ime, double temp) {
        senzori_.push_back({std::move(ime), temp});
        return &senzori_.back();          // pokazuje u TRENUTNI blok memorije
    }
private:
    std::vector<Senzor> senzori_;
};

int main() {
    Registar r;
    Senzor* motor = r.dodaj("motor", 70.0);
    r.dodaj("baterija", 38.0);            // realokacija: motor sada visi
    motor->temp += 1.0;                   // heap-use-after-free
    std::cout << motor->temp << '\n';
}
#else
class Registar {
public:
    // TODO korak 2
};

int main() {
    // Korak 2 -- otkomentariši:
    // Registar r;
    // std::size_t motor = r.dodaj("motor", 70.0);
    // for (int i = 0; i < 100; ++i) r.dodaj("s" + std::to_string(i), i);
    // r.uzmi(motor).temp += 1.0;
    // std::cout << r.uzmi(motor).ime << ' ' << r.uzmi(motor).temp << '\n';
    // std::cout << "ukupno: " << r.broj() << '\n';
}
#endif

/* OČEKIVANI IZLAZ
motor 71
ukupno: 101
*/
