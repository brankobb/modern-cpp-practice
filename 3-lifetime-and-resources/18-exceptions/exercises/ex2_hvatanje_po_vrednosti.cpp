// KIND: why
// DEMO-OUT: NAIVNO spolja: opšta greška
//
// Zadatak 2 -- zašto catch po const& i "throw;" (sekcije 1, 5)
// Rešenje: exercises/solutions/ex2_hvatanje_po_vrednosti.cpp
//
// obradi() uhvati grešku samo da je zabeleži, pa je prosledi dalje.
// Spolja se očekuje GreskaSenzora (sa id-jem senzora).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/18-exceptions/exercises/ex2_hvatanje_po_vrednosti.cpp -DNAIVNO
//   Spolja stiže "opšta greška" -- GreskaSenzora je nestala.
//   a) catch (std::runtime_error e) -- PO VREDNOSTI: e je nova kopija
//      samo runtime_error dela (slicing, lekcija 16). g++ -Wall upozori
//      (-Wcatch-value), clang ćuti.
//   b) throw e; -- baca KOPIJU promenljive e, statičkog tipa
//      runtime_error. Originalni izuzetak je izgubljen.
// Korak 2: u #else grani napiši obradi() ispravno: catch po const&, i
//   "throw;" -- ponovo baca ISTI objekat, sa njegovim pravim tipom.

#include <iostream>
#include <stdexcept>
#include <string>

class GreskaSenzora : public std::runtime_error {
public:
    GreskaSenzora(int senzorId, const std::string& poruka)
        : std::runtime_error("senzor " + std::to_string(senzorId) + ": " + poruka), id_(senzorId) {}
    int id() const noexcept { return id_; }

private:
    int id_;
};

#ifdef NAIVNO
void obradi() {
    try {
        throw GreskaSenzora(7, "timeout");
    } catch (std::runtime_error e) {
        std::cout << "log: " << e.what() << '\n';
        throw e;
    }
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne radi ništa)
void obradi() {}
#endif

int main() {
    try {
        obradi();
    } catch (const GreskaSenzora& e) {
        std::cout << "spolja: GreskaSenzora, id " << e.id() << '\n';
    } catch (const std::exception& e) {
        std::cout << "spolja: opšta greška (" << e.what() << ")\n";
    }
}

/* EXPECTED OUTPUT
log: senzor 7: timeout
spolja: GreskaSenzora, id 7
*/
