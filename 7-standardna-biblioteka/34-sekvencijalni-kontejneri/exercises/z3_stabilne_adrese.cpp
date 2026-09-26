// VRSTA: zašto
// DEMO-UB: NAIVNO heap-use-after-free
//
// Zadatak 3 -- zašto izbor kontejnera određuje da li pokazivači "drže"
// (sekcije 3, 4, 5)
// Rešenje: exercises/solutions/z3_stabilne_adrese.cpp
//
// Magistrala drži uređaje u kontejneru, a svaki novi uređaj dobija
// pokazivač na "master" uređaj (prvi dodat). Uređaji se dodaju stalno.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standardna-biblioteka/34-sekvencijalni-kontejneri/exercises/z3_stabilne_adrese.cpp -DNAIVNO
//   ASan: heap-use-after-free. Uređaji su u std::vector: kad vector
//   poraste, svi elementi se premeste u novi blok, a master pokazuje u
//   stari, oslobođeni blok. (Lekcija 04, z2 je ovo rešila indeksom.)
// Korak 2: u #else grani zadrži pokazivače, ali promeni kontejner:
//   std::deque -- push_back ne premešta postojeće elemente, pa pokazivači i
//   reference ostaju važeći. (std::list bi takođe radila: svaki element je
//   poseban čvor.) Koju cenu plaćaš za to u odnosu na vector? (sekcije 4, 5)

#include <deque>
#include <iostream>
#include <string>
#include <vector>

struct Uredjaj {
    std::string ime;
    int adresa;               // adresa na magistrali (npr. I2C)
    const Uredjaj* master;
};

int main() {
#ifdef NAIVNO
    std::vector<Uredjaj> magistrala;
#else
    // TODO korak 2 (ovde promeni tip kontejnera)
    std::vector<Uredjaj> magistrala;
    magistrala.reserve(100);    // privremeno, da nerešena verzija ne bi bila UB
#endif
    magistrala.push_back({"master", 1, nullptr});
    const Uredjaj* master = &magistrala.front();
    for (int i = 1; i <= 20; ++i) magistrala.push_back({"senzor-" + std::to_string(i), 10 + i, master});
    std::cout << magistrala.back().ime << " -> master na adresi " << magistrala.back().master->adresa << '\n';
    std::cout << "uređaja: " << magistrala.size() << '\n';
}

/* OČEKIVANI IZLAZ
senzor-20 -> master na adresi 1
uređaja: 21
*/
