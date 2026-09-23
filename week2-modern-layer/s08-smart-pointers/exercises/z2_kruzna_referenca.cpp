// VRSTA: zašto
// DEMO-UB: NAIVNO detected memory leaks
//
// Zadatak 2 -- zašto weak_ptr za "pokazivač nazad" (sekcija 5)
// Rešenje: exercises/solutions/z2_kruzna_referenca.cpp
//
// Cvor stabla drži decu (shared_ptr), a dete pamti roditelja.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week2-modern-layer/s08-smart-pointers/exercises/z2_kruzna_referenca.cpp -DNAIVNO
//   Nijedan destruktor se ne pozove, a LeakSanitizer prijavi curenje.
//   Posle kraja bloka koren i dete drže jedan drugog: svaki ima
//   use_count 1, pa nijedan ne pada na 0. shared_ptr broji vlasnike, ne
//   traži cikluse.
// Korak 2: u #else grani napiši Cvor gde je roditelj
//   std::weak_ptr<Cvor> -- dete POSMATRA roditelja, ne poseduje ga.
//   Vlasništvo ide samo nadole (roditelj -> deca). Metoda
//   std::string imeRoditelja() const vraća ime roditelja preko lock(), ili
//   "(nema)".

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifdef NAIVNO
struct Cvor {
    explicit Cvor(std::string i) : ime(std::move(i)) {}
    ~Cvor() { std::cout << "~Cvor(" << ime << ")\n"; }
    std::string ime;
    std::shared_ptr<Cvor> roditelj;                  // vlasništvo nagore: ciklus
    std::vector<std::shared_ptr<Cvor>> deca;
};

int main() {
    // Tri stabla, a ne jedno: LeakSanitizer je konzervativan -- zaostala
    // kopija pokazivača na steku (od već uništenog shared_ptr-a) može da
    // mu "sakrije" poslednje stablo. Sa clang-om se to dešava otprilike u
    // pola pokretanja; prva dva stabla prijavi uvek (provereno). Isto radi
    // ub/u01_ciklus_shared_ptr.
    for (int i = 0; i < 3; ++i) {
        auto koren = std::make_shared<Cvor>("koren");
        auto dete = std::make_shared<Cvor>("dete");
        dete->roditelj = koren;
        koren->deca.push_back(dete);
        if (i == 0) std::cout << "koren use_count: " << koren.use_count() << '\n';
    }
    std::cout << "kraj bloka\n";
}
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // {
    //     auto koren = std::make_shared<Cvor>("koren");
    //     auto dete = std::make_shared<Cvor>("dete");
    //     dete->roditelj = koren;
    //     koren->deca.push_back(dete);
    //     std::cout << "koren use_count: " << koren.use_count() << '\n';
    //     std::cout << "roditelj deteta: " << dete->imeRoditelja() << '\n';
    // }
    // std::cout << "kraj bloka\n";
}
#endif

/* OČEKIVANI IZLAZ
koren use_count: 1
roditelj deteta: koren
~Cvor(koren)
~Cvor(dete)
kraj bloka
*/
