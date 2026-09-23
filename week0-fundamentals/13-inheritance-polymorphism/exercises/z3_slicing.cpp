// VRSTA: zašto
// DEMO-OUT: NAIVNO senzor opšti, vrednost 0
//
// Zadatak 3 -- zašto se polimorfni objekti ne čuvaju po vrednosti (sekcija 8)
// Rešenje: exercises/solutions/z3_slicing.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/13-inheritance-polymorphism/exercises/z3_slicing.cpp -DNAIVNO
//   Ubačena su dva TermoSenzor-a, a oba se ispišu kao "senzor opšti".
//   std::vector<Senzor> čuva objekte tipa TAČNO Senzor: push_back kopira
//   samo Senzor deo TermoSenzor-a (slicing). Deo sa temperaturom i vptr
//   izvedene klase se ne kopiraju -- kopija je pravi Senzor.
// Korak 2: u #else grani čuvaj senzore kao
//   std::vector<std::unique_ptr<Senzor>> i ubacuj ih sa
//   std::make_unique<TermoSenzor>(...).
// Korak 3: zaštiti Senzor od slicing-a: copy konstruktor i copy dodela u
//   Senzor-u = delete (C.67). Tada se naivni kod više ne kompajlira --
//   probaj (lekcija 13, errors/e01).

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifdef NAIVNO
struct Senzor {
    virtual ~Senzor() = default;
    virtual std::string opis() const { return "senzor opšti, vrednost 0"; }
};

struct TermoSenzor : Senzor {
    explicit TermoSenzor(double t) : temp(t) {}
    std::string opis() const override { return "termo, " + std::to_string(temp); }
    double temp;
};

int main() {
    std::vector<Senzor> senzori;
    senzori.push_back(TermoSenzor(21.5));
    senzori.push_back(TermoSenzor(30.0));
    for (const Senzor& s : senzori) std::cout << s.opis() << '\n';
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 -- otkomentariši:
    // std::vector<std::unique_ptr<Senzor>> senzori;
    // senzori.push_back(std::make_unique<TermoSenzor>(21.5));
    // senzori.push_back(std::make_unique<TermoSenzor>(30.0));
    // for (const auto& s : senzori) std::cout << s->opis() << '\n';
}
#endif

/* OČEKIVANI IZLAZ
termo, 21.5
termo, 30.0
*/
